#pragma once
#include <boost/preprocessor.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <mal-packet-weaver/packet.hpp>

#include "../mql/mql.hpp"

using mal_packet_weaver::CreatePacketID;
using mal_packet_weaver::DerivedPacket;
using mal_packet_weaver::PacketSubsystemID;
using mal_packet_weaver::UniquePacketID;

constexpr PacketSubsystemID PacketSubsystemTradeInfo = 0x0003;

#define MPW_DERAR MAL_PACKET_WEAVER_DECLARE_EMPTY_REQUEST_AND_RESPONSE

#define MPW_DERADR MAL_PACKET_WEAVER_DECLARE_EMPTY_REQUEST_AND_DERIVED_RESPONSE

// clang-format off
MPW_DERAR(MQL_Version, PacketSubsystemTradeInfo, 0, 1, 60, 60, (bool, MQL4), (bool, MQL5))
MPW_DERADR(AccountInfoDouble, mql::common::AccountInfoDouble, PacketSubsystemTradeInfo, 2, 3, 60, 60)
MPW_DERADR(AccountInfoString, mql::common::AccountInfoString, PacketSubsystemTradeInfo, 4, 5, 60, 60)
MPW_DERADR(AccountInfoInteger, mql::common::AccountInfoInteger, PacketSubsystemTradeInfo, 6, 7, 60, 60)
MPW_DERADR(AccountInfoDoubleMinimal, mql::common::AccountInfoDoubleMinimal, PacketSubsystemTradeInfo, 8, 9, 60, 60)
MPW_DERADR(AccountInfoIntegerMinimal, mql::common::AccountInfoIntegerMinimal, PacketSubsystemTradeInfo, 10, 11, 60, 60)
MPW_DERADR(AccountInfoMinimal, mql::common::AccountInfoMinimal, PacketSubsystemTradeInfo, 12, 13, 60, 60)
MPW_DERADR(MQL4FullAccountInfo, mql::mql4::FullAccountInfo, PacketSubsystemTradeInfo, 14, 15, 60, 60)
MPW_DERADR(MQL5AccountInfoInteger, mql::mql5::AccountInfoInteger, PacketSubsystemTradeInfo, 16, 17, 60, 60)
MPW_DERADR(MQL5AccountInfoDouble, mql::mql5::AccountInfoDouble, PacketSubsystemTradeInfo, 18, 19, 60, 60)
MPW_DERADR(MQL5OrderInfoInteger, mql::mql5::OrderInfoInteger, PacketSubsystemTradeInfo, 20, 21, 60, 60)
MPW_DERADR(MQL5OrderInfoDouble, mql::mql5::OrderInfoDouble, PacketSubsystemTradeInfo, 22, 23, 60, 60)
MPW_DERADR(MQL5OrderInfoString, mql::mql5::OrderInfoString, PacketSubsystemTradeInfo, 24, 25, 60, 60)
MPW_DERADR(MQL5OrderInfo, mql::mql5::OrderInfo, PacketSubsystemTradeInfo, 26, 27, 60, 60)
MPW_DERADR(MQL5PositionInfoInteger, mql::mql5::PositionInfoInteger, PacketSubsystemTradeInfo, 28, 29, 60, 60)
MPW_DERADR(MQL5PositionInfoDouble, mql::mql5::PositionInfoDouble, PacketSubsystemTradeInfo, 30, 31, 60, 60)
MPW_DERADR(MQL5PositionInfoString, mql::mql5::PositionInfoString, PacketSubsystemTradeInfo, 32, 33, 60, 60)
MPW_DERADR(MQL5PositionInfo, mql::mql5::PositionInfo, PacketSubsystemTradeInfo, 34, 35, 60, 60)
MPW_DERADR(MQL5DealInfoInteger, mql::mql5::DealInfoInteger, PacketSubsystemTradeInfo, 36, 37, 60, 60)
MPW_DERADR(MQL5DealInfoDouble, mql::mql5::DealInfoDouble, PacketSubsystemTradeInfo, 38, 39, 60, 60)
MPW_DERADR(MQL5DealInfoString, mql::mql5::DealInfoString, PacketSubsystemTradeInfo, 40, 41, 60, 60)
MPW_DERADR(MQL5DealInfo, mql::mql5::DealInfo, PacketSubsystemTradeInfo, 42, 43, 60, 60)
MPW_DERADR(MQL4OrderInfo, mql::mql4::OrderInfo, PacketSubsystemTradeInfo, 44, 45, 60, 60)
// clang-format on