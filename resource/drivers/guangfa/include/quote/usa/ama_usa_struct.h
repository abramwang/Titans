#ifndef AMD_AMA_USA_STRUCT_H_
#define AMD_AMA_USA_STRUCT_H_

#include "../ama_datatype.h"

#pragma pack(push)
#pragma pack(1)

namespace amd { namespace ama {
/*
    精度说明:
    本文中所有字段带有类型说明的相比如实际值有扩大倍数,详细倍数扩大规则如下(没有类型说明的就无扩大倍数):
    类型:数量Qty        (如成交量,报买量等)的扩大倍数是 10^2
    类型:价格Price      (如参考价,报买价等)的扩大倍数是 10^6
    类型:金额Amt        (如拆借金额等)的扩大倍数是 10^5
*/

/**
 * @name MDUsaStockSnapshot 美股股票快照数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ */
struct MDUsaStockSnapshot
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 时间(为 YYYYMMDDHHMMSSsss 美东时间)
    int64_t pre_close_price;                                                // 昨收价(单位:元)(类型:价格Price)
    int64_t now_price;                                                      // 现价(区别于最新成交价.是经处理过的有效价格,用于画分时图/计算涨跌)(类型:价格Price)
    int64_t open_price;                                                     // 开盘价(单位:元)(类型:价格Price)
    int64_t high_price;                                                     // 最高价(仅盘中时段统计,单位:元)(类型:价格Price)
    int64_t low_price;                                                      // 最低价(仅盘中时段统计,单位:元)(类型:价格Price)
    int64_t close_price;                                                    // 收盘价(单位:元)(类型:价格Price)
    int64_t total_volume_trade;                                             // 总成交量(Nasdaq 交易所,统计全天盘前盘中盘后三个时段的所有成交,单位:股)(类型:数量Qty)
    int64_t total_value_trade;                                              // 总成交额(全美成交额估算值,统计全天盘前盘中盘后三个时段,单位:元)(类型:金额Amt)
    int64_t all_market_vol;                                                 // 全美总成交量(全美交易所累计,统计全天盘前盘中盘后三个时段,单位:股)(类型:数量Qty)
    int64_t exec_price;                                                     // 最新成交价(单位:元)(类型:价格Price)
    int64_t exec_volume;                                                    // 最新成交量(当为负数,则表示撤单;当为 0,则表示开盘、收盘或EOD或者其他,可以过滤掉该笔逐笔,单位:股)(类型:数量Qty)
    int32_t regsho;                                                         // 卖空标识(表示消息发布时证券的卖空价格限制状态,可为空)
    //************************************卖空标识*************************************************************
    // 48 表示卖空限制解除  49 表示卖空限制生效  50 表示卖空限制继续
    char    trade_status[ConstField::kUsaTradeStatusLen];                   // 交易状态(标识证券的当前交易状态)
    //************************************交易状态*************************************************************
    // H 表示停止交易(全美范围) P 表示暂停交易(Nasdaq上市的证券) Q 表示仅有效期报价(全美范围) T 表示交易中(Nasdaq市场)
    int32_t trade_session_id;                                               // 交易时段(标识该笔交易发生的时段)
    //************************************交易时段*************************************************************
    // -3表示准备03:00-04:00  -1表示盘前04:00-09:30  1表示盘中09:30-16:00  -2表示盘后16:00-20:00  0表示无发生交易或不适用
    int32_t trade_market;                                                   // 交易所发生的市场
    //************************************交易所发生的市场*******************************************************
    // 81 表示The Nasdaq Stock Market;  76 表示Nasdaq/FINRA Trade Reporting Facility (TRF) Cartaret;  50 表示Nasdaq/FINRA Trade Reporting Facility (TRF) Chicago;
    // 66 表示Nasdaq BX (BX);  88 表示Nasdaq PSX (PSX)
    char    exec_type[ConstField::kUsaExecTypeLen];                         // 成交类型(全美eod数据那一笔的成交类型为:eod)
    int32_t side;                                                           // 买卖方向(1 表示主动买入;0 表示中性盘;-1 表示主动卖出)
};

/**
 * @name MDUsaOrderSnapshot 美股挂单快照结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ */
struct MDUsaOrderSnapshot
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 行情时间(为 YYYYMMDDHHMMSSsss 美东时间)
    int64_t bid_price;                                                      // 买一价(单位:元)(类型:价格Price)
    int64_t bid_volume;                                                     // 买一量(单位:股)(类型:数量Qty)
    int64_t offer_price;                                                    // 卖一价(单位:元)(类型:价格Price)
    int64_t offer_volume;                                                   // 卖一量(单位:股)(类型:数量Qty)
};

/**
 * @name MDUsaEodSnapshot 美股EOD收盘快照数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ */
struct MDUsaEodSnapshot
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 行情时间(为 YYYYMMDDHHMMSSsss 美东时间)
    int64_t open_price;                                                     // 开盘价(单位:元)(类型:价格Price)
    int64_t high_price;                                                     // 最高价(单位:元)(类型:价格Price)
    int64_t low_price;                                                      // 最低价(单位:元)(类型:价格Price)
    int64_t close_price;                                                    // 收盘价(单位:元)(类型:价格Price)
    int64_t all_market_vol;                                                 // 全美总成交量(单位:股)(类型:数量Qty)
};

}; // end of ama
}; // end of amd

#pragma pack(pop)
#endif