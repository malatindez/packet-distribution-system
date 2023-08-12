#pragma once
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/endian/conversion.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/asio/spawn.hpp>
#include <memory>
#include <queue>
#include <algorithm>
#include <mutex>

#include <boost/lockfree/queue.hpp>

#include "packet.hpp"
#include "backoffs.hpp"
#include "crypto/aes.hpp"
#include "utils/utils.hpp"
#include "../include/spdlog.hpp"

namespace node_system
{
    using PacketReceiverFn = std::function<void(std::unique_ptr<Packet> &&)>;
    /**
     * @brief Represents a network session for sending and receiving packets.
     * @details To correctly destroy this object, you need to call Destroy function, because coroutines share the object from this.
     */
    class Session : public utils::non_copyable_non_movable, public std::enable_shared_from_this<Session>
    {
    public:
        explicit Session(boost::asio::io_context& io, boost::asio::ip::tcp::socket&& socket);
        virtual ~Session()
        {
            packets_to_send_.consume_all([](ByteArray *value){ if (value != nullptr)  delete value; });
            received_packets_.consume_all([](ByteArray *value){ if (value != nullptr) delete value; });
        }
        /**
         * @brief Sends any packet derived from DerivedPacket through the network.
         * 
         * @tparam T Final packet type. 
         * (Template functions cannot be overriden, we need to call serialize from the furthest child.)
         * 
         * @note Blockable until packets_to_send_ can retrieve the value.
         * 
         * @param packet_arg Packet value
         * @return true if session got the packet.
         * @return false if session was closed.
         */
        template<typename T>
        bool send_packet(const T& packet_arg) requires std::is_base_of_v<Packet, T>
        {
            if (!alive_)
            {
                spdlog::warn("Session is closed, cannot send packet");
                return false;
            }
            const auto& packet = static_cast<const Packet&>(packet_arg);
            ByteArray buffer = ByteArray{ uint32_to_bytes(packet.type) };
            packet.serialize(buffer);
            if (aes_)
            {
                buffer = encrypt(buffer);
            }
            // byte to check if connection is secured or not.
            buffer.insert(buffer.begin(), aes_ ? std::byte{1} : std::byte{0});
            ByteArray *value = new ByteArray{ std::move(buffer) };
            ExponentialBackoffUs backoff(std::chrono::microseconds(1), std::chrono::microseconds(1000), 2, 1, 0.1);
            while (!packets_to_send_.push(value) || !alive_)
            {
                std::this_thread::sleep_for(backoff.get_current_delay());
                backoff.increase_delay();
            }
            if(!alive_)
            {
                delete value;
                return false;
            }
            return true;
        }
        /**
         * @brief Returns the earliest acquired packet. If packet queue is empty, returns nullptr.
         * 
         * @warn If packet receiver is set through SetPacketReceiver there's no reason to call this function.
         * Generally packets will just go through the receiver. 
         * There's no ordering neither option to configure which packet you will receive.
         * 
         * @return std::unique_ptr<Packet> 
         */
        std::unique_ptr<Packet> pop_packet_now();
        
        /**
         * Returns nullptr if socket has crashed.
         * If not, it will wait until the packet is available and will return it as soon as possible.
         * This function is threadsafe.
         */
        boost::asio::awaitable<std::unique_ptr<Packet>> pop_packet_async(boost::asio::io_context& io);

        [[nodiscard]] bool has_packets()
        {
            return !received_packets_.empty();
        }

        void setup_encryption(ByteArray key, ByteArray salt, short n_rounds)
        {
            aes_ = std::make_unique<crypto::AES::AES256>(key, salt, n_rounds);
        }

        [[nodiscard]] bool secured() const noexcept { return aes_ != nullptr; }
        [[nodiscard]] bool is_closed() const noexcept { return !alive_; }
        [[nodiscard]] bool alive() const noexcept { return alive_; }
         /**
         * @brief Sends a packet through the network.
         * @tparam T Final packet type.
         * @param packet_arg Packet value.
         * @return true if the session successfully sent the packet, false if the session is closed.
         */
        void SetPacketReceiver(PacketReceiverFn const receiver)
        {
            std::lock_guard guard{packet_receiver_mutex_};
            packet_receiver_ = receiver;
        }
        /**
         * @brief Coroutines use the shared pointer from this, so you need to explicitly call Destroy so alive_ is false. 
         * This way coroutines can end and unlock the remaining instances of shared_ptr.
         */
        void Destroy() { alive_ = false; }

    protected:
        std::unique_ptr<ByteArray> pop_packet_data() noexcept;
    private:
        boost::asio::awaitable<std::shared_ptr<Session>> get_shared_ptr(boost::asio::io_context& io);
        void receive_all();
        boost::asio::awaitable<void> send_all(boost::asio::io_context& io);
        boost::asio::awaitable<void> async_packet_forger(boost::asio::io_context& io);
        boost::asio::awaitable<void> async_packet_sender(boost::asio::io_context& io);
        void read_bytes_to(ByteArray& byte_array, const size_t amount)
        {
            const size_t current_size = byte_array.size();
            byte_array.resize(current_size + amount);
            buffer_.sgetn(byte_array.as<char>() + current_size * sizeof(char), amount);
        }

        [[nodiscard]] static uint32_t bytes_to_uint32(const ByteView byte_view)
        {
            utils::Assert(byte_view.size() >= 4, "The byte array is too small to be converted to a uint32_t");
            return boost::endian::little_to_native(*reinterpret_cast<const uint32_t*>(byte_view.data()));
        }
        [[nodiscard]] static ByteArray uint32_to_bytes(const uint32_t value)
        {
            ByteArray byte_array(4);
            *byte_array.as<uint32_t>() = boost::endian::native_to_little(value);
            return byte_array;
        }
        /**
         * @brief Public method to encrypt bytearray using locally stored AES256 pointer.
         * 
         * @note will throw if aes_ is nullptr
         * 
         * @param data input plaintext
         * @return ByteArray output ciphertext
         */
        [[nodiscard]] ByteArray encrypt(const ByteView data) const
        {
            return aes_->encrypt(data);
        }
        /**
         * @brief Public method to decrypt bytearray using locally stored AES256 pointer.
         * 
         * @note will throw if aes_ is nullptr
         * 
         * @param data input ciphertext
         * @return ByteArray output plaintext
         */
        [[nodiscard]] ByteArray decrypt(const ByteView data) const
        {
            return aes_->decrypt(data);
        }
        /**
         * @brief Packets stored in this structure should be created using new
         * After popping the pointer you can either delete it, or wrap in smart pointers.
         * Either way before pushing you should release the smart pointer, because otherwise it would lead to undefined behaviour.
         *
         * @todo: circular buffer and ByteView handler for lockfree queue.
         * Because lockfree queue requires type to have trivial ctor/dtor the following requirements should be followed:
         * The byteview handler should hold simple pointer to circular buffer and byteview.
         * Circular buffer should automatically free memory allocated by ByteView after calling ByteViewHandler::free(); 
         * This way we can free memory from the buffer after acquiring the data and processing the packet
         * without actually acquiring/deleting memory from the OS and creating new instance on the heap each time.
         *
         * This is the one way. The other is to use default queue of shared pointers which will automatically call free.
         * This guarantees the deallocation of memory, but may result in lower performance. We need to test it out.
         * 
         * Right now this library is proof-of-concept, but it's a really important TODO.
         */
        boost::lockfree::queue<ByteArray*, boost::lockfree::fixed_sized<true>> received_packets_;
        boost::lockfree::queue<ByteArray*, boost::lockfree::fixed_sized<true>> packets_to_send_;

        bool alive_ = true;
        boost::asio::streambuf buffer_;
        boost::asio::ip::tcp::tcp::socket socket_;
        /**
         * @brief AES encryption holder.
         * @todo Add abstract encryption class that allows overloading of encrypt/decrypt from ByteView. 
         */
        std::unique_ptr<crypto::AES::AES256> aes_ = nullptr;

        std::mutex packet_receiver_mutex_;
        PacketReceiverFn packet_receiver_;
    };
}