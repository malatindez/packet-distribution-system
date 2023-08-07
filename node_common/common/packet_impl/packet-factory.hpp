#pragma once
#include "packet-common.hpp"
#include "packet-crypto.hpp"
#include "packet-node.hpp"
#include "packet-network.hpp"
#include "packet-system.hpp"
namespace node_system
{
    using SubsystemFactoryFunc = std::function<std::unique_ptr<Packet>(const ByteView&, uint32_t)>;
    class PacketFactory {
    public:
        static void RegisterSubsystemFactory(PacketSubsystemType type, SubsystemFactoryFunc factory) {
            subsystemFactories[type] = factory;
        }
        
        static std::unique_ptr<Packet> Deserialize(const ByteView& bytearray, uint32_t packet_type) {
            PacketSubsystemType subsystem = Uint32ToPacketSubsystemType(packet_type);
            auto it = subsystemFactories.find(subsystem);
            if (it != subsystemFactories.end()) {
                return it->second(bytearray, packet_type); 
            }
            return nullptr;
        }
        static void Initialize()
        {
            RegisterSubsystemFactory(
                PacketSubsystemType::CRYPTO,
                PacketFactorySubsystem<PacketSubsystemType::CRYPTO>::deserialize
            );
            RegisterSubsystemFactory(
                PacketSubsystemType::NODE,
                PacketFactorySubsystem<PacketSubsystemType::NODE>::deserialize
            );
            RegisterSubsystemFactory(
                PacketSubsystemType::NETWORK,
                PacketFactorySubsystem<PacketSubsystemType::NETWORK>::deserialize
            );
            RegisterSubsystemFactory(
                PacketSubsystemType::SYSTEM,
                PacketFactorySubsystem<PacketSubsystemType::SYSTEM>::deserialize
            );
        }
    private:
        static std::unordered_map<PacketSubsystemType, SubsystemFactoryFunc> subsystem_factories_;
    };
}