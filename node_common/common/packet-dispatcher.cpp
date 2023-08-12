#include "packet-dispatcher.hpp"
namespace node_system
{
    PacketDispatcher::PacketDispatcher(boost::asio::io_context &io_context)
        : io_context_{io_context},
          unprocessed_packets_input_strand_{io_context},
          promise_map_input_strand_{io_context},
          promise_filter_map_input_strand_{io_context},
          default_handlers_input_strand_{io_context}
    {
        spdlog::debug("PacketDispatcher constructor called.");
        co_spawn(io_context, std::bind(&PacketDispatcher::Run, this), boost::asio::detached);
    }


    boost::asio::awaitable<void> PacketDispatcher::Run()
    {
        ExponentialBackoffUs backoff{
            std::chrono::microseconds(1),
            std::chrono::microseconds(500),
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
                boost::asio::steady_timer async_timer(co_await boost::asio::this_coro::executor, backoff.get_current_delay());
                co_await async_timer.async_wait(boost::asio::use_awaitable);
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

    std::future<bool> PacketDispatcher::create_promise_map_input_pop_task()
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

    std::future<bool> PacketDispatcher::create_promise_filter_map_input_pop_task()
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

    std::future<bool> PacketDispatcher::create_default_handlers_input_pop_task()
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

    std::future<bool> PacketDispatcher::create_unprocessed_packets_input_pop_task()
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
                spdlog::trace("unprocessed_packets_input_pop_task completed"); });
        return unprocessed_packets_input_future;
    }

    boost::asio::awaitable<bool> PacketDispatcher::pop_inputs()
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
            while (future.wait_for(std::chrono::microseconds(100)) != std::future_status::ready)
            {
                co_await boost::asio::this_coro::executor;
            }
            rv |= future.get();
            spdlog::trace("Futures completed. Result for popping is: {}", rv);
        }
        co_return rv;
    }
}