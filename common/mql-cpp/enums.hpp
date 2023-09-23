#pragma once
#include "common.hpp"

namespace mql::common
{
    
    enum class EnumAccountTradeMode
    {
        
        AccountTradeModeDemo = 0,

        
        AccountTradeModeContest = 1,

        
        AccountTradeModeReal = 2
    };
    
    enum class EnumAccountStopOutMode
    {
        
        AccountStopOutModePercent = 0,

        
        AccountStopOutModeMoney = 1
    };
}  // namespace mql::common

namespace mql::mql5
{
    enum class EnumAccountMarginMode
    {
        
        AccountMarginModeRetailNetting,

        
        AccountMarginModeExchange,

        
        AccountMarginModeRetailHedging
    };

    
    enum class EnumOrderType
    {
        
        OrderTypeBuy = 0,

        
        OrderTypeSell = 1,

        
        OrderTypeBuyLimit = 2,

        
        OrderTypeSellLimit = 3,

        
        OrderTypeBuyStop = 4,

        
        OrderTypeSellStop = 5,

        
        OrderTypeBuyStopLimit = 6,

        
        OrderTypeSellStopLimit = 7,

        
        OrderTypeCloseBy = 8
    };
    
    enum class EnumOrderState
    {
        
        OrderStateStarted = 0,

        
        OrderStatePlaced = 1,

        
        OrderStateCanceled = 2,

        
        OrderStatePartial = 3,

        
        OrderStateFilled = 4,

        
        OrderStateRejected = 5,

        
        OrderStateExpired = 6,

        
        OrderStateRequestAdd = 7,

        
        OrderStateRequestModify = 8,

        
        OrderStateRequestCancel = 9
    };
    
    enum class EnumOrderTypeFilling
    {
        
        OrderFillingFOK = 0,
        
        OrderFillingIOC = 1,
        
        OrderFillingBOC = 2,
        
        OrderFillingReturn = 3
    };
    
    enum class EnumOrderTypeTime
    {
        
        OrderTimeGTC = 0,

        
        OrderTimeDay = 1,

        
        OrderTimeSpecified = 2,

        
        OrderTimeSpecifiedDay = 3
    };
    
    enum class EnumOrderReason
    {
        
        ORDER_REASON_CLIENT = 0,

        
        ORDER_REASON_MOBILE = 1,

        
        ORDER_REASON_WEB = 2,

        
        ORDER_REASON_EXPERT = 3,

        
        ORDER_REASON_SL = 4,

        
        ORDER_REASON_TP = 5,

        
        ORDER_REASON_SO = 6
    };
    
    enum class EnumPositionType
    {
        
        PositionTypeBuy = 0,

        
        PositionTypeSell = 1
    };
    
    enum class EnumPositionReason
    {
        
        PositionReasonClient = 0,

        
        PositionReasonMobile = 1,

        
        PositionReasonWeb = 2,

        
        PositionReasonExpert = 3
    };
    
    enum class EnumDealType
    {
        
        DealTypeBuy = 0,

        
        DealTypeSell = 1,

        
        DealTypeBalance = 2,

        
        DealTypeCredit = 3,

        
        DealTypeCharge = 4,

        
        DealTypeCorrection = 5,

        
        DealTypeBonus = 6,

        
        DealTypeCommission = 7,

        
        DealTypeCommissionDaily = 8,

        
        DealTypeCommissionMonthly = 9,

        
        DealTypeCommissionAgentDaily = 10,

        
        DealTypeCommissionAgentMonthly = 11,

        
        DealTypeInterest = 12,

        
        DealTypeBuyCanceled = 13,

        
        DealTypeSellCanceled = 14,

        
        DealDividend = 15,

        
        DealDividendFranked = 16,

        
        DealTax = 17
    };
    
    enum class EnumDealEntry
    {
        
        DealEntryIn = 0,

        
        DealEntryOut = 1,

        
        DealEntryInOut = 2,

        
        DealEntryOutBy = 3
    };
    
    enum class EnumDealReason
    {
        
        DealReasonClient = 0,

        
        DealReasonMobile = 1,

        
        DealReasonWeb = 2,

        
        DealReasonExpert = 3,

        
        DealReasonSL = 4,

        
        DealReasonTP = 5,

        
        DealReasonSO = 6,

        
        DealReasonRollover = 7,

        
        DealReasonVMargin = 8,

        
        DealReasonSplit = 9
    };

}  // namespace mql::mql5

#define MQL_DECLARE_ENUM_SERIALIZE(ENUM_TYPE)                                       \
    namespace boost::serialization                                                  \
    {                                                                               \
        template <class Archive>                                                    \
        void serialize(Archive &ar, ENUM_TYPE &enum_order_type, const unsigned int) \
        {                                                                           \
            int t = 0;                                                              \
            if (Archive::is_saving::value)                                          \
            {                                                                       \
                t = static_cast<int>(enum_order_type);                              \
                ar &t;                                                              \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                ar &t;                                                              \
                enum_order_type = static_cast<ENUM_TYPE>(t);                        \
            }                                                                       \
        }                                                                           \
    }

MQL_DECLARE_ENUM_SERIALIZE(mql::common::EnumAccountTradeMode)
MQL_DECLARE_ENUM_SERIALIZE(mql::common::EnumAccountStopOutMode)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumAccountMarginMode)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumOrderType)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumOrderState)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumOrderTypeFilling)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumOrderTypeTime)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumOrderReason)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumPositionType)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumPositionReason)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumDealType)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumDealEntry)
MQL_DECLARE_ENUM_SERIALIZE(mql::mql5::EnumDealReason)
