#pragma once
#include "packet.hpp"
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/spawn.hpp>

namespace node_system
{
    class PacketDispatcher
    {
    public:
        template <IsPacket P>
        void registerHandler(std::function<void(std::unique_ptr<P>)> handler)
        {
            handlers_[P::static_type] = [passedHandler = std::move(handler)](std::unique_ptr<Packet> packet)
                                            __lambda_force_inline -> void
            {
                passedHandler(std::dynamic_pointer_cast<P>(packet));
            };
        }

        void dispatch(std::unique_ptr<Packet> packet)
        {
            auto it = handlers_.find(packet->type);
            if (it != handlers_.end())
            {
                it->second(std::move(packet));
            }
        }
    private:
        std::unordered_map<UniquePacketID,
                           std::function<void(std::unique_ptr<Packet>)>>
            handlers_;
    };
    class CoroutinePacketDispatcher
    {
    public:
        template <IsPacket P>
        void registerHandler(std::function<boost::asio::awaitable<void>(std::unique_ptr<P>)> handler)
        {
            handlers_[P::static_type] = [passedHandler = std::move(handler)](std::unique_ptr<Packet> packet)
                                            __lambda_force_inline -> boost::asio::awaitable<void>
            {
                co_await passedHandler(std::dynamic_pointer_cast<P>(packet));
            };
        }

        boost::asio::awaitable<void> dispatch(std::unique_ptr<Packet> packet)
        {
            auto it = handlers_.find(packet->type);
            if (it != handlers_.end())
            {
                co_await it->second(std::move(packet));
            }
        }

    private:
        std::unordered_map<UniquePacketID,
                           std::function<boost::asio::awaitable<void>(std::unique_ptr<Packet>)>>
            handlers_;
    };
    
}