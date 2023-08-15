#pragma once
#include "mal-packet-weaver/packet.hpp"
constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemNetwork = 0x0002;

/**
 * @brief Unique packet ID for PingPacket.
 */
constexpr mal_packet_weaver::UniquePacketID PingPacketID =
    mal_packet_weaver::CreatePacketID(PacketSubsystemNetwork, 0x0000);

/**
 * @brief Unique packet ID for PongPacket.
 */
constexpr mal_packet_weaver::UniquePacketID PongPacketID =
    mal_packet_weaver::CreatePacketID(PacketSubsystemNetwork, 0x0001);

/**
 * @brief Unique packet ID for MessagePacket.
 */
constexpr mal_packet_weaver::UniquePacketID MessagePacketID =
    mal_packet_weaver::CreatePacketID(PacketSubsystemNetwork, 0x0002);

/**
 * @brief Unique packet ID for EchoPacket.
 */
constexpr mal_packet_weaver::UniquePacketID EchoPacketID =
    mal_packet_weaver::CreatePacketID(PacketSubsystemNetwork, 0x0003);

/**
 * @brief Packet for sending a ping signal.
 */
class PingPacket final : public mal_packet_weaver::DerivedPacket<class PingPacket>
{
public:
    static constexpr mal_packet_weaver::UniquePacketID static_type = PingPacketID;
    static constexpr float time_to_live = 10.0f;
   

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &, const unsigned int) { }
    static mal_packet_weaver::PacketTypeRegistrationHelper<PingPacket> registration;
};

inline mal_packet_weaver::PacketTypeRegistrationHelper<PingPacket> PingPacket::registration;
/**
 * @brief Packet for responding to a ping signal.
 */
class PongPacket final : public mal_packet_weaver::DerivedPacket<class PongPacket>
{
public:
    static constexpr mal_packet_weaver::UniquePacketID static_type = PongPacketID;
    static constexpr float time_to_live = 10.0f;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &, const unsigned int) { }
    static mal_packet_weaver::PacketTypeRegistrationHelper<PongPacket> registration;
};

inline mal_packet_weaver::PacketTypeRegistrationHelper<PongPacket> PongPacket::registration;

/**
 * @brief Packet for sending a text message.
 */
class MessagePacket final : public mal_packet_weaver::DerivedPacket<class MessagePacket>
{
public:
    static constexpr mal_packet_weaver::UniquePacketID static_type = MessagePacketID;
    static constexpr float time_to_live = 60.0f;
    std::string message;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
    {
        ar &message;
    }
    static mal_packet_weaver::PacketTypeRegistrationHelper<MessagePacket> registration;
};
inline mal_packet_weaver::PacketTypeRegistrationHelper<MessagePacket> MessagePacket::registration;

/**
 * @brief Packet for echoing a received message.
 */
class EchoPacket final : public mal_packet_weaver::DerivedPacket<class EchoPacket>
{
public:
    static constexpr mal_packet_weaver::UniquePacketID static_type = EchoPacketID;
    static constexpr float time_to_live = 5.0f;
    std::string echo_message;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, [[maybe_unused]] const unsigned int version)
    {
        ar &echo_message;
    }
    static mal_packet_weaver::PacketTypeRegistrationHelper<EchoPacket> registration;
};

inline mal_packet_weaver::PacketTypeRegistrationHelper<EchoPacket> EchoPacket::registration;