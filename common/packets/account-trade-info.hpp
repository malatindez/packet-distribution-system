#pragma once
#include "../mql/mql.hpp"
#include "subsystems.hpp"

class PacketTag
{
    uint64_t uid;

private:
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive &ar, const unsigned int)
    {
        ar &uid;
    }
};

// clang-format off
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL_VersionRequest, (PacketTag), PacketSubsystemTradeInfo, 0, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITH_PAYLOAD(MQL_VersionResponse, (PacketTag), PacketSubsystemTradeInfo, 1, 60, (bool, MQL4), (bool, MQL5))

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoDoubleRequest, (PacketTag), PacketSubsystemTradeInfo, 2, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoDoubleResponse, (PacketTag, mql::common::AccountInfoDouble), PacketSubsystemTradeInfo, 3, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoStringRequest, (PacketTag), PacketSubsystemTradeInfo, 4, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoStringResponse, (PacketTag, mql::common::AccountInfoString), PacketSubsystemTradeInfo, 5, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoIntegerRequest, (PacketTag), PacketSubsystemTradeInfo, 6, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoIntegerResponse, (PacketTag, mql::common::AccountInfoInteger), PacketSubsystemTradeInfo, 7, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoDoubleMinimalRequest, (PacketTag), PacketSubsystemTradeInfo, 8, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoDoubleMinimalResponse, (PacketTag, mql::common::AccountInfoDoubleMinimal), PacketSubsystemTradeInfo, 9, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoIntegerMinimalRequest, (PacketTag), PacketSubsystemTradeInfo, 10, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoIntegerMinimalResponse, (PacketTag, mql::common::AccountInfoIntegerMinimal), PacketSubsystemTradeInfo, 11, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoMinimalRequest, (PacketTag), PacketSubsystemTradeInfo, 12, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(AccountInfoMinimalResponse, (PacketTag, mql::common::AccountInfoMinimal), PacketSubsystemTradeInfo, 13, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL4FullAccountInfoRequest, (PacketTag), PacketSubsystemTradeInfo, 14, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL4FullAccountInfoResponse, (PacketTag, mql::mql4::FullAccountInfo), PacketSubsystemTradeInfo, 15, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5AccountInfoIntegerRequest, (PacketTag), PacketSubsystemTradeInfo, 16, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5AccountInfoIntegerResponse, (PacketTag, mql::mql5::AccountInfoInteger), PacketSubsystemTradeInfo, 17, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5AccountInfoDoubleRequest, (PacketTag), PacketSubsystemTradeInfo, 18, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5AccountInfoDoubleResponse, (PacketTag, mql::mql5::AccountInfoDouble), PacketSubsystemTradeInfo, 19, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoIntegerRequest, (PacketTag), PacketSubsystemTradeInfo, 20, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoIntegerResponse, (PacketTag, mql::mql5::OrderInfoInteger), PacketSubsystemTradeInfo, 21, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoDoubleRequest, (PacketTag), PacketSubsystemTradeInfo, 22, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoDoubleResponse, (PacketTag, mql::mql5::OrderInfoDouble), PacketSubsystemTradeInfo, 23, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoStringRequest, (PacketTag), PacketSubsystemTradeInfo, 24, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoStringResponse, (PacketTag, mql::mql5::OrderInfoString), PacketSubsystemTradeInfo, 25, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoRequest, (PacketTag), PacketSubsystemTradeInfo, 26, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5OrderInfoResponse, (PacketTag, mql::mql5::OrderInfo), PacketSubsystemTradeInfo, 27, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoIntegerRequest, (PacketTag), PacketSubsystemTradeInfo, 28, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoIntegerResponse, (PacketTag, mql::mql5::PositionInfoInteger), PacketSubsystemTradeInfo, 29, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoDoubleRequest, (PacketTag), PacketSubsystemTradeInfo, 30, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoDoubleResponse, (PacketTag, mql::mql5::PositionInfoDouble), PacketSubsystemTradeInfo, 31, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoStringRequest, (PacketTag), PacketSubsystemTradeInfo, 32, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoStringResponse, (PacketTag, mql::mql5::PositionInfoString), PacketSubsystemTradeInfo, 33, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoRequest, (PacketTag), PacketSubsystemTradeInfo, 34, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5PositionInfoResponse, (PacketTag, mql::mql5::PositionInfo), PacketSubsystemTradeInfo, 35, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoIntegerRequest, (PacketTag), PacketSubsystemTradeInfo, 36, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoIntegerResponse, (PacketTag, mql::mql5::DealInfoInteger), PacketSubsystemTradeInfo, 37, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoDoubleRequest, (PacketTag), PacketSubsystemTradeInfo, 38, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoDoubleResponse, (PacketTag, mql::mql5::DealInfoDouble), PacketSubsystemTradeInfo, 39, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoStringRequest, (PacketTag), PacketSubsystemTradeInfo, 40, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoStringResponse, (PacketTag, mql::mql5::DealInfoString), PacketSubsystemTradeInfo, 41, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoRequest, (PacketTag), PacketSubsystemTradeInfo, 42, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL5DealInfoResponse, (PacketTag, mql::mql5::DealInfo), PacketSubsystemTradeInfo, 43, 60)

MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL4OrderInfoRequest, (PacketTag), PacketSubsystemTradeInfo, 44, 60)
MAL_PACKET_WEAVER_DECLARE_DERIVED_PACKET_WITHOUT_PAYLOAD(MQL4OrderInfoResponse, (PacketTag, mql::mql4::OrderInfo), PacketSubsystemTradeInfo, 45, 60)

// clang-format on