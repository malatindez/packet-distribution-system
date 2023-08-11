#pragma once
#include "backoffs.hpp"
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
#include <boost/asio/steady_timer.hpp>
#include "include/spdlog.hpp"
namespace node_system
{
    // If function returns false that means that the packet was discarded and we should pass it on.
    template <typename DerivedPacket>
    using PacketHandlerFunc = std::function<boost::asio::awaitable<void>(std::unique_ptr<DerivedPacket>)>;
    
    template <typename DerivedPacket>
    using PacketFilterFunc = std::function<bool(DerivedPacket const &)>;
    /**
     * @brief Each instance of the PacketDispatcher class should be associated with a specific io_context. 
     * When you create an instance of the PacketDispatcher, pass the appropriate io_context to its constructor.
     */
    class PacketDispatcher final
    {
    public:
        using BasePacketPtr = std::unique_ptr<Packet>;
        template<typename T>
        using shared_promise = std::shared_ptr<std::promise<T>>;

        using shared_packet_promise = shared_promise<BasePacketPtr>;
        
        using promise_filter = std::pair<std::function<bool(BasePacketPtr const &)>, shared_packet_promise>;

        using handler_tuple = std::tuple<float, PacketFilterFunc<Packet>, PacketHandlerFunc<Packet>>;

        PacketDispatcher(boost::asio::io_context &io_context) 
        :   unprocessed_packets_input_strand_{io_context}, 
            promise_map_input_strand_{io_context}, 
            promise_filter_map_input_strand_{io_context},
            default_handlers_input_strand_{io_context}
         {}



        inline void enqueue_packet(BasePacketPtr &&packet)
        {
            push_packet(std::move(packet));
        }

        /**
         * @brief Wait until the packet is registered in dispatch system and returns as soon as possible.
         * 
         * @tparam DerivedPacket type of packet you want to wait for.
         * @param timeout If less or equal to zero the function won't return until promise is fulfilled. Otherwise, it will wait for timeout to retun.
         * @return boost::asio::awaitable<std::unique_ptr<DerivedPacket>> the packet
         */
        template <IsPacket DerivedPacket>
        boost::asio::awaitable<std::unique_ptr<DerivedPacket>> await_packet(float timeout = -1.0f)
        {
            auto packet_type = DerivedPacket::static_type;
            auto promise = std::make_shared<std::promise<BasePacketPtr>>();
            enqueue_promise(packet_type, promise);
            auto future = promise->get_future();
            co_await boost::asio::this_coro::executor;
            spdlog::debug("Waiting for packet: {}", DerivedPacket::static_type);
            if(timeout <= 0)
            {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket*>(base.release()));
            }
        
            std::future_status status = future.wait_for(std::chrono::microseconds(size_t(timeout * 1e6f)));

            if (status == std::future_status::ready) {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket*>(base.release()));
            } else if (status == std::future_status::timeout) {
                co_return nullptr;
            }
        }
        
        /**
         * @brief Wait until the packet is registered in dispatch system and returns as soon as possible.
         * 
         * @tparam DerivedPacket type of packet you want to wait for.
         * @param filter Function to filter the packet. If functor returns true the packet will fulfill the promise.
         * @param timeout If less or equal to zero the function won't return until promise is fulfilled. Otherwise, it will wait for timeout to retun.
         * @return boost::asio::awaitable<std::unique_ptr<DerivedPacket>> the packet
         */
        template <IsPacket DerivedPacket>
        boost::asio::awaitable<std::unique_ptr<DerivedPacket>> await_packet(PacketFilterFunc<DerivedPacket> filter, float timeout = -1.0f)
        {
            auto packet_type = DerivedPacket::static_type;
            auto promise = std::make_shared<std::promise<BasePacketPtr>>();
            enqueue_filter_promise(
                packet_type,
                {
                    [passedFilter = filter](BasePacketPtr const& packet) 
                    {
                        return passedFilter(*reinterpret_cast<DerivedPacket*>(packet.get()));
                    },
                    promise
                }
            );
            
            auto future = promise->get_future();
            co_await boost::asio::this_coro::executor;
            spdlog::debug("Waiting for packet: {}", DerivedPacket::static_type);
            if(timeout <= 0)
            {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket*>(base.release()));
            }
        
            std::future_status status = future.wait_for(std::chrono::microseconds(size_t(timeout * 1e6f)));

            if (status == std::future_status::ready) {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket*>(base.release()));
            } else if (status == std::future_status::timeout) {
                co_return nullptr;
            }
        }
        
        
        /**
         * @brief Registers default handler for provided packet.
         * 
         * @tparam DerivedPacket The type of packet that we should register handler for.
         * @param handler PacketHandlerFunc. If it returns false, we should pass the packet to the next handler.
         * @param delay when we should pass the packet to the handler. This delay can be used so if we got packet before promise was requested it won't immediately go to the handler.
         */
        template <IsPacket DerivedPacket>
        void register_default_handler(PacketHandlerFunc<DerivedPacket> handler, PacketFilterFunc<DerivedPacket> filter = {}, float delay = 0.0f)
        {
            spdlog::debug("Posting task to register default handler for packet {}", DerivedPacket::static_type);
            default_handlers_input_strand_.post(
                [this, 
                    delay, 
                    movedFilter = std::move(filter), 
                    movedHandler = std::move(handler)
                ]() __lambda_force_inline -> void
                {
                    auto packet_id = DerivedPacket::static_type;
                    spdlog::debug("Registered default handler for packet {}!", packet_id);
                    default_handlers_[packet_id] = { delay, std::move(movedFilter), std::move(movedHandler) };
                    default_handlers_input_updated_.test_and_set(std::memory_order_release);
                });
        }

        void enqueue_promise(UniquePacketID packet_id, shared_packet_promise promise)
        {
            spdlog::debug("Posting task to enqueue promise for packet {}", packet_id);
            promise_map_input_strand_.post(
                [this, packet_id, moved_promise = std::move(promise)]() mutable
                {
                    spdlog::debug("Promise enqueued for packet {}!", packet_id);
                    promise_map_input_.emplace_back(std::pair{packet_id, std::move(moved_promise)});
                    promise_map_input_updated_.test_and_set(std::memory_order_release);
                }
            );
        }
        
        void enqueue_filter_promise(UniquePacketID packet_id, promise_filter filtered_promise)
        {
            spdlog::debug("Posting task to enqueue promise with filter for packet {}", packet_id);
            promise_filter_map_input_strand_.post(
                [this, packet_id, moved_filtered_promise = std::move(filtered_promise)]() mutable
                {
                    spdlog::debug("Promise with filter enqueued for packet {}!", packet_id);
                    promise_filter_map_input_.emplace_back(std::pair{packet_id, std::move(moved_filtered_promise)});
                    promise_filter_map_input_updated_.test_and_set(std::memory_order_release);
                }
            );
        }

    private:
    
        boost::asio::awaitable<void> Run()
        {
            ExponentialBackoffUs backoff{
                std::chrono::microseconds(1), 
                std::chrono::microseconds(1000), 
                2, 32, 0.1};
            utils::SteadyTimer timer;
            float min_handler_timestamp = std::numeric_limits<float>::max();
            while (true)
            {
                bool updated = co_await pop_inputs();
                if(!updated)
                {
                    if(min_handler_timestamp < timer.elapsed())
                    {
                        spdlog::trace("Updating handlers...");
                        min_handler_timestamp = std::numeric_limits<float>::max();
                        
                        for(auto &[packet_id, packet_vector] : unprocessed_packets_)
                        {
                            std::erase_if(packet_vector, [this, &packet_id, &min_handler_timestamp, &timer](BasePacketPtr &packet) __lambda_force_inline 
                            {
                                return fulfill_handlers(packet_id, packet, min_handler_timestamp, timer);
                            });
                        }
                    }
                    boost::asio::steady_timer(co_await boost::asio::this_coro::executor, backoff.get_current_delay());
                    backoff.increase_delay();
                    continue;
                }

                spdlog::trace("Input arrays were updated! Fetching...");

                min_handler_timestamp = std::numeric_limits<float>::max();
                for(auto &[packet_id, packet_vector] : unprocessed_packets_)
                {
                    std::erase_if(packet_vector, [this, &packet_id, &min_handler_timestamp, &timer](BasePacketPtr &packet) __lambda_force_inline 
                    {
                        return fulfill_promises(packet_id, packet) || fulfill_handlers(packet_id, packet, min_handler_timestamp, timer) || packet->expired();
                    });
                }
                std::erase_if(
                    unprocessed_packets_, 
                    [](auto const &pair) __lambda_force_inline
                    { return pair.second.empty(); }
                );
                backoff.decrease_delay();
            }
        }

        inline bool fulfill_promises(UniquePacketID packet_id, BasePacketPtr &packet) {
            // Fulfill first filtered promise in filter_promise_map
            {
                auto it = promise_filter_map_.find(packet_id);
                if (it != promise_filter_map_.end())
                {   
                    for (auto &promise_filter : it->second)
                    {
                        if (!promise_filter.first || promise_filter.first(packet))
                        {
                            promise_filter.second->set_value(std::move(packet));
                            return true;
                        }
                    }
                }
            }

            {
                // Fulfill the first promise in promise_map
                auto it = promise_map_.find(packet_id);
                if (it != promise_map_.end() && !it->second.empty())
                {
                    it->second.front()->set_value(std::move(packet));
                    it->second.pop_front();
                    return true;
                }
            }
            return false;
        }
        
        inline bool fulfill_handlers(UniquePacketID packet_id, BasePacketPtr &packet, float &min_handler_timestamp, utils::SteadyTimer &timer)
        {
            auto it = default_handlers_.find(packet_id);
            if(it == default_handlers_.end())
            {
                return false;
            }
           
            for(auto &[delay, filter, handler] : it->second)
            {
                if (delay > packet->get_packet_time_alive())
                {
                    min_handler_timestamp = 
                        std::min<float>(
                            min_handler_timestamp, 
                            timer.elapsed() + delay - packet->get_packet_time_alive()
                            );
                    continue;
                }
 
                if(filter && !filter(*packet))
                {
                    continue;
                }
 
                handler(std::move(packet));
                return true;
            }
            return false;
        }

        /**
         * @brief Pushes input packet to the unprocessed_packets_input_ queue.
         * 
         * @param packet packet to push.
         */
        void push_packet(BasePacketPtr &&packet)
        {
            unprocessed_packets_input_strand_.post(
                [this, released_packet = packet.release()]()
                {
                    BasePacketPtr unique_packet{ released_packet };
                    unprocessed_packets_input_.emplace_back(std::move(unique_packet));
                    unprocessed_packets_input_updated_.test_and_set(std::memory_order_release);
                }
            );
        }

        std::future<bool> create_promise_map_input_pop_task()
        {
            std::shared_ptr<std::promise<bool>> promise_map_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> promise_map_input_future = promise_map_input_promise->get_future();
            promise_map_input_strand_.post(
                [this, promise_map_input_promise]() {
                    for (auto &[packet_id, shared_promise] : promise_map_input_)
                    {
                        auto &promise_queue = promise_map_[packet_id];
                        promise_queue.emplace_back(std::move(shared_promise));
                    }
                    bool t = promise_map_input_.size() > 0;
                    promise_map_input_.clear();
                    promise_map_input_promise->set_value(t);
                    promise_map_input_updated_.clear(std::memory_order_release);
                }
            );
            return promise_map_input_future;
        }
        std::future<bool> create_promise_filter_map_input_pop_task() 
        {
            std::shared_ptr<std::promise<bool>> promise_filter_map_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> promise_filter_map_input_future = promise_filter_map_input_promise->get_future();
            promise_filter_map_input_strand_.post(
                [this, promise_filter_map_input_promise]() {
                    for (auto &[packet_id, filter] : promise_filter_map_input_) {
                        auto &filter_queue = promise_filter_map_[packet_id];
                        filter_queue.emplace_back(std::move(filter));
                    }
                    bool t = promise_filter_map_input_.size() > 0;
                    promise_filter_map_input_.clear();
                    promise_filter_map_input_promise->set_value(t);
                    promise_filter_map_input_updated_.clear(std::memory_order_release);
            });
            return promise_filter_map_input_future;
        }
        std::future<bool> create_default_handlers_input_pop_task() 
        {
            std::shared_ptr<std::promise<bool>> default_handlers_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> default_handlers_input_future = default_handlers_input_promise->get_future();
            default_handlers_input_strand_.post([this, default_handlers_input_promise]() {
                for (auto &[packet_id, handler] : default_handlers_input_) {
                    auto &handler_list = default_handlers_[packet_id];
                    handler_list.emplace_back(std::move(handler));
                }
                bool t = default_handlers_input_.size() > 0;
                default_handlers_input_.clear();
                default_handlers_input_promise->set_value(t);
                default_handlers_input_updated_.clear(std::memory_order_release);
            });
            return default_handlers_input_future;
        }
        std::future<bool> create_unprocessed_packets_input_pop_task() {
            std::shared_ptr<std::promise<bool>> unprocessed_packets_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> unprocessed_packets_input_future = unprocessed_packets_input_promise->get_future();
            unprocessed_packets_input_strand_.post([this, unprocessed_packets_input_promise]() {
                for (auto &packet_ptr : unprocessed_packets_input_) {
                    auto packet_id = packet_ptr->type;
                    auto &unprocessed_packets_queue_ = unprocessed_packets_[packet_id];
                    unprocessed_packets_queue_.emplace_back(std::move(packet_ptr));
                }
                bool t = unprocessed_packets_input_.size() > 0;
                unprocessed_packets_input_.clear();
                unprocessed_packets_input_promise->set_value(t);
                unprocessed_packets_input_updated_.clear(std::memory_order_release);
            });
            return unprocessed_packets_input_future;
        }
        /**
         * @brief Pops input packets from input queues to local maps for processing.
         */
        boost::asio::awaitable<bool> pop_inputs()
        {
            std::vector<std::future<bool>> futures{};
            if(unprocessed_packets_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_promise_map_input_pop_task());
            }
            if(promise_map_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_promise_filter_map_input_pop_task());
            }
            
            if(promise_filter_map_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_default_handlers_input_pop_task());
            }
            
            if(default_handlers_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_unprocessed_packets_input_pop_task());
            }
            bool rv = false;
            for(auto &future : futures)
            {
                while(future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                {
                    co_await boost::asio::this_coro::executor;
                }
                rv |= future.get();
            }
            co_return rv;
        }



        boost::asio::io_context::strand unprocessed_packets_input_strand_;
        boost::asio::io_context::strand promise_map_input_strand_;
        boost::asio::io_context::strand promise_filter_map_input_strand_;
        boost::asio::io_context::strand default_handlers_input_strand_;

        std::atomic_flag unprocessed_packets_input_updated_;
        std::atomic_flag promise_map_input_updated_;
        std::atomic_flag promise_filter_map_input_updated_;
        std::atomic_flag default_handlers_input_updated_;

        std::vector<BasePacketPtr> unprocessed_packets_input_;
        std::vector<std::pair<UniquePacketID, shared_packet_promise>> promise_map_input_;
        std::vector<std::pair<UniquePacketID, promise_filter>> promise_filter_map_input_;
        std::vector<std::pair<UniquePacketID, handler_tuple>> default_handlers_input_;

        std::unordered_map<UniquePacketID, std::vector<BasePacketPtr>> unprocessed_packets_;
        std::unordered_map<UniquePacketID, std::deque<shared_packet_promise>> promise_map_;
        std::unordered_map<UniquePacketID, std::vector<promise_filter>> promise_filter_map_;
        std::unordered_map<UniquePacketID, std::vector<handler_tuple>> default_handlers_;
    };
}