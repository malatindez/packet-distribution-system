#pragma once
#include "../common/packet.hpp"
namespace node_system::packet::system
{
    constexpr UniquePacketID SystemInfoRequestPacketID = CreatePacketID(PacketSubsystemSystem, 0x0000);
    constexpr UniquePacketID SystemInfoResponsePacketID = CreatePacketID(PacketSubsystemSystem, 0x0001);
    
    class SystemInfoRequestPacket : public DerivedPacket<class SystemInfoRequestPacket> {
    public:
        static constexpr UniquePacketID static_type = SystemInfoRequestPacketID;
        static constexpr float time_to_live = 5.0f;
        [[nodiscard]] Permission get_permission() const override { return Permission::L2_CORE_NODE; }
        // TODO:
        // implement
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class SystemInfoRequestPacket>>(*this);
        }
    };

    class SystemInfoResponsePacket : public DerivedPacket<class SystemInfoResponsePacket> {
    public:
        static constexpr UniquePacketID static_type = SystemInfoResponsePacketID;
        static constexpr float time_to_live = 5.0f;
        [[nodiscard]] Permission get_permission() const override { return Permission::L2_CORE_NODE; }
        // TODO:
        // implement
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class SystemInfoResponsePacket>>(*this);
        }
    };
    
    inline void RegisterDeserializers()
    {
        node_system::packet::PacketFactory::RegisterDeserializer<SystemInfoRequestPacket>();
        node_system::packet::PacketFactory::RegisterDeserializer<SystemInfoResponsePacket>();
    }
}