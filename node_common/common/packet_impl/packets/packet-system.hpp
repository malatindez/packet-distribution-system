#pragma once
#include "../packet.hpp"
namespace node_system::packet::system
{
    constexpr PacketID SystemInfoRequestID = 0x0000;
    constexpr PacketID SystemInfoResponseID = 0x0001;
    
    class SystemInfoRequestPacket : public DerivedPacket<class SystemInfoRequestPacket> {
    public:
        static constexpr uint32_t static_type = CreatePacketID(PacketSubsystemSystem, SystemInfoRequestID);
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
        static constexpr uint32_t static_type = CreatePacketID(PacketSubsystemSystem, SystemInfoResponseID);
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
}