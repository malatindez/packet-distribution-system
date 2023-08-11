#pragma once
#include "../common/packet.hpp"
namespace node_system::packet::network
{
    constexpr UniquePacketID PingPacketID = CreatePacketID(PacketSubsystemNetwork, 0x0000);
    constexpr UniquePacketID PongPacketID = CreatePacketID(PacketSubsystemNetwork, 0x0001);
    constexpr UniquePacketID MessagePacketID = CreatePacketID(PacketSubsystemNetwork, 0x0002);
    constexpr UniquePacketID EchoPacketID = CreatePacketID(PacketSubsystemNetwork, 0x0003);

    class PingPacket : public DerivedPacket<class PingPacket> {
    public:
        static constexpr UniquePacketID static_type = PingPacketID;
        static constexpr float time_to_live = 10.0f;
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
        static constexpr UniquePacketID static_type = PongPacketID;
        static constexpr float time_to_live = 10.0f;
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
        static constexpr UniquePacketID static_type = MessagePacketID;
        static constexpr float time_to_live = 60.0f;
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
    
    class EchoPacket : public DerivedPacket<class EchoPacket> {
    public:
        static constexpr UniquePacketID static_type = EchoPacketID;
        static constexpr float time_to_live = 5.0f;
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }
        std::string echo_message;
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class EchoPacket>>(*this);
            ar& echo_message;
        }
    };


    constexpr void RegisterDeserializers()
    {
        node_system::packet::PacketFactory::RegisterDeserializer<PingPacket>();
        node_system::packet::PacketFactory::RegisterDeserializer<PongPacket>();
        node_system::packet::PacketFactory::RegisterDeserializer<MessagePacket>();
        node_system::packet::PacketFactory::RegisterDeserializer<EchoPacket>();
    }
}