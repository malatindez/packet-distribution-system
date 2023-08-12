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
    /**
     * @brief Callback function type to handle packets asynchronously.
     *
     * This function type defines the signature for packet handler functions that accept a unique pointer
     * to a derived packet type and return an awaitable boost::asio task.
     *
     * @tparam DerivedPacket The derived packet type.
     */
    template <typename DerivedPacket>
    using PacketHandlerFunc = std::function<void(std::unique_ptr<DerivedPacket>)>;

    /**
     * @brief Predicate function type to filter packets.
     *
     * This function type defines the signature for packet filter functions that accept a const reference
     * to a derived packet type and return a boolean indicating whether the packet should be filtered or not.
     *
     * @tparam DerivedPacket The derived packet type.
     */
    template <typename DerivedPacket>
    using PacketFilterFunc = std::function<bool(DerivedPacket const &)>;

    /**
     * @brief The PacketDispatcher class is responsible for managing packet dispatching and handling.
     *
     * This class associates with a specific io_context and provides functionality for enqueuing packets and
     * managing packet handlers and filters.
     */
    class PacketDispatcher final
    {
    public:
        /**
         * @brief Alias for a unique pointer to a base packet type.
         */
        using BasePacketPtr = std::unique_ptr<Packet>;

        /**
         * @brief Alias for a shared promise of type T.
         *
         * This type alias defines a shared pointer to a promise that holds a value of type T.
         *
         * @tparam T The type of the value held by the promise.
         */
        template <typename T>
        using shared_promise = std::shared_ptr<std::promise<T>>;

        /**
         * @brief Alias for a shared promise of a base packet pointer.
         */
        using shared_packet_promise = shared_promise<BasePacketPtr>;

        /**
         * @brief Alias for a filter function paired with a shared packet promise.
         *
         * This type alias defines a pair where the first element is a filter function that accepts a const reference
         * to a base packet pointer and returns a boolean. The second element is a shared promise that holds a base packet pointer.
         */
        using promise_filter = std::pair<std::function<bool(BasePacketPtr const &)>, shared_packet_promise>;

        /**
         * @brief Alias for a tuple containing information for packet handling.
         *
         * This type alias defines a tuple that holds information related to packet handling. The first element is a float
         * representing a priority, the second element is a packet filter function, and the third element is a packet handler function.
         *
         */
        using handler_tuple = std::tuple<float, PacketFilterFunc<Packet>, PacketHandlerFunc<Packet>>;

        /**
         * @brief Constructs a PacketDispatcher instance associated with the given io_context.
         *
         * @param io_context The io_context to associate with the dispatcher.
         */
        PacketDispatcher(boost::asio::io_context &io_context)
            : unprocessed_packets_input_strand_{io_context},
              promise_map_input_strand_{io_context},
              promise_filter_map_input_strand_{io_context},
              default_handlers_input_strand_{io_context}
        {
            spdlog::debug("PacketDispatcher constructor called.");
            co_spawn(io_context, std::bind(&PacketDispatcher::Run, this), boost::asio::detached);
        }

        /**
         * @brief Enqueues a packet for processing.
         *
         * This function enqueues a unique pointer to a packet for processing by pushing it onto the internal queue.
         *
         * @param packet The unique pointer to the packet to be enqueued.
         */
        inline void enqueue_packet(BasePacketPtr &&packet)
        {
            spdlog::trace("Enqueuing packet for processing.");
            push_packet(std::move(packet));
        }
        /**
         * @brief Wait until the packet is registered in the dispatch system and return as soon as possible.
         *
         * This function template waits for a specific type of packet to be registered in the dispatch system. It can optionally
         * wait for a specified timeout duration.
         *
         * @tparam DerivedPacket The type of packet you want to wait for.
         * @param timeout If less than or equal to zero, the function will not return until the promise is fulfilled. Otherwise, it will wait for the given timeout (in seconds) before returning.
         * @return boost::asio::awaitable<std::unique_ptr<DerivedPacket>> A unique pointer to the received packet, or nullptr if the timeout was reached.
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

            if (timeout <= 0)
            {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                spdlog::trace("Received packet: {}", DerivedPacket::static_type);
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket *>(base.release()));
            }

            std::future_status status = future.wait_for(std::chrono::microseconds(size_t(timeout * 1e6f)));

            if (status == std::future_status::ready)
            {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                spdlog::trace("Received packet: {}", DerivedPacket::static_type);
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket *>(base.release()));
            }
            else if (status == std::future_status::timeout)
            {
                spdlog::warn("Timed out waiting for packet: {}", DerivedPacket::static_type);
                co_return nullptr;
            }
            else
            {
                spdlog::error("An error occurred while waiting for packet: {}", DerivedPacket::static_type);
                co_return nullptr;
            }
        }

        /**
         * @brief Wait until a packet satisfying the filter condition is registered in the dispatch system and return as soon as possible.
         *
         * This function template waits for a packet of a specific type, satisfying a provided filter condition, to be registered in the dispatch system.
         * It can optionally wait for a specified timeout duration.
         *
         * @tparam DerivedPacket The type of packet you want to wait for.
         * @param filter A function to filter the packet. If the functor returns true, the packet will fulfill the promise.
         * @param timeout If less than or equal to zero, the function will not return until the promise is fulfilled. Otherwise, it will wait for the given timeout (in seconds) before returning.
         * @return boost::asio::awaitable<std::unique_ptr<DerivedPacket>> A unique pointer to the received packet, or nullptr if the timeout was reached or the filter condition was not satisfied.
         */
        template <IsPacket DerivedPacket>
        boost::asio::awaitable<std::unique_ptr<DerivedPacket>> await_packet(PacketFilterFunc<DerivedPacket> filter, float timeout = -1.0f)
        {
            auto packet_type = DerivedPacket::static_type;
            auto promise = std::make_shared<std::promise<BasePacketPtr>>();
            enqueue_filter_promise(
                packet_type,
                {[passedFilter = filter](BasePacketPtr const &packet)
                 {
                     return passedFilter(*reinterpret_cast<DerivedPacket *>(packet.get()));
                 },
                 promise});

            auto future = promise->get_future();
            co_await boost::asio::this_coro::executor;

            spdlog::trace("Waiting for packet: {}", DerivedPacket::static_type);

            if (timeout <= 0)
            {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                spdlog::trace("Received packet: {}", DerivedPacket::static_type);
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket *>(base.release()));
            }

            std::future_status status = future.wait_for(std::chrono::microseconds(size_t(timeout * 1e6f)));

            if (status == std::future_status::ready)
            {
                auto base = future.get();
                utils::Assert(base->type == DerivedPacket::static_type); // Sanity check
                spdlog::trace("Received packet: {}", DerivedPacket::static_type);
                co_return std::unique_ptr<DerivedPacket>(reinterpret_cast<DerivedPacket *>(base.release()));
            }
            else if (status == std::future_status::timeout)
            {
                spdlog::warn("Timed out waiting for packet: {}", DerivedPacket::static_type);
                co_return nullptr;
            }
            else
            {
                spdlog::error("An error occurred while waiting for packet: {}", DerivedPacket::static_type);
                co_return nullptr;
            }
        }

        /**
         * @brief Registers a default handler for the provided packet type.
         *
         * This function registers a default packet handler for a specific packet type. The handler function can be provided,
         * and if it returns false, the packet is passed to the next handler. An optional filter function can also be provided
         * to determine whether the handler should be applied based on the packet's properties. A delay parameter can be used
         * to postpone the handler's execution for a certain amount of time.
         *
         * @todo Add an ability to delete handlers
         * 
         * @tparam DerivedPacket The type of packet for which the handler should be registered.
         * @param handler The packet handler function. If it returns false, the packet will be passed to the next handler.
         * @param filter The packet filter function to determine whether the handler should be applied. (Optional)
         * @param delay The delay in seconds before the handler is executed. (Default is 0.0)
         */
        template <IsPacket DerivedPacket>
        void register_default_handler(PacketHandlerFunc<DerivedPacket> handler, PacketFilterFunc<DerivedPacket> filter = {}, float delay = 0.0f)
        {
            spdlog::trace("Posting task to register default handler for packet {}", DerivedPacket::static_type);
            default_handlers_input_strand_.post(
                [this,
                 delay,
                 movedFilter = filter,
                 movedHandler = handler]() __lambda_force_inline -> void
                {
                    constexpr auto packet_id = DerivedPacket::static_type;
                    spdlog::trace("Registered default handler for packet {}!", packet_id);
                    handler_tuple tuple = handler_tuple{
                        delay,
                        !bool(movedFilter) ? PacketFilterFunc<Packet>{} : ([movedFilter](Packet const& packet) -> bool { return movedFilter(reinterpret_cast<DerivedPacket const&>(packet)); }),
                        [movedHandler](std::unique_ptr<Packet>&& packet)  
                        { 
                            auto ptr = reinterpret_cast<DerivedPacket*>(packet.release());
                            auto uptr = std::unique_ptr<DerivedPacket>(ptr);
                            movedHandler(std::move(uptr));
                        }
                    };
                    default_handlers_input_.emplace_back(std::pair{packet_id, tuple});
                    default_handlers_input_updated_.test_and_set(std::memory_order_release);
                });
        }

        /**
         * @brief Enqueues a promise associated with a packet.
         *
         * This function enqueues a promise (associated with a specific packet ID) for future fulfillment. The promise is
         * associated with a unique packet identifier. The enqueued promises will be processed later.
         *
         * @param packet_id The unique packet identifier for which the promise is being enqueued.
         * @param promise The shared packet promise to be enqueued.
         */
        void enqueue_promise(UniquePacketID packet_id, shared_packet_promise promise)
        {
            spdlog::trace("Posting task to enqueue promise for packet {}", packet_id);
            promise_map_input_strand_.post(
                [this, packet_id, moved_promise = std::move(promise)]() mutable
                {
                    spdlog::trace("Promise enqueued for packet {}!", packet_id);
                    promise_map_input_.emplace_back(std::pair{packet_id, std::move(moved_promise)});
                    promise_map_input_updated_.test_and_set(std::memory_order_release);
                });
        }

        /**
         * @brief Enqueues a promise with a filter associated with a packet.
         *
         * This function enqueues a promise (associated with a specific packet ID) that includes a filter function. The promise
         * will be fulfilled based on the provided filter's outcome. The enqueued promises with filters will be processed later.
         *
         * @param packet_id The unique packet identifier for which the filtered promise is being enqueued.
         * @param filtered_promise The promise filter to be enqueued.
         */
        void enqueue_filter_promise(UniquePacketID packet_id, promise_filter filtered_promise)
        {
            spdlog::trace("Posting task to enqueue promise with filter for packet {}", packet_id);
            promise_filter_map_input_strand_.post(
                [this, packet_id, moved_filtered_promise = std::move(filtered_promise)]() mutable
                {
                    spdlog::trace("Promise with filter enqueued for packet {}!", packet_id);
                    promise_filter_map_input_.emplace_back(std::pair{packet_id, std::move(moved_filtered_promise)});
                    promise_filter_map_input_updated_.test_and_set(std::memory_order_release);
                });
        }

    private:
        /**
         * @brief This function represents the main loop for running a task with exponential backoff and asynchronous I/O.
         *        It processes input packets and handles them while managing delays and timers.
         *
         * @return A boost::asio::awaitable<void> representing the asynchronous task.
         */
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

                if (!updated)
                {
                    if (min_handler_timestamp < timer.elapsed())
                    {
                        spdlog::trace("Updating handlers...");
                        min_handler_timestamp = std::numeric_limits<float>::max();

                        for (auto &[packet_id, packet_vector] : unprocessed_packets_)
                        {
                            // Remove packets that fulfill handlers and update min_handler_timestamp
                            std::erase_if(packet_vector, [this, &packet_id, &min_handler_timestamp, &timer](BasePacketPtr &packet) __lambda_force_inline
                                          { return fulfill_handlers(packet_id, packet, min_handler_timestamp, timer); });
                        }
                    }

                    // Introduce delay and increase it using exponential backoff strategy
                    boost::asio::steady_timer(co_await boost::asio::this_coro::executor, backoff.get_current_delay());
                    backoff.increase_delay();
                    continue;
                }

                spdlog::trace("Input arrays were updated! Fetching...");

                min_handler_timestamp = std::numeric_limits<float>::max();
                for (auto &[packet_id, packet_vector] : unprocessed_packets_)
                {
                    // Process packets: fulfill promises, fulfill handlers, and check for expiration
                    std::erase_if(packet_vector, [this, &packet_id, &min_handler_timestamp, &timer](BasePacketPtr &packet) __lambda_force_inline
                                  { return fulfill_promises(packet_id, packet) || fulfill_handlers(packet_id, packet, min_handler_timestamp, timer) || packet->expired(); });
                }

                // Remove empty entries from the unprocessed_packets_ map
                std::erase_if(
                    unprocessed_packets_,
                    [](auto const &pair) __lambda_force_inline
                    { return pair.second.empty(); });

                // Decrease the delay for exponential backoff
                backoff.decrease_delay();
            }
        }

        /**
         * @brief Fulfills promises associated with a packet ID.
         *
         * This function fulfills promises from two different maps: promise_filter_map_
         * and promise_map_. It searches for promises in promise_filter_map_ first,
         * and if a matching promise is found, it checks if the associated filter
         * condition (if any) is satisfied before fulfilling the promise. If no matching
         * promise is found in promise_filter_map_, it then searches for a promise in
         * promise_map_ and fulfills the first one if available.
         *
         * @param packet_id The unique ID of the packet.
         * @param packet A reference to the packet to be fulfilled.
         * @return `true` if at least one promise was fulfilled, otherwise `false`.
         */
        inline bool fulfill_promises(UniquePacketID packet_id, BasePacketPtr &packet)
        {
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
                            spdlog::trace("Fulfilled filtered promise for packet_id: {}", packet_id);
                            return true;
                        }
                    }
                }
            }

            // Fulfill the first promise in promise_map
            {
                auto it = promise_map_.find(packet_id);
                if (it != promise_map_.end() && !it->second.empty())
                {
                    it->second.front()->set_value(std::move(packet));
                    it->second.pop_front();
                    spdlog::trace("Fulfilled promise for packet_id: {}", packet_id);
                    return true;
                }
            }

            spdlog::trace("No promises to fulfill for packet_id: {}", packet_id);
            return false;
        }

        /**
         * @brief Fulfills handlers associated with a packet ID and packet data.
         *
         * This function fulfills handlers for a given packet ID by searching for
         * associated handlers in the default_handlers_ map. For each handler,
         * it checks if the specified delay is greater than the packet's time alive.
         * If so, it updates the minimum handler timestamp. Then, it checks if the
         * associated filter condition (if any) is satisfied before executing the handler.
         *
         * @param packet_id The unique ID of the packet.
         * @param packet A reference to the packet for which handlers should be fulfilled.
         * @param min_handler_timestamp The minimum handler timestamp to update.
         * @param timer The timer used for timestamp calculations.
         * @return `true` if at least one handler was fulfilled, otherwise `false`.
         */
        inline bool fulfill_handlers(UniquePacketID packet_id, BasePacketPtr &packet, float &min_handler_timestamp, utils::SteadyTimer &timer)
        {
            auto it = default_handlers_.find(packet_id);
            if (it == default_handlers_.end())
            {
                spdlog::warn("No handlers to fulfill for packet_id: {}", packet_id);
                return false;
            }

            for (auto &[delay, filter, handler] : it->second)
            {
                if (delay > packet->get_packet_time_alive())
                {
                    min_handler_timestamp =
                        std::min<float>(
                            min_handler_timestamp,
                            timer.elapsed() + delay - packet->get_packet_time_alive());
                    spdlog::trace("Handler delay for packet_id {} is greater than packet time alive.", packet_id);
                    continue;
                }

                if (bool(filter) && !filter(*packet))
                {
                    spdlog::trace("Filter condition not satisfied for packet_id: {}", packet_id);
                    continue;
                }

                handler(std::move(packet));
                spdlog::trace("Fulfilled handler for packet_id: {}", packet_id);
                return true;
            }

            spdlog::trace("No suitable handlers to fulfill for packet_id: {}", packet_id);
            return false;
        }

        /**
         * @brief Pushes an input packet to the unprocessed_packets_input_ queue.
         *
         * This function posts a task to the unprocessed_packets_input_strand_
         * to push an input packet into the unprocessed_packets_input_ queue. The
         * packet is moved into a unique pointer, and the unprocessed_packets_input_updated_
         * atomic flag is set to indicate that the queue has been updated.
         *
         * @param packet The packet to push (as an rvalue reference).
         */
        void push_packet(BasePacketPtr &&packet)
        {
            unprocessed_packets_input_strand_.post(
                [this, released_packet = packet.release()]()
                {
                    BasePacketPtr unique_packet{released_packet};
                    unprocessed_packets_input_.emplace_back(std::move(unique_packet));
                    unprocessed_packets_input_updated_.test_and_set(std::memory_order_release);
                    spdlog::trace("Pushed packet to unprocessed_packets_input_ queue.");
                });
        }

        /**
         * @brief Creates a promise_map_input_pop_task and returns a future to await completion.
         * @return A future that signals the completion of the task.
         */
        std::future<bool> create_promise_map_input_pop_task()
        {
            spdlog::trace("Creating promise_map_input_pop_task");
            std::shared_ptr<std::promise<bool>> promise_map_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> promise_map_input_future = promise_map_input_promise->get_future();
            promise_map_input_strand_.post(
                [this, promise_map_input_promise]()
                {
                    for (auto &[packet_id, shared_promise] : promise_map_input_)
                    {
                        auto &promise_queue = promise_map_[packet_id];
                        promise_queue.emplace_back(std::move(shared_promise));
                    }
                    bool t = promise_map_input_.size() > 0;
                    promise_map_input_.clear();
                    promise_map_input_promise->set_value(t);
                    promise_map_input_updated_.clear(std::memory_order_release);
                    spdlog::trace("promise_map_input_pop_task completed");
                });
            return promise_map_input_future;
        }

        /**
         * @brief Creates a promise_filter_map_input_pop_task and returns a future to await completion.
         * @return A future that signals the completion of the task.
         */
        std::future<bool> create_promise_filter_map_input_pop_task()
        {
            spdlog::trace("Creating promise_filter_map_input_pop_task");
            std::shared_ptr<std::promise<bool>> promise_filter_map_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> promise_filter_map_input_future = promise_filter_map_input_promise->get_future();
            promise_filter_map_input_strand_.post(
                [this, promise_filter_map_input_promise]()
                {
                    for (auto &[packet_id, filter] : promise_filter_map_input_)
                    {
                        auto &filter_queue = promise_filter_map_[packet_id];
                        filter_queue.emplace_back(std::move(filter));
                    }
                    bool t = promise_filter_map_input_.size() > 0;
                    promise_filter_map_input_.clear();
                    promise_filter_map_input_promise->set_value(t);
                    promise_filter_map_input_updated_.clear(std::memory_order_release);
                    spdlog::debug("promise_filter_map_input_pop_task completed");
                });
            return promise_filter_map_input_future;
        }

        /**
         * @brief Creates a default_handlers_input_pop_task and returns a future to await completion.
         * @return A future that signals the completion of the task.
         */
        std::future<bool> create_default_handlers_input_pop_task()
        {
            spdlog::trace("Creating default_handlers_input_pop_task");
            std::shared_ptr<std::promise<bool>> default_handlers_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> default_handlers_input_future = default_handlers_input_promise->get_future();
            default_handlers_input_strand_.post([this, default_handlers_input_promise]()
                                                {
        for (auto &[packet_id, handler] : default_handlers_input_) {
            auto &handler_list = default_handlers_[packet_id];
            handler_list.emplace_back(std::move(handler));
        }
        bool t = default_handlers_input_.size() > 0;
        default_handlers_input_.clear();
        default_handlers_input_promise->set_value(t);
        default_handlers_input_updated_.clear(std::memory_order_release);
        spdlog::trace("default_handlers_input_pop_task completed"); });
            return default_handlers_input_future;
        }

        /**
         * @brief Creates an unprocessed_packets_input_pop_task and returns a future to await completion.
         * @return A future that signals the completion of the task.
         */
        std::future<bool> create_unprocessed_packets_input_pop_task()
        {
            spdlog::trace("Creating unprocessed_packets_input_pop_task");
            std::shared_ptr<std::promise<bool>> unprocessed_packets_input_promise = std::make_shared<std::promise<bool>>();
            std::future<bool> unprocessed_packets_input_future = unprocessed_packets_input_promise->get_future();
            unprocessed_packets_input_strand_.post([this, unprocessed_packets_input_promise]()
            {
                for (auto &packet_ptr : unprocessed_packets_input_) {
                    if (packet_ptr == nullptr)
                    {
                        spdlog::warn("packet_ptr in unprocessed_input is nullptr");
                        continue;
                    }
                    auto packet_id = packet_ptr->type;
                    auto &unprocessed_packets_queue_ = unprocessed_packets_[packet_id];
                    unprocessed_packets_queue_.emplace_back(std::move(packet_ptr));
                }
                bool t = unprocessed_packets_input_.size() > 0;
                unprocessed_packets_input_.clear();
                unprocessed_packets_input_promise->set_value(t);
                unprocessed_packets_input_updated_.clear(std::memory_order_release);
                spdlog::trace("unprocessed_packets_input_pop_task completed"); 
            });
            return unprocessed_packets_input_future;
        }

        /**
         * @brief Pops input packets from input queues to local maps for processing.
         * @return An awaitable indicating whether the task was successful.
         */
        boost::asio::awaitable<bool> pop_inputs()
        {
            std::vector<std::future<bool>> futures{};
            if (unprocessed_packets_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_unprocessed_packets_input_pop_task());
            }
            if (promise_map_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_promise_map_input_pop_task());
            }
            if (promise_filter_map_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_promise_filter_map_input_pop_task());
            }
            if (default_handlers_input_updated_.test(std::memory_order_acquire))
            {
                futures.emplace_back(create_default_handlers_input_pop_task());
            }
            bool rv = false;
            for (auto &future : futures)
            {
                spdlog::trace("Waiting for futures to complete...");
                while (future.wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                {
                    co_await boost::asio::this_coro::executor;
                }
                rv |= future.get();
                spdlog::trace("Futures completed. Result for popping is: {}", rv);
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