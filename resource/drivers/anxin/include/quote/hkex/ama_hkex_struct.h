#ifndef AMD_AMA_HKEX_STRUCT_H_
#define AMD_AMA_HKEX_STRUCT_H_

#include "../ama_datatype.h"

#pragma pack(push)
#pragma pack(1)

namespace amd { namespace ama {

/*
    精度说明：
    本文中所有字段带有类型说明的相比如实际值有扩大倍数,详细倍数扩大规则如下(没有类型说明的就无扩大倍数)：
    类型:数量Qty (如成交量、报买量等)的扩大倍数是  10^2
    类型:价格Price (如参考价、报买价等)的扩大倍数是  10^6
    类型:金额Amt (如拆借金额等)的扩大倍数是  10^5
    类型:比例Rate (如最新利率、开盘利率等)的扩大倍数  10^6
*/

/**
 * @name MDHKExSnapshot 商业港股股票快照数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 */
struct MDHKExSnapshot
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 行情时间(为YYYYMMDDHHMMSSsss)
    int64_t pre_close_price;                                                // 昨收价(类型:价格Price)
    int64_t last_price;                                                     // 最新价(类型:价格Price)
    int64_t open_price;                                                     // 开盘价(类型:价格Price)
    int64_t high_price;                                                     // 最高价(类型:价格Price)
    int64_t low_price;                                                      // 最低价(类型:价格Price)
    int64_t close_price;                                                    // 收市价(类型:价格Price)
    int64_t total_volume_trade;                                             // 成交量(类型:数量Qty)
    int64_t total_value_trade;                                              // 成交额(类型:金额Amt)
    int64_t short_volume_shares;                                            // 沽空量(类型:数量Qty)
    int64_t short_value_turnover;                                           // 沽空额(类型:金额Amt)
    uint8_t variety_category;                                               // 品种类别
};

/**
 * @name MDHKExListItem 买卖单序列结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 */
struct MDHKExListItem
{
    int64_t order_price;                  // 挂单价格(类型:价格Price)
    int64_t order_volume;                 // 挂单数量(类型:数量Qty)
    int64_t num_of_orders;                // 挂单经纪席位数目
};


/**
 * @name MDHKExOrderSnapshot 商业港股委托挂单数据结构(嵌套在商业港股股票快照结构中,FiuType=FIU_ORDER,消息体=PBOrder)
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 */
struct MDHKExOrderSnapshot
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 时间(为YYYYMMDDHHMMSSsss)
    MDHKExListItem bid_list[ConstField::kPositionLevelLen];                 // 买单序列
    MDHKExListItem ask_list[ConstField::kPositionLevelLen];                 // 卖单序列
    uint8_t variety_category;                                               // 品种类别
};

/**
 * @name MDHKExDetailItem 明细结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 */
struct MDHKExDetailItem
{
    uint8_t level;                                                      // 档位信息
    uint16_t broker_num;                                                // 席位号
};

/**
 * @name MDHKExOrderBrokerSnapshot 商业港股经纪席位数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 */
struct MDHKExOrderBrokerSnapshot
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 时间(为YYYYMMDDHHMMSSsss)
    uint8_t side;                                                           // 买卖方向('B'买,'S'卖)
    char    broker_flag;                                                    // 是否还有未展示完的席位排位(Y-是 N-否)
    MDHKExDetailItem  detail[ConstField::kDetailLen];                       // 买卖经纪席位明细(所有档位的席位号数量之和最多有40个)
    uint8_t variety_category;                                               // 品种类别
};

/**
 * @name MDHKExTickExecution 商业港股逐笔成交数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 */
struct MDHKExTickExecution
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 时间(为YYYYMMDDHHMMSSsss)
    int64_t appl_seq_num;                                                   // 序号
    uint8_t side;                                                           // 买卖方向('B'主动买入,'-'中性盘,'S'主动卖出)
    int64_t exec_price;                                                     // 委托价格(类型:价格Price)
    int64_t exec_volume;                                                    // 委托数量(类型:数量Qty)
    uint8_t exec_type;                                                      // 成交类型
    //************************************成交类型取值及定义******************************************************
    //0-自动对盘 4-开市前成交盘 22-非自动对盘 100-同一证券商自动对盘
    //101-同一证券商非自动对盘 102-碎股交易 103-竞价交易
    char cancel_flag;                                                       // 撤单标识(Y 是/N 否)
    uint8_t variety_category;                                               // 品种类别
};

/**
 * @name MDHKExIndexSnapshot 商业港股指数行情快照数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 */
struct MDHKExIndexSnapshot
{
    int32_t market_type;                                                    // 市场类型
    char    security_code[ConstField::kSecurityCodeLen];                    // 证券代码
    int64_t orig_time;                                                      // 时间(为YYYYMMDDHHMMSSsss)
    char    index_status;                                                   // 指数状态
    int64_t pre_close_price;                                                // 昨收价(类型:价格Price)
    int64_t open_price;                                                     // 开盘价(类型:价格Price)
    int64_t high_price;                                                     // 最高价(类型:价格Price)
    int64_t low_price;                                                      // 最低价(类型:价格Price)
    int64_t close_price;                                                    // 收市价(类型:价格Price)
    int64_t last_price;                                                     // 最新价(类型:价格Price)
    int64_t total_volume_trade;                                             // 成交量(类型:数量Qty)
    int64_t total_value_trade;                                              // 成交额(类型:金额Amt)
    int64_t change;                                                         // 涨跌(类型:比例Rate)
    int64_t ratio_of_change;                                                // 涨跌%(类型:比例Rate)
    int64_t eas_value;                                                      // 预估结算值(类型:金额Amt)
    uint8_t variety_category;                                               // 品种类别
};


}; // end of ama
}; // end of amd

#pragma pack(pop)
#endif