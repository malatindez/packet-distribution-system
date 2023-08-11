#pragma once
#include "packet.hpp"
namespace node_system::packet
{
    class PacketFactory {
    public:
        template<IsPacket PacketType>
        static inline void RegisterDeserializer()
        {
            packet_deserializers_[PacketType::static_type] = PacketType::deserialize;
        }
        static inline void RegisterDeserializer(UniquePacketID packet_id, PacketDeserializeFunc const &factory) {
            packet_deserializers_[packet_id] = factory;
        }
        
        [[nodiscard]] static inline std::unique_ptr<Packet> Deserialize(const ByteView& bytearray, UniquePacketID packet_type) {
            auto it = packet_deserializers_.find(packet_type);
            if (it != packet_deserializers_.end()) {
                return it->second(bytearray); 
            }
            return nullptr;
        }
    private:
        static std::unordered_map<UniquePacketID, PacketDeserializeFunc> packet_deserializers_;
    };
}