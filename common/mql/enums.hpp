#pragma once
#include "common.hpp"

namespace mql::common
{
    /**
     * @brief There are several types of accounts that can be opened on a trade server. The type of
     * account on which an MQL4 program is running can be found out using the
     * ENUM_ACCOUNT_TRADE_MODE enumeration.
     *
     */
    enum class EnumAccountTradeMode
    {
        /** @brief Demo account */
        AccountTradeModeDemo = 0,

        /** @brief Contest account */
        AccountTradeModeContest = 1,

        /** @brief Real account */
        AccountTradeModeReal = 2
    };
    /**
     * @brief n case equity is not enough for maintaining open orders, the Stop Out situation, i.e.
     * forced closing occurs. The minimum margin level at which Stop Out occurs can be set in
     * percentage or in monetary terms. To find out the mode set for the account use the
     * ENUM_ACCOUNT_STOPOUT_MODE enumeration.
     *
     */
    enum class EnumAccountStopOutMode
    {
        /** @brief Account stop out mode in percents */
        AccountStopOutModePercent = 0,

        /** @brief Account stop out mode in money */
        AccountStopOutModeMoney = 1
    };
}  // namespace mql::common

namespace mql::mql5
{
    enum class EnumAccountMarginMode
    {
        /** @brief Used for the OTC markets to interpret positions in the "netting" mode (only one
         * position can exist for one symbol). The margin is calculated based on the symbol type (
         * SYMBOL_TRADE_CALC_MODE ). */
        AccountMarginModeRetailNetting,

        /** @brief Used for the exchange markets.
         *  Margin is calculated based on the discounts specified in symbol settings.
         *  Discounts are set by the broker, but not less than the values set by the exchange. */
        AccountMarginModeExchange,

        /** @brief Used for the exchange markets where individual positions are possible (hedging,
         * multiple positions can exist for one symbol). The margin is calculated based on the
         * symbol type ( SYMBOL_TRADE_CALC_MODE ) taking into account the hedged margin (
         * SYMBOL_MARGIN_HEDGED ). */
        AccountMarginModeRetailHedging
    };

    /**
     * @brief Order type information.
     *
     * When sending a trade request using the OrderSend() function, some operations require
     * the indication of the order type. The order type is specified in the type field of the
     * special structure MqlTradeRequest, and can accept values of the ENUM_ORDER_TYPE enumeration.
     *
     * @details
     * https://www.mql5.com/en/docs/constants/tradingconstants/orderproperties#enum_order_type
     */
    enum class EnumOrderType
    {
        /** @brief Market Buy order */
        OrderTypeBuy = 0,

        /** @brief Market Sell order */
        OrderTypeSell = 1,

        /** @brief Buy Limit pending order */
        OrderTypeBuyLimit = 2,

        /** @brief Sell Limit pending order */
        OrderTypeSellLimit = 3,

        /** @brief Buy Stop pending order */
        OrderTypeBuyStop = 4,

        /** @brief Sell Stop pending order */
        OrderTypeSellStop = 5,

        /** @brief Upon reaching the order price, a pending Buy Limit order is placed at the
           StopLimit price */
        OrderTypeBuyStopLimit = 6,

        /** @brief Upon reaching the order price, a pending Sell Limit order is placed at the
           StopLimit price */
        OrderTypeSellStopLimit = 7,

        /** @brief Order to close a position by an opposite one */
        OrderTypeCloseBy = 8
    };
    /**
     * @brief Order state information.
     *
     * Each order has a status that describes its state. To obtain information, use
     * OrderGetInteger() or HistoryOrderGetInteger() with the ORDER_STATE modifier. Allowed values
     * are stored in the ENUM_ORDER_STATE enumeration.
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/orderproperties#enum_order_state
     */
    enum class EnumOrderState
    {
        /** @brief Order checked, but not yet accepted by broker */
        OrderStateStarted = 0,

        /** @brief Order accepted */
        OrderStatePlaced = 1,

        /** @brief Order canceled by client */
        OrderStateCanceled = 2,

        /** @brief Order partially executed */
        OrderStatePartial = 3,

        /** @brief Order fully executed */
        OrderStateFilled = 4,

        /** @brief Order rejected */
        OrderStateRejected = 5,

        /** @brief Order expired */
        OrderStateExpired = 6,

        /** @brief Order is being registered (placing to the trading system) */
        OrderStateRequestAdd = 7,

        /** @brief Order is being modified (changing its parameters) */
        OrderStateRequestModify = 8,

        /** @brief Order is being deleted (deleting from the trading system) */
        OrderStateRequestCancel = 9
    };
    /**
     * @brief Volume filling policy is specified in the ORDER_TYPE_FILLING order property and may
     * contain only the values from the ENUM_ORDER_TYPE_FILLING enumeration
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/orderproperties#enum_order_type_filling
     */
    enum class EnumOrderTypeFilling
    {
        /**
         * @brief Fill or Kill
         *
         * @details An order can be executed in the specified volume only. If the necessary amount
         * of a financial instrument is currently unavailable in the market, the order will not be
         * executed. The desired volume can be made up of several available offers. The possibility
         * of using FOK orders is determined at the trade server.
         *
         * ORDER_FILLING_FOK
         */
        OrderFillingFOK = 0,
        /**
         * @brief Immediate or Cancel
         *
         * @details A trader agrees to execute a deal with the volume maximally available in the
         * market within that indicated in the order. If the request cannot be filled completely, an
         * order with the available volume will be executed, and the remaining volume will be
         * canceled. The possibility of using IOC orders is determined at the trade server.
         *
         * ORDER_FILLING_IOC
         */
        OrderFillingIOC = 1,
        /**
         * @brief Passive (Book or Cancel)
         *
         * @details The BoC order assumes that the order can only be placed in the Depth of Market
         * and cannot be immediately executed. If the order can be executed immediately when placed,
         * then it is canceled. In fact, the BOC policy guarantees that the price of the placed
         * order will be worse than the current market. BoC orders are used to implement passive
         * trading, so that the order is not executed immediately when placed and does not affect
         * current liquidity. Only limit and stop limit orders are supported (ORDER_TYPE_BUY_LIMIT,
         * ORDER_TYPE_SELL_LIMIT, ORDER_TYPE_BUY_STOP_LIMIT, ORDER_TYPE_SELL_STOP_LIMIT).
         *
         * ORDER_FILLING_BOC
         */
        OrderFillingBOC = 2,
        /**
         * @brief Return
         *
         * @details In case of partial filling, an order with remaining volume is not canceled but
         * processed further. Return orders are not allowed in the Market Execution mode (market
         * execution — SYMBOL_TRADE_EXECUTION_MARKET).
         *
         * ORDER_FILLING_RETURN
         *
         */
        OrderFillingReturn = 3
    };
    /**
     * @brief Order type time information
     *
     * The order validity period can be set in the type_time field of the special structure
     * MqlTradeRequest when sending a trade request using the OrderSend() function. Values of the
     * ENUM_ORDER_TYPE_TIME enumeration are allowed. To obtain the value of this property use the
     * function OrderGetInteger() or HistoryOrderGetInteger() with the ORDER_TYPE_TIME modifier.
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/orderproperties#enum_order_type_time
     */
    enum class EnumOrderTypeTime
    {
        /** @brief Good till cancel order */
        OrderTimeGTC = 0,

        /** @brief Good till current trade day order */
        OrderTimeDay = 1,

        /** @brief Good till expired order */
        OrderTimeSpecified = 2,

        /**
         * @brief The order will be effective till 23:59:59 of the specified day.
         * If this time is outside a trading session, the order expires in the nearest trading time.
         */
        OrderTimeSpecifiedDay = 3
    };
    /**
     * @brief Order reason information.
     *
     * The reason for order placing is contained in the ORDER_REASON property. An order can be
     * placed by an MQL5 program, from a mobile application, as a result of StopOut, etc. Possible
     * values of ORDER_REASON are described in the ENUM_ORDER_REASON enumeration.
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/orderproperties#enum_order_reason
     */
    enum class EnumOrderReason
    {
        /** @brief The order was placed from a desktop terminal */
        ORDER_REASON_CLIENT = 0,

        /** @brief The order was placed from a mobile application */
        ORDER_REASON_MOBILE = 1,

        /** @brief The order was placed from a web platform */
        ORDER_REASON_WEB = 2,

        /**
         * @brief The order was placed from an MQL5-program, i.e. by an Expert
         * Advisor or a script
         */
        ORDER_REASON_EXPERT = 3,

        /** @brief The order was placed as a result of Stop Loss activation */
        ORDER_REASON_SL = 4,

        /** @brief The order was placed as a result of Take Profit activation */
        ORDER_REASON_TP = 5,

        /** @brief The order was placed as a result of the Stop Out event */
        ORDER_REASON_SO = 6
    };
    /**
     * @brief Direction of an open position (buy or sell) is defined by the value from the
     * ENUM_POSITION_TYPE enumeration. In order to obtain the type of an open position use the
     * PositionGetInteger() function with the POSITION_TYPE modifier.
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/positionproperties#enum_position_type
     */
    enum class EnumPositionType
    {
        /** @brief Buy */
        PositionTypeBuy = 0,

        /** @brief Sell */
        PositionTypeSell = 1
    };
    /**
     * @brief The reason for opening a position is contained in the POSITION_REASON property. A
     * position can be opened as a result of activation of an order placed from a desktop terminal,
     * a mobile application, by an Expert Advisor, etc. Possible values of POSITION_REASON are
     * described in the ENUM_POSITION_REASON enumeration.
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/positionproperties#enum_position_reason
     */
    enum class EnumPositionReason
    {
        /** @brief The position was opened as a result of activation of an order placed from a
           desktop terminal */
        PositionReasonClient = 0,

        /** @brief The position was opened as a result of activation of an order placed from a
           mobile application */
        PositionReasonMobile = 1,

        /** @brief The position was opened as a result of activation of an order placed from the web
           platform */
        PositionReasonWeb = 2,

        /**
         * @brief The position was opened as a result of activation of an order placed from an MQL5
         * program, i.e. an Expert Advisor or a script
         */
        PositionReasonExpert = 3
    };
    /**
     * @brief Each deal is characterized by a type, allowed values are enumerated in ENUM_DEAL_TYPE.
     * In order to obtain information about the deal type, use the HistoryDealGetInteger() function
     * with the DEAL_TYPE modifier.
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/dealproperties#enum_deal_type
     *
     */
    enum class EnumDealType
    {
        /** @brief Buy */
        DealTypeBuy = 0,

        /** @brief Sell */
        DealTypeSell = 1,

        /** @brief Balance */
        DealTypeBalance = 2,

        /** @brief Credit */
        DealTypeCredit = 3,

        /** @brief Additional charge */
        DealTypeCharge = 4,

        /** @brief Correction */
        DealTypeCorrection = 5,

        /** @brief Bonus */
        DealTypeBonus = 6,

        /** @brief Additional commission */
        DealTypeCommission = 7,

        /** @brief Daily commission */
        DealTypeCommissionDaily = 8,

        /** @brief Monthly commission */
        DealTypeCommissionMonthly = 9,

        /** @brief Daily agent commission */
        DealTypeCommissionAgentDaily = 10,

        /** @brief Monthly agent commission */
        DealTypeCommissionAgentMonthly = 11,

        /** @brief Interest rate */
        DealTypeInterest = 12,

        /** @brief Canceled buy deal.
         *
         * There can be a situation when a previously executed buy deal is canceled.
         * In this case, the type of the previously executed deal (DealTypeBuy)
         * is changed to DealTypeBuyCanceled, and its profit/loss is zeroized.
         * Previously obtained profit/loss is charged/withdrawn using a separated balance operation.
         */
        DealTypeBuyCanceled = 13,

        /** @brief Canceled sell deal.
         *
         * There can be a situation when a previously executed sell deal is canceled.
         * In this case, the type of the previously executed deal (DealTypeSell)
         * is changed to DealTypeSellCanceled, and its profit/loss is zeroized.
         * Previously obtained profit/loss is charged/withdrawn using a separated balance operation.
         */
        DealTypeSellCanceled = 14,

        /** @brief Dividend operations */
        DealDividend = 15,

        /** @brief Franked (non-taxable) dividend operations */
        DealDividendFranked = 16,

        /** @brief Tax charges */
        DealTax = 17
    };
    /**
     * @brief Deals differ not only in their types set in ENUM_DEAL_TYPE, but also in the way they
     * change positions. This can be a simple position opening, or accumulation of a previously
     * opened position (market entering), position closing by an opposite deal of a corresponding
     * volume (market exiting), or position reversing, if the opposite-direction deal covers the
     * volume of the previously opened position. All these situations are described by values from
     * the ENUM_DEAL_ENTRY enumeration. In order to receive this information about a deal, use the
     * HistoryDealGetInteger() function with the DEAL_ENTRY modifier.
     *
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/dealproperties#enum_deal_entry
     */
    enum class EnumDealEntry
    {
        /** @brief Entry in */
        DealEntryIn = 0,

        /** @brief Entry out */
        DealEntryOut = 1,

        /** @brief Reverse */
        DealEntryInOut = 2,

        /** @brief Close a position by an opposite one */
        DealEntryOutBy = 3
    };
    /**
     * @brief The reason for deal execution is contained in the DEAL_REASON property. A deal can be
     * executed as a result of triggering of an order placed from a mobile application or an MQL5
     * program, as well as as a result of the StopOut event, variation margin calculation, etc.
     * Possible values of DEAL_REASON are described in the ENUM_DEAL_REASON enumeration. For
     * non-trading deals resulting from balance, credit, commission and other operations,
     * DEAL_REASON_CLIENT is indicated as the reason.
     * @details More info:
     * https://www.mql5.com/en/docs/constants/tradingconstants/dealproperties#enum_deal_reason
     */
    enum class EnumDealReason
    {
        /** @brief The deal was executed as a result of activation of an order placed from a desktop
           terminal */
        DealReasonClient = 0,

        /** @brief The deal was executed as a result of activation of an order placed from a mobile
           application */
        DealReasonMobile = 1,

        /** @brief The deal was executed as a result of activation of an order placed from the web
           platform */
        DealReasonWeb = 2,

        /** @brief The deal was executed as a result of activation of an order placed from an MQL5
           program, i.e. an Expert Advisor or a script */
        DealReasonExpert = 3,

        /** @brief The deal was executed as a result of Stop Loss activation */
        DealReasonSL = 4,

        /** @brief The deal was executed as a result of Take Profit activation */
        DealReasonTP = 5,

        /** @brief The deal was executed as a result of the Stop Out event */
        DealReasonSO = 6,

        /** @brief The deal was executed due to a rollover */
        DealReasonRollover = 7,

        /** @brief The deal was executed after charging the variation margin */
        DealReasonVMargin = 8,

        /** @brief The deal was executed after the split (price reduction) of an instrument, which
           had an open position during split announcement */
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
