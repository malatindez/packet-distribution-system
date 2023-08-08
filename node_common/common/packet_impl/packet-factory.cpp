#include "packet-factory.hpp"

namespace node_system::packet
{
    std::unordered_map<UniquePacketID, PacketDeserializeFunc> PacketFactory::packet_deserializers_;
}