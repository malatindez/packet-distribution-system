#pragma once
#include "packet.hpp"
#include "packets/packet-crypto.hpp"
#include "packets/packet-node.hpp"
#include "packets/packet-network.hpp"
#include "packets/packet-system.hpp"

namespace node_system::packet
{
    class PacketFactory {
    public:
        template<typename PacketType>
        static void RegisterDeserializer() requires std::is_base_of_v<Packet, PacketType>
        {
            packet_deserializers_[PacketType::static_type] = PacketType::deserialize;
        }
        static void RegisterDeserializer(UniquePacketID packet_id, PacketDeserializeFunc const &factory) {
            packet_deserializers_[packet_id] = factory;
        }
        
        static std::unique_ptr<Packet> Deserialize(const ByteView& bytearray, UniquePacketID packet_type) {
            auto it = packet_deserializers_.find(packet_type);
            if (it != packet_deserializers_.end()) {
                return it->second(bytearray); 
            }
            return nullptr;
        }

        static void InitializeBasePackets()
        {
            using namespace node_system::packet::crypto;
            using namespace node_system::packet::network;
            using namespace node_system::packet::node;
            using namespace node_system::packet::system;
            RegisterDeserializer<DHKeyExchangeRequestPacket>();
            RegisterDeserializer<DHKeyExchangeResponsePacket>();
            RegisterDeserializer<PingPacket>();
            RegisterDeserializer<PongPacket>();
            RegisterDeserializer<MessagePacket>();
            RegisterDeserializer<NodeInfoRequestPacket>();
            RegisterDeserializer<NodeInfoResponsePacket>();
            RegisterDeserializer<SystemInfoRequestPacket>();
            RegisterDeserializer<SystemInfoResponsePacket>();
        }
    private:
        static std::unordered_map<UniquePacketID, PacketDeserializeFunc> packet_deserializers_;
    };
}