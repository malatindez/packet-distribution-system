#include "session.hpp"

namespace node_system
{
    Session::Session(boost::asio::io_context& io, boost::asio::ip::tcp::socket&& socket) : socket_(std::move(socket)), received_packets_{8192}, packets_to_send_{8192}
    {
        receive_all();
        // call async packet forger
        if (alive_ = socket_.is_open();
            alive_)
            spdlog::info("Session created");

        co_spawn(socket_.get_executor(), std::bind(&Session::async_packet_forger, this, std::ref(io)), boost::asio::detached);
        co_spawn(socket_.get_executor(), std::bind(&Session::send_all, this, std::ref(io)), boost::asio::detached);
        for(size_t i = 0; i < 4; i++)
        {
            co_spawn(socket_.get_executor(), std::bind(&Session::async_packet_sender, this, std::ref(io)), boost::asio::detached);
        }
        //            co_spawn(socket_.get_executor(), std::bind(&Session::read_all, this), boost::asio::detached);

    }

    std::unique_ptr<Packet> Session::pop_packet_now()
    {
        if (const std::unique_ptr<ByteArray> packet_data = pop_packet_data();
            packet_data)
        {
            if (aes_)
            {
                const ByteArray plain = decrypt(*packet_data);
                const uint32_t packet_type = bytes_to_uint32(plain.view(0, 4));
                return packet::PacketFactory::Deserialize(plain.view(4), packet_type);
            }
            const uint32_t packet_type = bytes_to_uint32(packet_data->view(0, 4));
            return packet::PacketFactory::Deserialize(packet_data->view(4), packet_type);
        }
        return nullptr;
    }

    boost::asio::awaitable<std::unique_ptr<Packet>> Session::pop_packet_async(boost::asio::io_context& io)
    {
        ExponentialBackoffUs backoff(std::chrono::microseconds(1), std::chrono::microseconds(1000 * 50), 2, 0.1);
        while (this->alive_)
        {
            std::unique_ptr<Packet> packet = pop_packet_now();

            if (packet)
            {
                co_return packet;
            }

            boost::asio::steady_timer timer(io, backoff.get_current_delay());
            co_await timer.async_wait(boost::asio::use_awaitable);
            backoff.increase_delay();
        }
        co_return nullptr;
    }

    std::unique_ptr<ByteArray> Session::pop_packet_data() noexcept
    {
        ByteArray *packet = nullptr;
        received_packets_.pop(packet);
        return std::unique_ptr<ByteArray>(packet);
    }

    void Session::receive_all()
    {
        boost::asio::async_read(socket_, buffer_, boost::asio::transfer_all(),
            [this](const boost::system::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (ec)
                {
                    spdlog::warn("Error reading message: {}", ec.message());
                    socket_.close();
                    alive_ = false;
                    packets_to_send_.consume_all([](ByteArray *value){ if(value != nullptr) delete value; });
                }
                else
                {
                    spdlog::info("Received total of {} bytes", length);
                }
            });
    }
    boost::asio::awaitable<std::shared_ptr<Session>> Session::get_shared_ptr(boost::asio::io_context& io)
    {
        ExponentialBackoffUs backoff(std::chrono::microseconds(1), std::chrono::microseconds(1000 * 25), 2, 32, 0.1);
        
            int it = 0;
            do
            {
                try {
                    co_return shared_from_this();
                } 
                catch (std::bad_weak_ptr&) 
                {
                    it++;
                }
                boost::asio::steady_timer timer(io, backoff.get_current_delay());
                co_await timer.async_wait(boost::asio::use_awaitable); 
                backoff.increase_delay();
                if(it >= 100 && it % 20 == 0)
                {
                    spdlog::error("Failed to retrieve pointer {} times", it);
                }
            } while(it <= 200);
        co_return nullptr;
    }
    boost::asio::awaitable<void> Session::send_all(boost::asio::io_context& io)
    {
        bool writing = false;
        ByteArray data_to_send;
        // TODO: make these configurable per session
        // 64 Kb, no reason to allocate more per session
        const uint32_t kDefaultDataToSendSize = 1024 * 64;
        // If user somehow managed to send the packet of this size or bigger
        // we shrink the size back to kDefaultDataToSendSize.
        // If capacity of the vector is lower than this we will keep it's size.
        // This is done solely so we don't consume a lot of memory per session if we send heavy packets from time to time.
        const uint32_t kMaximumDataToSendSize = 1024 * 1024 * 1;
        data_to_send.reserve(kDefaultDataToSendSize);
        std::shared_ptr<Session> session_lock = co_await get_shared_ptr(io);
        if(session_lock == nullptr)
        {
            spdlog::error("Couldn't retrieve shared pointer for session. Did you create the session using std::make_shared?");
            co_return;
        }

        ExponentialBackoffUs backoff(std::chrono::microseconds(1), std::chrono::microseconds(1000 * 25), 2, 32, 0.1);
        while (alive_)
        {
            if (!packets_to_send_.empty() && !writing)
            {
                writing = true;
                {
                    data_to_send.clear(); 

                    if(data_to_send.capacity() >= kMaximumDataToSendSize)
                    {
                        data_to_send.shrink_to_fit();
                    }

                    ByteArray *packet = nullptr;
                    for (int i = 0; (i < 1000 && data_to_send.size() < kDefaultDataToSendSize) &&       
                        packets_to_send_.pop(packet);
                        i++)
                    {
                        data_to_send.append(uint32_to_bytes(static_cast<uint32_t>(packet->size())));
                        data_to_send.append(*packet);
                    }
                    if (packet != nullptr)
                    {
                        delete packet;
                    }
                }

                async_write(socket_, boost::asio::buffer(data_to_send.as<char>(), data_to_send.size()),
                    [&](const boost::system::error_code ec, [[maybe_unused]] std::size_t length)
                    {
                        writing = false;
                        data_to_send.clear();
                        if (ec) { spdlog::warn("Error sending message: {}", ec.message()); }
                    }
                );
                backoff.decrease_delay();
                continue;
            }
            boost::asio::steady_timer timer(io, backoff.get_current_delay());
            co_await timer.async_wait(boost::asio::use_awaitable);
            backoff.increase_delay();
        }
    }

    boost::asio::awaitable<void> Session::async_packet_forger(boost::asio::io_context& io)
    {
        ExponentialBackoffUs backoff(std::chrono::microseconds(1), std::chrono::microseconds(1000 * 50), 2, 32, 0.1);
        std::shared_ptr<Session> session_lock = co_await get_shared_ptr(io);
        if(session_lock == nullptr)
        {
            spdlog::error("Couldn't retrieve shared pointer for session. Did you create the session using std::make_shared?");
            co_return;
        }
        while (alive_)
        {
            if (buffer_.size() >= 4)
            {
                ByteArray packet_size_data;
                read_bytes_to(packet_size_data, 4);
                const int64_t packet_size = bytes_to_uint32(packet_size_data);
                // TODO: add a system that ensures that packet data size is correct.
                // TODO: handle exception, and if packet size is too big we need to do something about it.
                utils::AlwaysAssert(packet_size != 0 && packet_size < 1024ULL * 1024 * 1024 * 4, "The amount of bytes to read is too big. 4GB? What are you transfering? Anyways, it seems to be a bug.");
                
                while (static_cast<int64_t>(buffer_.size()) < packet_size && alive_)
                {
                    boost::asio::steady_timer timer(io, backoff.get_current_delay());
                    co_await timer.async_wait(boost::asio::use_awaitable);
                    backoff.increase_delay();
                }
                if (static_cast<int64_t>(buffer_.size()) < packet_size) 
                // While loop waits until requirement is satisfied, so if it's false then alive_ is false and session is dead, so we won't get any data anymore
                {
                    break;
                }

                ByteArray *packet_data = new ByteArray;
                read_bytes_to(*packet_data, packet_size);
                while(!received_packets_.push(packet_data))
                {
                    boost::asio::steady_timer timer(io, std::chrono::microseconds(1000));
                    co_await timer.async_wait(boost::asio::use_awaitable);
                }
                backoff.decrease_delay();
                continue;
            }
            boost::asio::steady_timer timer(io, backoff.get_current_delay());
            co_await timer.async_wait(boost::asio::use_awaitable);
            backoff.increase_delay();
        }
    }
    
    boost::asio::awaitable<void> Session::async_packet_sender(boost::asio::io_context& io)
    {
        ExponentialBackoffUs backoff(
            std::chrono::microseconds(1), 
            std::chrono::microseconds(1000 * 500), 
            2, 
            64, 
            0.1);
        
        std::shared_ptr<Session> session_lock = co_await get_shared_ptr(io);
        if(session_lock == nullptr)
        {
            spdlog::error("Couldn't retrieve shared pointer for session. Did you create the session using std::make_shared?");
            co_return;
        }
        while (alive_)
        {
            ByteArray *packet = nullptr;
            while((!bool(packet_receiver_) || !received_packets_.pop(packet)))
            {
                if(!alive_)
                {
                    break;
                }
                boost::asio::steady_timer timer(io, backoff.get_current_delay());
                co_await timer.async_wait(boost::asio::use_awaitable);
                backoff.increase_delay();
            }
            
            packet_receiver_(std::unique_ptr<ByteArray>(packet));
            backoff.decrease_delay();
        }
    }
    
}