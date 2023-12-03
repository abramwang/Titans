#ifndef TI_QUOTE_STRUCT_H
#define TI_QUOTE_STRUCT_H

#include "ti_common_struct.h"

const int32_t TI_STOCK_ARRAY_LEN  = 10;
const int32_t TI_FUTURE_ARRAY_LEN = 5;
const int32_t TI_ORDERBOOK_ARRAY_LEN = 64;

const char TI_QUOTE_DATA_TYPE_NONE            = '0';
const char TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK  = 'S';
const char TI_QUOTE_DATA_TYPE_SNAPSHOT_INDEX  = 'I';
const char TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE = 'F';
const char TI_QUOTE_DATA_TYPE_ORDER           = 'O';
const char TI_QUOTE_DATA_TYPE_MATCH           = 'M';
const char TI_QUOTE_DATA_TYPE_ORDERBOOK_LEVEL = 'L';
typedef char TI_QUOTE_DATA_TYPE;


const char TI_QUOTE_EVENT_TYPE_CONNECTED        = 'C';
const char TI_QUOTE_EVENT_TYPE_DISCONNECTED     = 'D';
const char TI_QUOTE_EVENT_TYPE_LOGON            = 'L';
const char TI_QUOTE_EVENT_TYPE_LOGOUT           = 'O';
typedef char TI_QUOTE_EVENT_TYPE;

#define USE_SHARD_PTR 1

#pragma pack(1)

struct TiSymbolInfo
{
    TI_ExchangeType exchange; 
    TI_SymbolType   symbol;   
};

/// @brief 快照定义
struct TiQuoteSnapshotStockField
{
    TI_ExchangeType exchange;
    TI_SymbolType symbol;
    int32_t date;
    int32_t time;
    int64_t timestamp;
    TI_ISODateTimeType time_str;
    TI_ISODateTimeType recv_time_str;
    int64_t recv_timestamp;
    double last;
    double pre_close;
    double open;
    double high;
    double low;
    double high_limit;
    double low_limit;
    int64_t volume;
    double turnover;
    int64_t acc_volume;
    double acc_turnover;
    int32_t match_items;
    double total_ask_qty;
    double total_bid_qty;
    double wavg_ask;
    double wavg_bid;
    double interest;
    double ask_price[TI_STOCK_ARRAY_LEN];
    int32_t ask_volume[TI_STOCK_ARRAY_LEN];
    int32_t ask_order_num[TI_STOCK_ARRAY_LEN];
    double bid_price[TI_STOCK_ARRAY_LEN];
    int32_t bid_volume[TI_STOCK_ARRAY_LEN];
    int32_t bid_order_num[TI_STOCK_ARRAY_LEN];
};

/// @brief 快照定义
struct TiQuoteSnapshotFutureField
{
    TI_ExchangeType exchange;
    TI_SymbolType symbol;
    int32_t date;
    int32_t time;
    int64_t timestamp;
    TI_ISODateTimeType time_str;
    TI_ISODateTimeType recv_time_str;
    int64_t recv_timestamp;
    double last;
    double pre_settlement_close;    ///上次结算价
    double pre_open_interest;       ///昨持仓量
    double pre_close;
    double open;
    double high;
    double low;
    double close;                   ///今收盘
    double settlement;              ///本次结算价
    double high_limit;
    double low_limit;
    int64_t volume;
    double turnover;
    int64_t acc_volume;
    double acc_turnover;
    double open_interest;          ///持仓量
    double pre_delta;               ///昨虚实度
    double curr_delta;              ///今虚实度
    double ave_price;               ///当日均价
    double ask_price[TI_FUTURE_ARRAY_LEN];
    int32_t ask_volume[TI_FUTURE_ARRAY_LEN];
    double bid_price[TI_FUTURE_ARRAY_LEN];
    int32_t bid_volume[TI_FUTURE_ARRAY_LEN];
};

/// @brief 快照定义
struct TiQuoteSnapshotIndexField
{
    TI_ExchangeType exchange;
    TI_SymbolType symbol;
    int32_t date;
    int32_t time;
    int64_t timestamp;
    TI_ISODateTimeType time_str;
    TI_ISODateTimeType recv_time_str;
    int64_t recv_timestamp;
    double last;
    double pre_close;
    double open;
    double high;
    double low;
    int64_t volume;
    double turnover;
    double close;
};

/// @brief 委托定义
struct TiQuoteOrderField
{
    TI_ExchangeType exchange;
    TI_SymbolType symbol;
    int32_t date;
    int32_t time;
    int64_t timestamp;
    TI_ISODateTimeType time_str;
    TI_ISODateTimeType recv_time_str;
    int64_t recv_timestamp;
    int32_t channel;
    int32_t seq;
    int32_t order_orino;
    double price;
    int32_t volume;
    char order_type;
    char function_code;
};

/// @brief 成交定义
struct TiQuoteMatchesField
{
    TI_ExchangeType exchange;
    TI_SymbolType symbol;
    int32_t date;
    int32_t time;
    int64_t timestamp;
    TI_ISODateTimeType time_str;
    TI_ISODateTimeType recv_time_str;
    int64_t recv_timestamp;
    int32_t channel;
    int32_t seq;
    double price;
    int32_t volume;
    char bs_flag;
    char function_code;
    int32_t ask_order_seq;
    int32_t bid_order_seq;
};

/// @brief 深度行情快照基本信息定义
struct TiQuoteDepthSnapshotBaseField
{
    TI_ExchangeType exchange;
    TI_SymbolType symbol;
    int32_t date;
    int32_t time;
    int64_t timestamp;
    TI_ISODateTimeType time_str;
    TI_ISODateTimeType recv_time_str;
    double last;
    double pre_close;
    double open;
    double high;
    double low;
    double high_limit;
    double low_limit;
    int64_t acc_volume;
    double acc_turnover;
    int32_t match_items;
};

/// @brief 深度行情快照基本信息定义
struct TiQuoteDepthLevel
{
    double price;
    int32_t volume;
    int32_t order_count;
};

/// @brief 深度行情OrderBookLevel定义
struct TiQuoteDepthOrder
{
    int32_t seq;
    int32_t order_orino;
    int32_t volume;
    int32_t dealt_vol;
};

/// @brief 深度行情OrderBook定义
struct TiQuoteOrderBookField
    : public TiQuoteDepthSnapshotBaseField
{
    int32_t level;
    double bid_price;
    int32_t bid_volume;
    int32_t bid_order_count;
    double ask_price;
    int32_t ask_volume;
    int32_t ask_order_count;
    TiQuoteDepthOrder bid_orders[TI_ORDERBOOK_ARRAY_LEN];
    TiQuoteDepthOrder ask_orders[TI_ORDERBOOK_ARRAY_LEN];
};

/// @brief 行情信息整合结构体
struct TiQuoteData
{
    TI_QUOTE_DATA_TYPE              type;
    TiQuoteSnapshotStockField       m_snapshot_stock;
    TiQuoteSnapshotIndexField       m_snapshot_index;
    TiQuoteOrderField               m_order;
    TiQuoteMatchesField             m_matches;
};

#pragma pack()
#endif