#include "packet-factory.hpp"

namespace node_system
{
    std::map<PacketSubsystemType, SubsystemFactoryFunc> PacketFactory::subsystemFactories;
}