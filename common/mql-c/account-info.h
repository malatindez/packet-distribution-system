#ifndef ACCOUNT_INFO_C_H
#define ACCOUNT_INFO_C_H

#ifdef __cplusplus
extern "C" {
#endif
#ifndef ACCOUNT_INFO_H
#define ACCOUNT_INFO_H

#include "common_alternate.h"
#include "enums_alternate.h"

    typedef struct {
        double balance;
        double credit;
        double profit;
        double equity;
        double margin;
        double margin_free;
        double margin_level;
        double margin_so_call;
        double margin_so_so;
    } AccountInfoDouble;

    typedef struct {
        char* account_name;
        char* trade_server_name;
        char* account_currency;
        char* account_company;
    } AccountInfoString;

    typedef struct {
        MQL_long account_login;
        EnumAccountTradeMode trade_mode;
        MQL_long account_leverage;
        MQL_int limit_orders;
        EnumAccountStopOutMode margin_so_mode;
        int trade_allowed;
        int expert_trade_allowed;
    } AccountInfoInteger;

    typedef struct {
        double balance;
        double profit;
        double equity;
        double margin;
        double margin_free;
    } AccountInfoDoubleMinimal;

    typedef struct {
        MQL_long account_login;
        MQL_long account_leverage;
    } AccountInfoIntegerMinimal;

    typedef struct {
        AccountInfoIntegerMinimal integer_minimal_info;
        AccountInfoDoubleMinimal double_minimal_info;
        AccountInfoString string_info;
    } AccountInfoMinimal;

    typedef struct {
        AccountInfoInteger integer_info;
        AccountInfoDouble double_info;
        AccountInfoString string_info;
    } FullAccountInfo;

    typedef struct {
        AccountInfoInteger base_info;
        EnumAccountMarginMode margin_mode;
        MQL_int currency_digits;
        int fifo_close;
        int hedge_allowed;
    } MQL5_AccountInfoInteger;

    typedef struct {
        AccountInfoDouble base_info;
        double margin_initial;
        double margin_maintenance;
        double assets;
        double liabilities;
        double commission_blocked;
    } MQL5_AccountInfoDouble;
} 
#endif