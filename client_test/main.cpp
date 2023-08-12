#include <iostream>
#include <thread>

#include "common/session.hpp"
#include "common/packet-dispatcher.hpp"
#include "crypto/diffie-hellman.hpp"
#include "crypto/ecdsa.hpp"
#include "packets/packet-crypto.hpp"
#include "packets/packet-network.hpp"
#include "packets/packet-node.hpp"
#include "packets/packet-system.hpp"
#include "crypto/diffie-hellman.hpp"
#include "crypto/ecdsa.hpp"

using namespace node_system;
using namespace node_system::crypto;
using namespace node_system::packet;
using namespace node_system::packet::crypto;
using namespace node_system::packet::network;
using namespace node_system::packet::system;
using namespace node_system::packet::node;

std::string bytes_to_hex_str(node_system::ByteView const byte_view)
{
    std::string rv;
    for (int i = 0; i < byte_view.size(); i++)
    {
        const uint8_t val = static_cast<uint8_t>(byte_view[i]);
        const static std::string hex_values = "0123456789abcdef";
        rv += hex_values[val >> 4];
        rv += hex_values[val & 0xF];
    }
    return rv;
}

boost::asio::awaitable<void> setup_encryption_for_session(
    std::shared_ptr<node_system::Session> connection,
    std::shared_ptr<node_system::PacketDispatcher> dispatcher,
    boost::asio::io_context &io,
    node_system::crypto::ECDSA::Verifier &verifier)
{
    node_system::crypto::DiffieHellmanHelper dh{};
    DHKeyExchangeRequestPacket dh_packet;
    dh_packet.public_key = dh.get_public_key();
    connection->send_packet(dh_packet);

    auto response = co_await dispatcher->await_packet<DHKeyExchangeResponsePacket>();

    if (!verifier.verify_hash(response->get_hash(), response->signature))
    {
        spdlog::warn("encryption response packet has the wrong signature. Aborting application.");
        std::abort();
    }

    node_system::ByteArray shared_secret = dh.get_shared_secret(response->public_key);
    std::cout << "Computed shared secret: " << bytes_to_hex_str(shared_secret) << std::endl;
    shared_secret.append(response->salt);
    const node_system::crypto::Hash shared_key = node_system::crypto::SHA::ComputeHash(shared_secret, node_system::crypto::Hash::HashType::SHA256);
    std::cout << "Computed shared key: " << bytes_to_hex_str(shared_key.hash_value) << std::endl;

    connection->setup_encryption(shared_key.hash_value, response->salt, static_cast<uint16_t>(response->n_rounds));
    node_system::packet::network::EchoPacket echo;
    echo.echo_message = "0";
    connection->send_packet(echo);
}

void process_echo(
    std::shared_ptr<node_system::Session> connection,
    std::unique_ptr<node_system::packet::network::EchoPacket> &&echo)
{
    node_system::packet::network::EchoPacket response;
    response.echo_message = std::to_string(std::stoi(echo->echo_message) + 1);
    connection->send_packet(response);
    spdlog::info("Received message: {}", echo->echo_message);
}

void workThread(boost::asio::io_context &ioContext)
{
    ioContext.run();
}

int main()
{
    spdlog::set_level(spdlog::level::debug);
    node_system::packet::crypto::RegisterDeserializers();
    node_system::packet::network::RegisterDeserializers();
    node_system::packet::node::RegisterDeserializers();
    node_system::packet::system::RegisterDeserializers();

    try
    {
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket(io_context);
        socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
        std::cout << "Connected to server." << std::endl;
        std::shared_ptr session = std::make_shared<node_system::Session>(io_context, std::move(socket));

        std::shared_ptr<node_system::PacketDispatcher> dispatcher = std::make_shared<node_system::PacketDispatcher>(io_context);
        session->SetPacketReceiver([&dispatcher](std::unique_ptr<node_system::Packet> &&packet) __lambda_force_inline
                                   { dispatcher->enqueue_packet(std::move(packet)); });
        dispatcher->register_default_handler<EchoPacket>(
            [session, &io_context](std::unique_ptr<node_system::packet::network::EchoPacket> &&packet)
            {
                process_echo(session, std::move(packet));
            }
       );
        node_system::ByteArray public_key;
        std::ifstream public_key_file("core_public.pem");
        // count amount of bytes in file
        public_key_file.seekg(0, std::ios::end);
        public_key.resize(public_key_file.tellg());
        public_key_file.seekg(0, std::ios::beg);
        public_key_file.read(reinterpret_cast<char *>(public_key.data()), public_key.size());
        public_key_file.close();

        node_system::crypto::ECDSA::Verifier verifier{public_key, node_system::crypto::Hash::HashType::SHA256};
        co_spawn(io_context,
                 std::bind(&setup_encryption_for_session,
                           session,
                           dispatcher,
                           std::ref(io_context),
                           std::ref(verifier)),
                 boost::asio::detached);

        std::vector<std::thread> threads;
        for (int i = 0; i < 8; ++i)
        {
            threads.emplace_back([&io_context]()
                                 {
                try
                {
                    workThread(io_context);
                }
                catch (std::exception& e)
                {
                    spdlog::error(e.what());
                } });
        }

        try
        {
            io_context.run();
        }
        catch (std::exception &e)
        {
            spdlog::error(e.what());
        }
        for (auto &thread : threads)
        {
            thread.join();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}