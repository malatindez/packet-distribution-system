#pragma once
#include "../packet.hpp"
namespace node_system::packet::node
{
    constexpr PacketID NodeInfoRequestID = 0x0000;
    constexpr PacketID NodeInfoResponseID = 0x0001;
    
    class NodeInfoRequestPacket : public DerivedPacket<class NodeInfoRequestPacket> {
    public:
        static constexpr uint32_t static_type = CreatePacketID(PacketSubsystemNode, NodeInfoRequestID);
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
        static constexpr uint32_t static_type = CreatePacketID(PacketSubsystemNode, NodeInfoResponseID);
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