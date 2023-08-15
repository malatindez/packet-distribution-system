#pragma once
#include "common.hpp"
#include "enums.hpp"
namespace mql::common
{
    struct AccountInfoDouble
    {
        double balance;
        double credit;
        double profit;
        double equity;
        double margin;
        double margin_free;
        double margin_level;
        double margin_so_call;
        double margin_so_so;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &balance;
            ar &credit;
            ar &profit;
            ar &equity;
            ar &margin;
            ar &margin_free;
            ar &margin_level;
            ar &margin_so_call;
            ar &margin_so_so;
        }
    };

    struct AccountInfoString
    {
        std::string account_name;
        std::string trade_server_name;
        std::string account_currency;
        std::string account_company;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &account_name;
            ar &trade_server_name;
            ar &account_currency;
            ar &account_company;
        }
    };

    struct AccountInfoInteger
    {
        MQL_long account_login;
        EnumAccountTradeMode trade_mode;
        MQL_long account_leverage;
        MQL_int limit_orders;
        EnumAccountStopOutMode margin_so_mode;
        bool trade_allowed;
        bool expert_trade_allowed;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &account_login;
            ar &trade_mode;
            ar &account_leverage;
            ar &limit_orders;
            ar &margin_so_mode;
            ar &trade_allowed;
            ar &expert_trade_allowed;
        }
    };

    struct AccountInfoDoubleMinimal
    {
        double balance;
        double profit;
        double equity;
        double margin;
        double margin_free;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &balance;
            ar &profit;
            ar &equity;
            ar &margin;
            ar &margin_free;
        }
    };

    struct AccountInfoIntegerMinimal
    {
        MQL_long account_login;
        MQL_long account_leverage;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &account_login;
            ar &account_leverage;
        }
    };

    struct AccountInfoMinimal : AccountInfoIntegerMinimal,
                                AccountInfoDoubleMinimal,
                                AccountInfoString
    {
    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &boost::serialization::base_object<AccountInfoIntegerMinimal>(*this);
            ar &boost::serialization::base_object<AccountInfoDoubleMinimal>(*this);
            ar &boost::serialization::base_object<AccountInfoString>(*this);
        }
    };
}  // namespace mql::common
namespace mql::mql4
{
    using common::AccountInfoDouble;
    using common::AccountInfoDoubleMinimal;
    using common::AccountInfoInteger;
    using common::AccountInfoIntegerMinimal;
    using common::AccountInfoMinimal;
    using common::AccountInfoString;

    struct FullAccountInfo : AccountInfoInteger, AccountInfoDouble, AccountInfoString
    {
    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &boost::serialization::base_object<AccountInfoInteger>(*this);
            ar &boost::serialization::base_object<AccountInfoDouble>(*this);
            ar &boost::serialization::base_object<AccountInfoString>(*this);
        }
    };
}  // namespace mql::mql4
namespace mql::mql5
{
    using common::AccountInfoDoubleMinimal;
    using common::AccountInfoIntegerMinimal;
    using common::AccountInfoMinimal;
    using common::AccountInfoString;

    struct AccountInfoInteger : common::AccountInfoInteger
    {
        EnumAccountMarginMode margin_mode;
        MQL_int currency_digits;
        bool fifo_close;
        bool hedge_allowed;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &margin_mode;
            ar &currency_digits;
            ar &fifo_close;
            ar &hedge_allowed;
        }
    };

    struct AccountInfoDouble : common::AccountInfoDouble
    {
        double margin_initial;
        double margin_maintenance;
        double assets;
        double liabilities;
        double commission_blocked;

    private:
        friend class boost::serialization::access;
        template <class Archive>
        void serialize(Archive &ar, const unsigned int)
        {
            ar &margin_initial;
            ar &margin_maintenance;
            ar &assets;
            ar &liabilities;
            ar &commission_blocked;
        }
    };

}  // namespace mql::mql5
