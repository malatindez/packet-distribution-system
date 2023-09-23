#pragma once
#include "common.hpp"
#include "enums.hpp"

namespace mql::mql5
{
    struct OrderInfoInteger
    {
        /** @brief Order ticket. Unique number assigned to each order*/
        MQL_long ticket;
        /** @brief Order setup time*/
        MQL_DateTime time_setup;
        /** @brief Order type*/
        EnumOrderType type;
        /** @brief Order state*/
        EnumOrderState state;
        /** @brief Order expiration time*/
        MQL_DateTime time_expiration;
        /** @brief Order execution or cancellation time */
        MQL_DateTime time_done;
        /** @brief The time of placing an order for execution in milliseconds since 01.01.1970 */
        MQL_long time_setup_msc;
        /** @brief Order execution/cancellation time in milliseconds since 01.01.1970 */
        MQL_long time_done_msc;
        /** @brief Order filling type */
        EnumOrderTypeFilling type_filling;
        /** @brief Order lifetime */
        EnumOrderTypeTime type_time;
        /** @brief ID of an Expert Advisor that has placed the order (designed to ensure that each
         * Expert Advisor places its own unique number) */
        MQL_long magic;
        /** @brief The reason or source for placing an order */
        EnumOrderReason reason;
        /** @brief Position identifier that is set to an order as soon as it is executed. Each
         * executed order results in a deal that opens or modifies an already existing position. The
         * identifier of exactly this position is set to the executed order at this moment. */
        MQL_long position_id;
        /** @brief Identifier of an opposite position used for closing by order  ORDER_TYPE_CLOSE_BY
         */
        MQL_long position_by_id;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &ticket;
            ar &time_setup;
            ar &type;
            ar &state;
            ar &time_expiration;
            ar &time_done;
            ar &time_setup_msc;
            ar &time_done_msc;
            ar &state;
            ar &state;
            ar &magic;
            ar &state;
            ar &position_id;
            ar &position_by_id;
        }
    };

    struct OrderInfoDouble
    {
        /** @brief Order initial volume */
        double volume_initial;
        /** @brief Order current volume */
        double volume_current;
        /** @brief Price specified in the order */
        double price_open;
        /** @brief Stop Loss value */
        double stop_loss;
        /** @brief Take Profit value */
        double take_profit;
        /** @brief The current price of the order symbol */
        double price_current;
        /** @brief The Limit order price for the StopLimit order */
        double stop_limit;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &volume_initial;
            ar &volume_current;
            ar &price_open;
            ar &stop_loss;
            ar &take_profit;
            ar &price_current;
            ar &stop_limit;
        }
    };

    struct OrderInfoString
    {
        /** @brief Symbol of the order */
        std::string symbol;
        /** @brief Order comment */
        std::string comment;
        /** @brief Order identifier in an external trading system (on the Exchange) */
        std::string external_id;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &symbol;
            ar &comment;
            ar &external_id;
        }
    };

    struct OrderInfo : OrderInfoInteger, OrderInfoDouble, OrderInfoString
    {
    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &boost::serialization::base_object<OrderInfoInteger>(*this);
            ar &boost::serialization::base_object<OrderInfoDouble>(*this);
            ar &boost::serialization::base_object<OrderInfoString>(*this);
        }
    };

    struct PositionInfoInteger
    {
        /**
         * @brief Position ticket.
         *
         * @details Unique number assigned to each newly opened position. It usually matches the
         * ticket of an order used to open the position except when the ticket is changed as a
         * result of service operations on the server, for example, when charging swaps with
         * position re-opening. To find an order used to open a position, apply the
         * POSITION_IDENTIFIER property. POSITION_TICKET value corresponds to
         * MqlTradeRequest::position.
         */
        MQL_long ticket;
        /**
         * @brief Position open time
         */
        MQL_DateTime open_time;
        /** @brief Position opening time in milliseconds since 01.01.1970 */
        MQL_long open_time_msc;
        /** @brief Position changing time */
        MQL_DateTime time_update;
        /** @brief Position changing time in milliseconds since 01.01.1970 */
        MQL_long time_update_msc;
        /** @brief Position type */
        EnumPositionType type;
        /** @brief Position magic number
         * @see OrderInfoInteger::magic
         */
        MQL_long magic;
        /** @brief Position identifier is a unique number assigned to each re-opened position.
         *
         * @details It does not change throughout its life cycle and corresponds to the ticket of an
         * order used to open a position. Position identifier is specified in each order
         * (ORDER_POSITION_ID) and deal (DEAL_POSITION_ID) used to open, modify, or close it. Use
         * this property to search for orders and deals related to the position. When reversing a
         * position in netting mode (using a single in/out trade), POSITION_IDENTIFIER does not
         * change. However, POSITION_TICKET is replaced with the ticket of the order that led to the
         * reversal. Position reversal is not provided in hedging mode.
         */
        MQL_long identifier;
        /** @brief The reason for opening a position */
        EnumPositionReason reason;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &ticket;
            ar &open_time;
            ar &open_time_msc;
            ar &time_update;
            ar &time_update_msc;
            ar &type;
            ar &magic;
            ar &identifier;
            ar &reason;
        }
    };

    struct PositionInfoDouble
    {
        /** @brief Position volume */
        double volume;
        /** @brief Position open price */
        double price_open;
        /** @brief Stop Loss level of opened position */
        double stop_loss;
        /** @brief Take Pofit level of opened position */
        double take_profit;
        /** @brief Current price of the position symbol */
        double price_current;
        /** @brief Cumulative swap */
        double swap;
        /** @brief Current profit */
        double profit;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &volume;
            ar &price_open;
            ar &stop_loss;
            ar &take_profit;
            ar &price_current;
            ar &swap;
            ar &profit;
        }
    };

    struct PositionInfoString
    {
        /**
         * @brief Symbol of the position
         */
        std::string symbol;
        /**
         * @brief Position comment
         */
        std::string comment;
        /**
         * @brief Position identifier in an external trading system (on the Exchange)
         */
        std::string external_id;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &symbol;
            ar &comment;
            ar &external_id;
        }
    };

    struct PositionInfo : PositionInfoInteger, PositionInfoDouble, PositionInfoString
    {
    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &boost::serialization::base_object<PositionInfoInteger>(*this);
            ar &boost::serialization::base_object<PositionInfoDouble>(*this);
            ar &boost::serialization::base_object<PositionInfoString>(*this);
        }
    };

    struct DealInfoInteger
    {
        /**
         * @brief Deal ticket. Unique number assigned to each deal.
         */
        MQL_long ticket;

        /**
         * @brief Deal order number.
         */
        MQL_long order;

        /**
         * @brief Deal time.
         */
        MQL_DateTime time;

        /**
         * @brief The time of a deal execution in milliseconds since 01.01.1970.
         */
        MQL_long time_msc;

        /**
         * @brief Deal type.
         */
        EnumDealType type;

        /**
         * @brief Deal entry - entry in, entry out, reverse.
         */
        EnumDealEntry entry;

        /**
         * @brief Deal magic number.
         *
         * @details See OrderInfoInteger::magic for more information.
         */
        MQL_long magic;

        /**
         * @brief The reason or source for deal execution.
         */
        EnumDealReason reason;

        /**
         * @brief Identifier of a position, in the opening, modification, or closing of which this
         * deal took part.
         *
         * @details Each position has a unique identifier that is assigned to all deals executed for
         * the symbol during the entire lifetime of the position.
         */
        MQL_long position_id;

    private:
        friend class boost::serialization::access;

        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &ticket;
            ar &order;
            ar &time;
            ar &time_msc;
            ar &type;
            ar &entry;
            ar &magic;
            ar &reason;
            ar &position_id;
        }
    };

    struct DealInfoDouble
    {
        /** @brief Deal volume */
        double volume;

        /** @brief Deal price */
        double price;

        /** @brief Deal commission */
        double commission;

        /** @brief Cumulative swap on close */
        double swap;

        /** @brief Deal profit */
        double profit;

        /** @brief Fee for making a deal charged immediately after performing a deal */
        double fee;

        /** @brief Stop Loss level
         *
         * Entry and reversal deals use the Stop Loss values from the original order based on which
         * the position was opened or reversed. Exit deals use the Stop Loss of a position as at the
         * time of position closing.
         */
        double stop_loss;

        /** @brief Take Profit level
         *
         * Entry and reversal deals use the Take Profit values from the original order based on
         * which the position was opened or reversed. Exit deals use the Take Profit value of a
         * position as at the time of position closing.
         */
        double take_profit;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &volume;
            ar &price;
            ar &commission;
            ar &swap;
            ar &profit;
            ar &fee;
            ar &stop_loss;
            ar &take_profit;
        }
    };

    struct DealInfoString
    {
        /** @brief Deal symbol */
        std::string symbol;

        /** @brief Deal comment */
        std::string comment;

        /** @brief Deal identifier in an external trading system (on the Exchange) */
        std::string external_id;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &symbol;
            ar &comment;
            ar &external_id;
        }
    };

    struct DealInfo : DealInfoInteger, DealInfoDouble, DealInfoString
    {
    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &boost::serialization::base_object<DealInfoInteger>(*this);
            ar &boost::serialization::base_object<DealInfoDouble>(*this);
            ar &boost::serialization::base_object<DealInfoString>(*this);
        }
    };

}  // namespace mql::mql5

namespace mql::mql4
{
    struct OrderInfo
    {
        double close_price;
        MQL_DateTime close_time;
        std::string comment;
        double commission;
        MQL_DateTime expiration;
        double lots;
        MQL_int magic;
        double open_price;
        MQL_DateTime open_time;
        double order_profit;
        double stop_loss;
        double swap;
        std::string symbol;
        double take_profit;
        MQL_int ticket;
        MQL_int type;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &close_price;
            ar &close_time;
            ar &comment;
            ar &commission;
            ar &expiration;
            ar &lots;
            ar &magic;
            ar &open_price;
            ar &open_time;
            ar &order_profit;
            ar &stop_loss;
            ar &swap;
            ar &symbol;
            ar &ticket;
            ar &type;
        }
    };
}  // namespace mql::mql4