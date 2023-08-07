#pragma once
#include "../packet.hpp"
namespace node_system::packet::network
{
    constexpr PacketID PingID = 0x0000;
    constexpr PacketID PongID = 0x0001;
    constexpr PacketID MessageID = 0x0002;

    class PingPacket : public DerivedPacket<class PingPacket> {
    public:
        static constexpr UniquePacketID static_type = CreatePacketID(PacketSubsystemNetwork, PingID);
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class PingPacket>>(*this);
        }
    };

    class PongPacket : public DerivedPacket<class PongPacket> {
    public:
        static constexpr UniquePacketID static_type = CreatePacketID(PacketSubsystemNetwork, PongID);
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class PongPacket>>(*this);
        }
    };
    
    class MessagePacket : public DerivedPacket<class MessagePacket> {
    public:
        static constexpr UniquePacketID static_type = CreatePacketID(PacketSubsystemNetwork, MessageID);
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }
        std::string message;
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class MessagePacket>>(*this);
            ar& message;
        }
    };

}