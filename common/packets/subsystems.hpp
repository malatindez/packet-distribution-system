#pragma once
#include <mal-packet-weaver/packet-impl/packet-macro.hpp>

#include "mal-packet-weaver/packet.hpp"

constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemCrypto = 0x0001;
constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemNetwork = 0x0002;
constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemTradeInfo = 0x0003;
constexpr mal_packet_weaver::PacketSubsystemID PacketSubsystemNodeInfo = 0x0004;