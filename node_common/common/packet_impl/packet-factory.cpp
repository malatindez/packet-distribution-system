#include "packet-factory.hpp"

namespace node_system::packet
{
    std::unordered_map<UniquePacketID, PacketDeserializer> PacketFactory::packet_deserializers_;
}