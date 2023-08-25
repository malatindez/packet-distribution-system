#pragma once
#include "subsystems.hpp"

MAL_PACKET_WEAVER_DECLARE_PACKET(PingPacket, PacketSubsystemNetwork, 0, 120.0f)
MAL_PACKET_WEAVER_DECLARE_PACKET(PongPacket, PacketSubsystemNetwork, 1, 120.0f)
MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_PAYLOAD(MessagePacket, PacketSubsystemNetwork, 2, 120.0f,
                                              (std::string, message))
MAL_PACKET_WEAVER_DECLARE_PACKET_WITH_PAYLOAD(EchoPacket, PacketSubsystemNetwork, 3, 120.0f,
                                              (std::string, echo_message))
