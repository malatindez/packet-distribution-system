#pragma once
#include "packet.hpp"
#include <queue>
#include <deque>
#include <unordered_map>
#include <future>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/bind/bind.hpp>

namespace node_system
{
    // If function returns false that means that the packet was discarded and we should pass it on.
    template <typename DerivedPacket>
    using PacketHandlerFunc = std::function<bool(std::unique_ptr<DerivedPacket>)>;
    
    template <typename DerivedPacket>
    using PacketFilterFunc = std::function<bool(DerivedPacket const &)>;
    /**
     * @brief Each instance of the PacketQueue class should be associated with a specific io_context. 
     * When you create an instance of the PacketQueue, pass the appropriate io_context to its constructor.
     */
    class PacketQueue
    {
    public:
        using BasePacketPtr = std::unique_ptr<Packet>;
        template<typename T>
        using shared_promise = std::shared_ptr<std::promise<T>>;

        using shared_packet_promise = shared_promise<BasePacketPtr>;
        
        using filter_promise = std::pair<std::function<bool(BasePacketPtr const &)>, shared_packet_promise>;

        PacketQueue(boost::asio::io_context &io_context) 
                : fulfill_promises_strand_{io_context},
                  enqueue_packet_strand_{io_context } {}

        void enqueue(BasePacketPtr packet)
        {
            fulfill_promises_strand_.post(
                [this, &packet]()
                {
                    if (fulfill_promises(packet))
                    {
                        packet = nullptr;
                    }
                }
            );
            if (packet)
            {
                enqueue_packet_strand_.post(
                    [this, &packet]()
                    {
                        enqueue_packet(std::move(packet));
                    }
                );
            }
        }

        template <IsPacket DerivedPacket>
        boost::asio::awaitable<std::unique_ptr<DerivedPacket>> await_for_packet()
        {
            auto packetType = DerivedPacket::static_type;
            auto promise = std::make_shared<std::promise<BasePacketPtr>>();

            promise_map_[packetType].push_back(promise);

            auto future = promise->get_future();
            co_await boost::asio::this_coro::executor;

            co_return std::dynamic_pointer_cast<DerivedPacket>(std::future.get());
        }
        template <IsPacket DerivedPacket>
        boost::asio::awaitable<std::unique_ptr<DerivedPacket>> await_for_packet(PacketFilterFunc<DerivedPacket> filter)
        {
            auto packetType = DerivedPacket::static_type;
            auto promise = std::make_shared<std::promise<BasePacketPtr>>();
            {
                filter_promise_map_[packetType].emplace_back(
                    [passedFilter = filter](BasePacketPtr const& packet) 
                    {
                        return passedFilter(*reinterpret_cast<DerivedPacket*>(packet.get()));
                    },
                    promise);
            }
            auto future = promise->get_future();
            co_await boost::asio::this_coro::executor;

            co_return std::dynamic_pointer_cast<DerivedPacket>(future.get());
        }
        
        /**
         * @brief Registers default handler for provided packet.
         * 
         * @tparam DerivedPacket The type of packet that we should register handler for.
         * @param handler PacketHandlerFunc. If it returns false, we should pass the packet to the next handler.
         * @param delay when we should pass the packet to the handler. This delay can be used so if we got packet before promise was requested it won't immediately go to the handler.
         */
        template <IsPacket DerivedPacket>
        void register_default_handler(PacketHandlerFunc<DerivedPacket> handler, float delay = 0.0f)
        {
            enqueue_packet_strand_.post([this, movedHandler = std::move(handler), delay]() {
                auto packetType = DerivedPacket::static_type;
                default_handlers_[packetType] = { std::move(movedHandler), handler };
                });
        }

    private:
    
        boost::asio::awaitable<void> Run()
        {
            while (true)
            {
                // TODO
            }
        }

        bool fulfill_promises(BasePacketPtr &packet) {
            auto packetType = packet->type;
            
            // Fulfill promises in filter_promise_map
            {
                auto it = filter_promise_map_.find(packetType);
                if (it != filter_promise_map_.end())
                {   
                    for (auto &filterPromise : it->second)
                    {
                        if (!filterPromise.first || filterPromise.first(packet))
                        {
                            filterPromise.second->set_value(std::move(packet));
                            return true;
                        }
                    }
                }
            }

            {
                // Fulfill the first promise in promise_map
                auto promiseIt = promise_map_.find(packetType);
                if (promiseIt != promise_map_.end() && !promiseIt->second.empty())
                {
                    promiseIt->second.front()->set_value(std::move(packet));
                    promiseIt->second.pop_front();
                    return true;
                }

            }
            return false;
        }

        void enqueue_packet(BasePacketPtr &&packet)
        {    
            unprocessed_packets_[packet->type].push(std::move(packet));
        }

        boost::asio::io_context::strand fulfill_promises_strand_;
        boost::asio::io_context::strand enqueue_packet_strand_;

        std::unordered_map<UniquePacketID, std::queue<BasePacketPtr>> unprocessed_packets_;
        std::unordered_map<UniquePacketID, std::deque<shared_packet_promise>> promise_map_;
        std::unordered_map<UniquePacketID, std::deque<filter_promise>> filter_promise_map_;
        std::unordered_map<UniquePacketID, std::vector<std::pair<float, PacketHandlerFunc<Packet>>>> default_handlers_;
    };
}