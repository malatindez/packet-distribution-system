#pragma once
#include "../common/packet.hpp"
namespace node_system::packet::node
{
    constexpr UniquePacketID NodeInfoRequestPacketID =  CreatePacketID(PacketSubsystemNode,0x0000);
    constexpr UniquePacketID NodeInfoResponsePacketID =  CreatePacketID(PacketSubsystemNode,0x0001);
    
    class NodeInfoRequestPacket : public DerivedPacket<class NodeInfoRequestPacket> {
    public:
        static constexpr UniquePacketID static_type = NodeInfoRequestPacketID;
        static constexpr float time_to_live = 5.0f;
        [[nodiscard]] Permission get_permission() const override { return Permission::ANY; }

        // TODO:
        // implement
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class NodeInfoRequestPacket>>(*this);
        }
    };

    class NodeInfoResponsePacket : public DerivedPacket<class NodeInfoResponsePacket> {
    public:
        static constexpr UniquePacketID static_type = NodeInfoResponsePacketID;
        static constexpr float time_to_live = 5.0f;
        [[nodiscard]] Permission get_permission() const override { return Permission::L2_CORE_NODE; }
        // TODO:
        // implement
    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
            ar& boost::serialization::base_object<DerivedPacket<class NodeInfoResponsePacket>>(*this);
        }
    };
}