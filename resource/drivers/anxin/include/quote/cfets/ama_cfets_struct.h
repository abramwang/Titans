#ifndef AMD_AMA_CFETS_STRUCT_H_
#define AMD_AMA_CFETS_STRUCT_H_

#include "../ama_datatype.h"
#include "../ama_export.h"
#include <string>

#pragma pack(push)
#pragma pack(1)

namespace amd { namespace ama {

/*
精度说明:
本文中所有字段带有类型说明的相比如实际值有扩大倍数,详细倍数扩大规则如下(没有类型说明的就无扩大倍数):
类型:数量Qty        (如成交量,报买量等)的扩大倍数是 10^2
类型:数量CMDS-Qty   (如当日累计成交量等)的扩大倍数是 10^4
类型:数量RDI-Qty    (如增发量,行权量等)的扩大倍数是 10^10
类型:价格Price      (如参考价,报买价等)的扩大倍数是 10^6
类型:金额Amt        (如拆借金额等)的扩大倍数是 10^5
类型:比例Rate       (如最新利率,开盘利率等)的扩大倍数 10^6
类型:面额Par        (如发行价等)的扩大倍数是 10^10
类型:汇率ExRate     (如应计利息等)的扩大倍数 10^8
类型:利息Interest   (如每百元应计利息)的扩大倍数 10^15
*/

/**
* @name MDSpotTickExecution 现券买卖逐笔成交数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDSpotTickExecution
{
   int32_t  market_type;                                                     // 市场类型
   char     msg_type[ConstField::kMsgTypeLen];                               // 报文类型(8-成交单报文)
   int64_t  msg_seq_num;                                                     // 报文序号
   int64_t  sending_time;                                                    // 报文发送时间(YYYYMMDDHHMMSSsss)
   char     security_code[ConstField::kCFETSSecurityCodeLen];                // 证券代码
   char     symbol[ConstField::kSymbolLen];                                  // 证券简称
   int64_t  orig_time;                                                       // 成交时间(YYYYMMDDHHMMSSsss)
   char     market_indicator[ConstField::kMarketIndicatorLen];               // 市场标识(4-现券买卖)
   char     deal_number[ConstField::kExecIDLen];                             // 成交编号
   char     exec_type[ConstField::kExecTypeLen];                             // 成交类型(枚举值:F-成交 4-撤销 5-更新)
   int32_t  trade_type;                                                      // 成交类别(枚举值:1-非做市 4-买方做市 5-卖方做市)
   int64_t  exec_price;                                                      // 净价(单位:元;类型:价格Price)
   int64_t  exec_yield;                                                      // 收益率(单位:%;类型:比例Rate)
   int64_t  total_face_value;                                                // 券面总额(单位:元;类型:金额Amt)
   char     pre_market_bond_indicator;                                       // 上市前债券(枚举值:Y-YES N-NO)
   int32_t  trade_method;                                                    // 交易方式(枚举值:1-协商 3-匿名点击 4-请求报价 5-点击成交 11-匿名拍卖)
   char     transaction_method[ConstField::kTransactionMethodLen];           // 成交方向(取值:TKN GVN TRD DONE)
   uint8_t  variety_category;                                                // 品种类别
};
/**  @} */

/**
* @name MDSpotSummarySnapshot 现券买卖汇总成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDSpotSummarySnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(4-现券买卖)
    int32_t  trade_method;                                                   // 交易方式(0-汇总)
    char     md_type;                                                        // 市场行情类型(R-表示本币市场)
    int32_t  md_sub_type;                                                    // 市场行情子类别(7-代表交易品种行情)
    int64_t  pre_close_clean_price;                                          // 前收盘净价(类型:价格Price)
    int64_t  pre_weighted_avg_clean_price;                                   // 前加权平均净价(类型:价格Price)
    int64_t  open_clean_price;                                               // 开盘净价(类型:价格Price)
    int64_t  last_clean_price;                                               // 最新净价(类型:价格Price)
    int64_t  ratio_of_change;                                                // 涨跌幅(类型:比例Rate)
    int64_t  high_clean_price;                                               // 最高净价(类型:价格Price)
    int64_t  low_clean_price;                                                // 最低净价(类型:价格Price)
    int64_t  close_clean_price;                                              // 收盘净价(类型:价格Price)
    int64_t  weighted_avg_clean_price;                                       // 加权平均净价(类型:价格Price)
    int64_t  pre_close_yield;                                                // 前收盘收益率(类型:比例Rate)
    int64_t  pre_weighted_avg_yield;                                         // 前加权平均收益率(类型:比例Rate)
    int64_t  open_yield;                                                     // 开盘收益率(类型:比例Rate)
    int64_t  last_yield;                                                     // 最新收益率(类型:比例Rate)
    int64_t  high_yield;                                                     // 最高收益率(类型:比例Rate)
    int64_t  low_yield;                                                      // 最低收益率(类型:比例Rate)
    int64_t  close_yield;                                                    // 收盘收益率(类型:比例Rate)
    int64_t  weighted_avg_yield;                                             // 加权平均收益率(类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:百万;类型:数量CMDS-Qty)
    int64_t  num_trades;                                                     // 成交笔数(类型:数量CMDS-Qty)
    int64_t  change_over_net_price;                                          // 净价涨跌(类型:比例Rate)
    int64_t  change_over_yields;                                             // 收益率涨跌(类型:比例Rate)
    int64_t  term_to_maturity;                                               // 年化待偿期(类型:比例Rate)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */


/**
* @name MDSpotXBondSnapshot 现券买卖匿名点击成交行情(X-Bond)数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDSpotXBondSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(w)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(4-现券买卖)
    int32_t  trade_method;                                                   // 交易方式(3-匿名点击)
    char     md_type;                                                        // 市场行情类型(R-表示人民币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(0-成交行情)
    char     settl_type[ConstField::kSettlTypeLen];                          // 清算速度(1-T+0 2-T+1)
    char     side;                                                           // 最新成交方向(1-买入 2-卖出)
    int64_t  last_clean_price;                                               // 最新成交净价(单位:元;类型:价格Price)
    int64_t  last_yield;                                                     // 最新成交到期收益率(单位:%;类型:比例Rate)
    int64_t  high_clean_price;                                               // 最高成交净价(单位:元;类型:价格Price)
    int64_t  high_yield;                                                     // 最高成交到期收益率(单位:%;类型:比例Rate)
    int64_t  low_clean_price;                                                // 最低成交净价(单位:元;类型:价格Price)
    int64_t  low_yield;                                                      // 最低成交到期收益率(单位:%;类型:比例Rate)
    int64_t  open_clean_price;                                               // 开盘价(单位:元;类型:价格Price)
    int64_t  open_yield;                                                     // 开盘到期收益率(类型:比例Rate)
    int64_t  pre_open_clean_price;                                           // 前开盘净价(类型:价格Price)
    int64_t  pre_open_yield;                                                 // 前开盘到期收益率(类型:比例Rate)
    int64_t  weighted_avg_clean_price;                                       // 加权平均净价(类型:价格Price)
    int64_t  weighted_avg_yield;                                             // 加权平均到期收益率(类型:比例Rate)
    int64_t  pre_weighted_avg_clean_price;                                   // 前加权平均净价(类型:价格Price)
    int64_t  pre_weighted_avg_yield;                                         // 前加权平均到期收益率(类型:比例Rate)
    int64_t  pre_close_clean_price;                                          // 前收盘净价(类型:价格Price)
    int64_t  pre_close_yield;                                                // 前收盘到期收益率(类型:比例Rate)
    int64_t  ratio_of_change;                                                // 净价涨跌幅(类型:比例Rate)
    int64_t  change_over_yields;                                             // 收益率涨跌(类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:元;类型:数量CMDS-Qty)
    char     pre_market_bond_indicator;                                      // 上市前债券(Y-YES N-NO)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDPledgeRepoTickExecution 质押式回购逐笔成交数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDPledgeRepoTickExecution
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(8-表示成交单报文)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称(交易品种名称)
    int64_t  orig_time;                                                      // 成交时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(9-质押式回购)
    char     deal_number[ConstField::kExecIDLen];                            // 成交编号
    char     exec_type[ConstField::kExecTypeLen];                            // 成交类型(枚举值:F-成交 4-撤销 5-更新)
    int64_t  exec_price;                                                     // 回购利率(单位:%;类型:比例Rate)
    int64_t  exec_amount;                                                    // 交易金额(单位:元(四舍五入);类型:金额Amt)
    char     trade_limit_days[ConstField::kTradeLimitDaysLen];               // 回购期限(单位:天;整数)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDPledgeRepoSummarySnapshot 质押式回购汇总成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDPledgeRepoSummarySnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(9-质押式回购)
    int32_t  trade_method;                                                   // 交易方式(0-汇总行情)
    char     md_type;                                                        // 市场行情类型(R-人民币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(0-成交行情)
    int64_t  avg_term;                                                       // 平均回购期限(单位:天;类型:数量CMDS-Qty)
    int64_t  last_price;                                                     // 最新利率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘利率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高利率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低利率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均利率(单位:%;类型:比例Rate)
    int64_t  bond_weighted_avg_price;                                        // 加权平均利率(利率债)(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                         // 前加权平均利率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                    // 收盘利率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                // 前收盘利率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  num_trades;                                                     // 成交笔数(类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDPledgeRepoInquirySnapshot 质押式回购询价成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDPledgeRepoInquirySnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(9-质押式回购)
    char     md_type;                                                        // 市场行情类型(R-本币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(8-回购利率市场行情)
    int64_t  avg_term;                                                       // 平均回购期限(单位:天;类型:数量CMDS-Qty)
    int64_t  last_price;                                                     // 最新利率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘利率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高利率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低利率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均利率(单位:%;类型:比例Rate)
    int64_t  bond_weighted_avg_price;                                        // 加权平均利率(利率债)(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                         // 前加权平均利率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                    // 收盘利率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                // 前收盘利率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  num_trades;                                                     // 成交笔数(单位:笔;类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDPledgeRepoDepositSnapshot 质押式回购存款类机构间成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDPledgeRepoDepositSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(9-质押式回购)
    char     md_type;                                                        // 市场行情类型(R-表示本币市场)
    int32_t  md_sub_type;                                                    // 市场行情子类别(37-存款类机构)
    int64_t  avg_term;                                                       // 平均回购期限(单位:天;类型:数量CMDS-Qty)
    int64_t  last_price;                                                     // 最新利率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘利率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高利率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低利率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均利率(单位:%;类型:比例Rate)
    int64_t  bond_weighted_avg_price;                                        // 加权平均利率(利率债)(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                         // 前加权平均利率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                    // 收盘利率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                // 前收盘利率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:亿元;类型:数量CMDS-Qty)
    int64_t  num_trades;                                                     // 成交笔数(类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDPledgeRepoXRepoSnapshot 质押式回购双边匿名点击(X-Repo)成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDPledgeRepoXRepoSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 合约名称
    char     security_group[ConstField::kSecurityGroupLen];                  // 交易品种
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(9-质押式回购)
    int32_t  trade_method;                                                   // 交易方式(3-匿名点击)
    char     md_type;                                                        // 市场行情类型(R-本币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(0-成交行情)
    int64_t  last_price;                                                     // 最新利率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘利率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高利率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低利率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均利率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  num_trades;                                                     // 成交笔数(类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */


/**
* @name MDCeditLoanTickExecution 同业拆借逐笔成交数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDCeditLoanTickExecution
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(8-成交单报文)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 成交时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(1-信用拆借市场)
    char     deal_number[ConstField::kExecIDLen];                            // 成交编号
    char     exec_type[ConstField::kExecTypeLen];                            // 成交类型(0-正常 4-撤销 5-更新 7-停止 8-拒绝 F-已成交(部分或全部) H-交易撤销 101-应急)
    int64_t  exec_price;                                                     // 拆借利率(类型:比例Rate)
    int64_t  exec_amount;                                                    // 拆借金额(类型:金额Amt)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDCeditLoanSnapshot 同业拆借成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDCeditLoanSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(1-信用拆借)
    char     md_type;                                                        // 市场行情类型(R-表示本币市场)
    int32_t  md_sub_type;                                                    // 市场行情子类别(7-代表交易品种行情)
    int64_t  avg_term;                                                       // 平均拆借期限(单位:天;类型:数量CMDS-Qty)
    int64_t  last_price;                                                     // 最新利率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘利率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高利率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低利率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均利率(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                         // 前加权平均利率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                    // 收盘利率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                // 前收盘利率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  num_trades;                                                     // 成交笔数(类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDCeditLoanDepositSnapshot 同业拆借存款类机构间成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDCeditLoanDepositSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(交易品种)
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(1-信用拆借)
    char     md_type;                                                        // 市场行情类型(R-本币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(37-存款类机构行情)
    int64_t  avg_term;                                                       // 平均拆借期限(单位:天;类型:数量CMDS-Qty)
    int64_t  last_price;                                                     // 最新利率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘利率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高利率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低利率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均利率(单位:%;类型:比例Rate)
    int64_t  bond_weighted_avg_price;                                        // 加权平均利率(利率债)(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                         // 前加权平均利率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                    // 收盘利率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                // 前收盘利率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:亿元;类型:数量CMDS-Qty)
    int64_t  num_trades;                                                     // 成交笔数(类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDSwapFixedFloatTickExecution 利率互换固定-浮动逐笔成交数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDSwapFixedFloatTickExecution
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(8-成交单)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(2-利率互换市场)
    char     deal_number[ConstField::kExecIDLen];                            // 成交编号
    char     exec_type[ConstField::kExecTypeLen];                            // 成交类型(F-成交 4-撤销 5-更新)
    char     side;                                                           // 成交方向(J-固定对浮动利率)
    int64_t  price;                                                          // 名义本金金额(类型:金额Amt)
    int64_t  fixed_rate;                                                     // 固定利率(类型:比例Rate)
    char     benchmark_curve_name[ConstField::kBenchmarkCurveNameLen];       // 参考利率曲线名称
    int64_t  interest_rate_spread;                                           // 利差(类型:比例Rate)
    char     trade_limit_days[ConstField::kTradeLimitDaysLen];               // 交易期限
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDSwapFixedFloatSnapshot 利率互换固定-浮动成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDSwapFixedFloatSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(2-利率互换)
    char     md_type;                                                        // 市场行情类型(R-本币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(2-固定-浮动成交行情)
    char     benchmark_curve_name[ConstField::kBenchmarkCurveNameLen];       // 参考利率曲线名称
    char     trade_limit_days[ConstField::kTradeLimitDaysLen];               // 交易期限
    int64_t  last_price;                                                     // 最新固定利率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘固定利率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高固定利率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低固定利率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均固定利率(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                         // 前加权平均固定利率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                    // 收盘固定利率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                // 前收盘固定利率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:百万元;类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDSwapXSwapSnapshot 利率互换匿名点击(X-Swap)成交行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDSwapXSwapSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(交易品种)
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(2-利率互换)
    int32_t  trade_method;                                                   // 交易方式(3-匿名点击)
    char     md_type;                                                        // 市场行情类型(R-本币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(0-成交行情)
    int64_t  last_price;                                                     // 最新利率(单位:直接合约为% 期差合约为bp;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘利率(单位:直接合约为% 期差合约为bp;类型:比例Rate)
    int64_t  high_price;                                                     // 最高利率(单位:直接合约为% 期差合约为bp;类型:比例Rate)
    int64_t  low_price;                                                      // 最低利率(单位:直接合约为% 期差合约为bp;类型:比例Rate)
    int64_t  ref_price;                                                      // 盘中参考价(单位:直接合约为% 期差合约为bp;类型:价格Price)
    int64_t  volume_trade;                                                   // 最新成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:元;类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDSwapXSwapBestOfferSnapshot 利率互换匿名点击(X-Swap)最优报价行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDSwapXSwapBestOfferSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(合约品种)
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(2-利率互换)
    int32_t  trade_method;                                                   // 交易方式(3-匿名点击)
    char     md_type;                                                        // 市场行情类型(R-本币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(38-报价行情)
    int64_t  bid_price;                                                      // 报买价(单位:直接合约为% 期差合约为bp;类型:价格Price)
    int64_t  bid_volume;                                                     // 报买量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_price;                                                    // 报卖价(单位:直接合约为% 期差合约为bp;类型:价格Price)
    int64_t  offer_volume;                                                   // 报卖量(单位:元;类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
* @name MDBondLoanTickExecution 债券借贷逐笔成交数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDBondLoanTickExecution
{
    int32_t  market_type;                                                       // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                                 // 报文类型(8)
    int64_t  msg_seq_num;                                                       // 报文序号
    int64_t  sending_time;                                                      // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];                  // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                    // 证券简称
    int64_t  orig_time;                                                         // 成交时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];                 // 市场标识(8-债券借贷市场)
    char     deal_number[ConstField::kExecIDLen];                               // 成交编号
    char     exec_type[ConstField::kExecTypeLen];                               // 成交类型(F-成交 4-撤销 5-更新)
    int64_t  price;                                                             // 借贷费率(类型:比例Rate)
    char     underlying_security_code[ConstField::kUnderlyingSecurityCodeLen];  // 标的债券代码
    char     underlying_symbol[ConstField::kUnderlyingSymbolLen];               // 标的债券名称
    int64_t  underlying_qty;                                                    // 标的债券券面总额(类型:金额Amt)
    uint8_t  variety_category;                                                  // 品种类别
};
/**  @} */

/**
* @name MDBondLoanUnderlyingSecurityRateSnapshot 债券借贷标的债券借贷费率行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDBondLoanUnderlyingSecurityRateSnapshot
{
    int32_t  market_type;                                                       // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                                 // 报文类型(W)
    int64_t  msg_seq_num;                                                       // 报文序号
    int64_t  sending_time;                                                      // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     underlying_security_code[ConstField::kUnderlyingSecurityCodeLen];  // 标的债券代码
    char     underlying_symbol[ConstField::kUnderlyingSymbolLen];               // 标的债券名称
    int64_t  orig_time;                                                         // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];                 // 市场标识(8-债券借贷市场)
    char     md_type;                                                           // 市场行情类型(R-表示本币市场)
    int32_t  md_sub_type;                                                       // 市场行情子类别(3-表示标的债券行情)
    int64_t  last_price;                                                        // 最新费率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                        // 开盘费率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                        // 最高费率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                         // 最低费率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                                // 加权平均费率(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                            // 前加权平均费率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                       // 收盘费率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                   // 前收盘费率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                                // 当日累计成交量(单位:百万元;类型:数量CMDS-Qty)
    uint8_t  variety_category;                                                  // 品种类别
};
/**  @} */

/**
* @name MDBondLoanTradingProductRateSnapshot 债券借贷交易品种费率行情数据结构
* 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
* @{
* */
struct MDBondLoanTradingProductRateSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间(YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码(为标准品种时传标准品种代码,为非标准品种时传"User-Defined")
    char     symbol[ConstField::kSymbolLen];                                 // 证券简称
    int64_t  orig_time;                                                      // 行情时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(8-债券借贷)
    char     md_type;                                                        // 市场行情类型(R-本币市场行情)
    int32_t  md_sub_type;                                                    // 市场行情子类别(7-交易品种行情行情)
    int64_t  last_price;                                                     // 最新费率(单位:%;类型:比例Rate)
    int64_t  open_price;                                                     // 开盘费率(单位:%;类型:比例Rate)
    int64_t  high_price;                                                     // 最高费率(单位:%;类型:比例Rate)
    int64_t  low_price;                                                      // 最低费率(单位:%;类型:比例Rate)
    int64_t  weighted_avg_price;                                             // 加权平均费率(单位:%;类型:比例Rate)
    int64_t  pre_weighted_avg_price;                                         // 前加权平均费率(单位:%;类型:比例Rate)
    int64_t  close_price;                                                    // 收盘费率(单位:%;类型:比例Rate)
    int64_t  pre_close_price;                                                // 前收盘费率(单位:%;类型:比例Rate)
    int64_t  total_volume_trade;                                             // 当日累计成交量(单位:百万元;类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

// 现券买卖做市报价深度行情重复组
struct MarketMakersQuoteItem
{
    int32_t  quote_type;                                                     // 报价类型
    char     quote_entry_id[ConstField::kQuoteEntryIDLen];                   // 报价编号
    int64_t  tranding_time;                                                  // 业务发生时间(YYYYMMDDHHMMSSsss)
    int64_t  clean_price;                                                    // 净价(类型:价格Price)
    int64_t  maturity_yield;                                                 // 到期收益率(类型:比例Rate)
    int64_t  dirty_price;                                                    // 全价(类型:价格Price)
    int64_t  total_face_value;                                               // 全面总额(类型:金额Amt)
    int32_t  clearing_method;                                                // 清算类型(6-净额清算 13-全额清算 4001-净额清算或全额清算)
    char     settl_type[ConstField::kSettlTypeLen];                          // 清算速度(1-T+0 2-T+1)
    int32_t  settl_date;                                                     // 结算日(YYYYMMDD)
    int32_t  delivery_type;                                                  // 结算方式(0-券款对付 4-见券付款 5-见款付券)
    char     settl_currency[ConstField::kSettlCurrencyLen];                  // 结算币种
    int64_t  settl_curr_fx_rate;                                             // 汇率(类型:比例Rate)
    char     trade_account_id[ConstField::kAccountIDLen];                    // 做市方交易账户6位码
    char     trader_id[ConstField::kTraderIDLen];                            // 做市方交易员ID
};


/**
 * @name MDSpotMarketMakersQuoteSnapshot 现券买卖做市报价深度行情数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSpotMarketMakersQuoteSnapshot
{
    int32_t  market_type;                                                       // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                                 // 报文类型(W)
    int64_t  msg_seq_num;                                                       // 报文序号
    int64_t  sending_time;                                                      // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];                  // 证券代码
    char     symbol[ConstField::kSymbolLen];                                    // 证券简称
    char     market_indicator[ConstField::kMarketIndicatorLen];                 // 市场标识(4-现券买卖市场)
    int32_t  md_book_type;                                                      // 订阅行情类型(2-市场深度行情)
    int32_t  md_sub_book_type;                                                  // 订阅行情子类型(123-做市报价)
    char     security_type[ConstField::kSecurityTypeLen];                       // 证券类型(债券类型)
    char     md_req_id[ConstField::kExecIDLen];                                 // 订阅请求ID(客户参考编号)
    MarketMakersQuoteItem bid[ConstField::kPositionLevelLen];                   // 买入档位信息
    MarketMakersQuoteItem offer[ConstField::kPositionLevelLen];                 // 卖出档位信息
    uint8_t  variety_category;                                                  // 品种类别
};
/**  @} */

// 现券买卖指示性报价深度行情重复组
struct DirectiveQuoteItem
{
    int64_t  tranding_time;                                                     // 业务发生时间(YYYYMMDDHHMMSSsss)
    int64_t  clean_price;                                                       // 净价(类型:价格Price)
    int64_t  maturity_yield;                                                    // 到期收益率(类型:比例Rate)
    char     settl_type[ConstField::kSettlTypeLen];                             // 清算速度(1-T+0 2-T+1)
    char     party_id[ConstField::kPartyIDLen];                                 // 交易账户21位代码
};

/**
 * @name MDSpotDirectiveQuoteSnapshot 现券买卖指示性报价深度行情数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSpotDirectiveQuoteSnapshot
{
    int32_t  market_type;                                                       // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                                 // 报文类型(W)
    int64_t  msg_seq_num;                                                       // 报文序号
    int64_t  sending_time;                                                      // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];                  // 证券代码
    char     market_indicator[ConstField::kMarketIndicatorLen];                 // 市场标识(4-现券买卖市场)
    int32_t  md_book_type;                                                      // 订阅行情类型(2-市场深度行情)
    int32_t  md_sub_book_type;                                                  // 订阅行情子类型(124-指示性报价)
    char     security_type[ConstField::kSecurityTypeLen];                       // 证券类型(债券类型)
    char     md_req_id[ConstField::kExecIDLen];                                 // 订阅请求ID(客户参考编号)
    DirectiveQuoteItem bid[ConstField::kDireciveLevelLen];                      // 买入档位信息
    DirectiveQuoteItem offer[ConstField::kDireciveLevelLen];                    // 卖出档位信息
    uint8_t  variety_category;                                                  // 品种类别
};
/**  @} */

/**
 * @name MDSpotXBondDepthQuoteSnapshot 现券买卖匿名点击(X-Bond)报价深度行情数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSpotXBondDepthQuoteSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    int64_t  orig_time;                                                      // 业务发生时间(YYYYMMDDHHMMSSsss)
    int32_t  md_book_type;                                                   // 订阅行情类型(2-市场深度行情 107-集中匹配行情)
    char     md_req_id[ConstField::kExecIDLen];                              // 订阅请求ID(客户参考编号)
    char     settl_type[ConstField::kSettlTypeLen];                          // 清算速度(1-T+0 2-T+1)
    int64_t  bid_clean_price[ConstField::kPositionLevel6];                   // 报买净价(单位:元;类型:价格Price)
    int64_t  bid_volume[ConstField::kPositionLevel6];                        // 报买量(单位:元;类型:数量CMDS-Qty)
    int64_t  bid_yield[ConstField::kPositionLevel6];                         // 报买到期收益率(单位:%;类型:比例Rate)
    int64_t  offer_clean_price[ConstField::kPositionLevel6];                 // 报卖净价(单位:元;类型:价格Price)
    int64_t  offer_volume[ConstField::kPositionLevel6];                      // 报卖量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_yield[ConstField::kPositionLevel6];                       // 报卖到期收益率(单位:%;类型:比例Rate)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
 * @name MDSpotXBondMatchQuoteSnapshot 现券买卖匿名点击(X-Bond)报价集中匹配行情数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSpotXBondMatchQuoteSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    int64_t  orig_time;                                                      // 业务发生时间(YYYYMMDDHHMMSSsss)
    int32_t  md_book_type;                                                   // 订阅行情类型(2-市场深度行情 107-集中匹配行情)
    char     md_req_id[ConstField::kExecIDLen];                              // 订阅请求ID(客户参考编号)
    char     settl_type[ConstField::kSettlTypeLen];                          // 清算速度(1-T+0 2-T+1)
    int64_t  bid_volume;                                                     // 买入未匹配量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_volume;                                                   // 卖出未匹配量(单位:元;类型:数量CMDS-Qty)
    int64_t  total_available_volume;                                         // 可成交总量(单位:元;类型:数量CMDS-Qty)
    int64_t  open_clean_price;                                               // 开盘净价(单位:元;类型:价格Price))
    int64_t  open_yield;                                                     // 开盘到期收益率(单位:%;类型:比例Rate)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
 * @name MDPledgeRepoXRepoDepthQuoteSnapshot 质押式回购匿名点击(X-Repo)报价深度行情数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDPledgeRepoXRepoDepthQuoteSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    int64_t  orig_time;                                                      // 业务发生时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(9-质押式回购市场 )
    int32_t  md_book_type;                                                   // 订阅行情类型(2-市场深度行情 1-最优行情)
    char     match_type;                                                     // 回购方式 ("1"-双边回购  "3"-通用回购)
    char     md_req_id[ConstField::kExecIDLen];                              // 订阅请求ID(客户参考编号)
    int64_t  bid_price[ConstField::kPositionLevel6];                         // 逆回购利率(单位:%;类型:比例Rate)
    int64_t  bid_volume[ConstField::kPositionLevel6];                        // 逆回购可成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  bid_trade_volume[ConstField::kPositionLevel6];                  // 逆回购量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_price[ConstField::kPositionLevel6];                       // 正回购利率(单位:%;类型:比例Rate)
    int64_t  offer_volume[ConstField::kPositionLevel6];                      // 正回购可成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_trade_volume[ConstField::kPositionLevel6];                // 正回购量(单位:元;类型:数量CMDS-Qty)  
    char     account_id[ConstField::kAccountIDLen];                          // 本方交易账户6位码
    uint8_t  variety_category;                                               // 品种类别 
};
/**  @} */

/**
 * @name MDPledgeRepoXRepoBestOfferQuoteSnapshot 质押式回购匿名点击(X-Repo)报价最优行情数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDPledgeRepoXRepoBestOfferQuoteSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    int64_t  orig_time;                                                      // 业务发生时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(9-质押式回购市场)
    int32_t  md_book_type;                                                   // 订阅行情类型(2-市场深度行情 1-最优行情)
    char     match_type;                                                     // 回购方式 ("1"-双边回购  "3"-通用回购)
    char     md_req_id[ConstField::kExecIDLen];                              // 订阅请求ID(客户参考编号)
    int64_t  bid_price;                                                      // 逆回购利率(单位:%;类型:比例Rate)
    int64_t  bid_trade_volume;                                               // 逆回购量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_price;                                                    // 正回购利率(单位:%;类型:比例Rate)
    int64_t  offer_trade_volume;                                             // 正回购量(单位:元;类型:数量CMDS-Qty)  
    uint8_t  variety_category;                                               // 品种类别 
};
/**  @} */

/**
 * @name MDSwapXSwapDepthQuoteSnapshot 利率互换匿名点击(X-Swap)报价深度行情数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSwapXSwapDepthQuoteSnapshot
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    int64_t  orig_time;                                                      // 业务发生时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(2-利率互换市场)
    int32_t  md_book_type;                                                   // 订阅行情类型(2-市场深度行情 1-最优行情)
    char     real_time_under_take_flag;                                      // 实时承接标识('1'-是 '0'-否)
    char     account_subject_type[ConstField::kAccountSubjectTypeLen];       // 账户主体类型("103"-自营 "104"-代理 "105"-资管)
    int64_t  bid_price[ConstField::kDireciveLevelLen];                       // 买价(单位:%;类型:价格Price)
    int64_t  bid_volume[ConstField::kDireciveLevelLen];                      // 买单可成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  bid_trade_volume[ConstField::kDireciveLevelLen];                // 买单总量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_price[ConstField::kDireciveLevelLen];                     // 卖价(单位:%;类型:价格Price)
    int64_t  offer_volume[ConstField::kDireciveLevelLen];                    // 卖单可成交量(单位:元;类型:数量CMDS-Qty)
    int64_t  offer_trade_volume[ConstField::kDireciveLevelLen];              // 卖单总量(单位:元;类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

/**
 * @name MDSwapXSwapTickExecution 利率互换匿名点击(X-Swap)逐笔成交数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSwapXSwapTickExecution
{
    int32_t  market_type;                                                    // 市场类型
    char     msg_type[ConstField::kMsgTypeLen];                              // 报文类型(W)
    int64_t  msg_seq_num;                                                    // 报文序号
    int64_t  sending_time;                                                   // 报文发送时间 (YYYYMMDDHHMMSSsss)
    char     security_code[ConstField::kCFETSSecurityCodeLen];               // 证券代码
    int64_t  orig_time;                                                      // 业务发生时间(YYYYMMDDHHMMSSsss)
    char     market_indicator[ConstField::kMarketIndicatorLen];              // 市场标识(2-利率互换市场)
    int32_t  md_book_type;                                                   // 订阅行情类型(101-逐笔成交行)
    int32_t  trade_method;                                                   // 交易方式(3-匿名点击)
    char     real_time_under_take_flag;                                      // 实时承接标识('1'-是 '0'-否)
    char     transaction_method[ConstField::kTransactionMethodLen];          // 成交方向(取值:TKN GVN TRD DONE)  
    char     bridge_deal_indic;                                              // 是否桥成交(Y-是 N-否)
    char     split_indicator;                                                // 是否拆分(Y-是 N-否)
    int64_t  exec_price;                                                     // 价格(类型:价格Price)
    int64_t  exec_volume;                                                    // 成交量(类型:数量CMDS-Qty)
    uint8_t  variety_category;                                               // 品种类别
};
/**  @} */

//RDI
//信用评级
struct BondInfoCreditItem
{
    char    credit_rating_agency[ConstField::kCreditRatingAgencyLen];           // 信用评级机构
    char    credit_rating[ConstField::kCrdRtgLen];                              // 信用评级
    int64_t credit_validity_start_date;                                         // 信用等级有效起始日(YYYYMMDD)
    int64_t credit_validity_end_date;                                           // 信用等级有效终止日(YYYYMMDD)
};

//可转债
struct BondInfoConvertItem
{
    int64_t convert_date;                                                       // 转换日期(YYYYMMDD)
    int32_t convert_type;                                                       // 转换类型(0-部分 1-全部)
    int64_t convert_ratio;                                                      // 转换比例(类型:比例Rate)
    int64_t convert_price;                                                      // 转换价格(单位:元;类型:价格Price)
};

//再发行信息
struct BondInfoReissueItem
{
    int64_t reissue_date;                                                       // 增发发行日(YYYYMMDD)
    int64_t reissue_event_date;                                                 // 增发上市日(YYYYMMDD)
    int64_t reissue_volume;                                                     // 增发量(单位:亿;类型:数量RDI-Qty)
};

//行权
struct BondInfoExerciseItem
{
    int32_t exercise_type;                                                      // 行权类型 (2-Call表示可提前兑付 1-Put表示可提前赎回)
    int64_t exercise_date;                                                      // 行权日 (YYYYMMDD)
    int64_t exercise_price;                                                     // 行权价格(单位:元;类型:价格Price)
    int64_t exercise_volume;                                                    // 行权量(单位:亿;类型:数量RDI-Qty)
    int64_t exercise_spread;                                                    // 行权后利率/基准利差(单位:%)(类型:比例Rate)
};

/**
 * @name MDBondInfoInterbank 债券信息(银行间)数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDBondInfoInterbank
{
    char    security_type[ConstField::kDescLen];                                // 债券类型
    char    symbol[ConstField::kSymLen];                                        // 债券名称
    char    security_code[ConstField::kRDISecurityCodeLen];                     // 债券代码
    char    security_type_code[ConstField::kSecuritTypeCodeLen];                // 债券类型代码
    char    standard_compen_period[ConstField::kTermToMaturityStringLen];       // 标准待偿期
    char    issuer_code[ConstField::kIssuerCodeLen];                            // 发行人代码
    int64_t issue_size;                                                         // 发行量(单位:亿;类型:数量RDI-Qty)
    int64_t issue_price;                                                        // 发行价格(单位:元;类型:价格Price)
    int64_t face_value;                                                         // 面值(单位:元;类型:金额Amt)
    int64_t issue_date;                                                         // 发行日(YYYYMMDD)
    int64_t circulation_volume;                                                 // 流通量(单位:亿;类型:数量RDI-Qty)
    int64_t expiry_date;                                                        // 到期日(YYYYMMDD)
    char    bond_maturity[ConstField::kTermToMaturityStringLen];                // 债券期限(YYYYMMDD)
    char    compen_period[ConstField::kTermToMaturityLen];                      // 待偿期
    int64_t acrd_int_amt;                                                       // 每百元应计利息(类型:利息Interest)
    int64_t list_date;                                                          // 上市交易日(YYYYMMDD)
    int64_t delist_date;                                                        // 摘牌日期(YYYYMMDD)
    char    status;                                                             // 状态(1-活动)
    char    special_debt;                                                       // 特殊债(Y-是 N-否)
    char    default_bond_indicator;                                             // 到期违约债(Y-是 N-否)
    int64_t eventdate_start_date;                                               // 再次流通起始日(YYYYMMDD)
    int64_t eventdate_end_date;                                                 // 再次流通终止日(YYYYMMDD)
    uint32_t credit_item_cnt;                                                   // 信用评级信息重复组长度
    BondInfoCreditItem* credit_items;                                           // 信用评级信息(重复组)
    int32_t	coupon_rate_type;													// 息票类型(1-浮动利率 0-固定利率 2-零息利率 3-贴现)        
    int32_t	coupon_payment_frequency;                                           // 付息频率(3-月 4-季 0-半年 1-年 2-到期)         
    int32_t	interest_accrual_method;                                            // 计算方法(2-频率 3-天数 4-无)            
    int32_t	interest_basis;                                                     // 计息基准(日计数基准)(3-实际/365 0-实际/实际 1-实际/360 5-实际/365F 2-30/360)
    int64_t	settle_date;                                                        // 起息日(YYYYMMDD)            
    int64_t	first_interest_payment_date;                                        // 首次付息日(YYYYMMDD)
    int64_t fixed_rate;                                                         // 固定利率(单位:%)(类型:比例Rate)
    char	base_rate_name[ConstField::kBenchmarkCurveNameLen];					// 基准利率名	                        
    int64_t	base_rate_value;                                                    // 基准利率值(单位:%)(类型:比例Rate)      
    int64_t	base_rate_spread;                                                   // 基准利差(基点)(单位:%)(类型:比例Rate)
    int64_t	interest_cap_rate;                                                  // 利率上限(类型:比例Rate)              
    int64_t	interest_floor_rate;                                                // 利率下限(类型:比例Rate)              
    int64_t	current_coupon_rate;                                                // 当前票面利率(类型:比例Rate)          
    char	issuer_agency_symbol[ConstField::kIssrLen];                         // 发行机构简称-中文
    uint32_t convert_item_cnt;                                                  // 可转债信息重复组长度
    BondInfoConvertItem* convert_items;                                         // 可转债信息重复组
    char reissue_indicator;                                                     // 增发标识(Y-是 N-否)
    uint32_t reissue_item_cnt;                                                  // 增发信息重复组长度
    BondInfoReissueItem* reissue_items;                                         // 增发信息重复组
    uint32_t exercise_item_cnt;                                                 // 行权信息重复组长度
    BondInfoExerciseItem* exercise_items;                                       // 行权信息重复组
    char    custodian_agency[ConstField::kPartyIDLen];                          // 托管机构
    char    net_settl_indicator;                                                // 是否可适用于净额清算(Y-是 N-否)
    char    currency[ConstField::kCcyLen];                                      // 币种(CNY:人民币 USD:美元)
    int32_t circulation_range;                                                  // 流通范围(0-定向 1-公开)

    AMA_EXPORT MDBondInfoInterbank();
    AMA_EXPORT ~MDBondInfoInterbank();
};
/**  @} */

/**
 * @name MDABSInfo 资产支持证券信息(ABS)数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDABSInfo
{
    char    symbol[ConstField::kSymLen];                                        // 资产支持证券名称	               
    char    security_code[ConstField::kRDISecurityCodeLen];                     // 资产支持证券代码	               
    char    issuer_code[ConstField::kIssuerCodeLen];                            // 发起人代码	                     
    char    security_type[ConstField::kDescLen];                                // 债券类型	                       
    char    security_type_code[ConstField::kSecuritTypeCodeLen];                // 债券类型代码	                   
    char    standard_compen_period[ConstField::kTermToMaturityStringLen];       // 标准待偿期	                     
    char    currency[ConstField::kCcyLen];                                      // 币种(CNY:人民币 USD:美元)                      
    int64_t issue_volume;                                                       // 发行总量(单位:亿;类型:数量RDI-Qty)      
    int64_t circulation_volume;                                                 // 流通总量(单位:亿;类型:数量RDI-Qty)      
    int64_t principal_price;                                                    // 每百元本金额(单位:元;类型:价格Price)
    int32_t interest_basis;                                                     // 计息基础(3-实际/365 0-实际/实际 1-实际/360 5-实际/365F 2-30/360)                       
    int64_t issue_date;                                                         // 发行日(YYYYMMDD)	                         
    int64_t list_date;                                                          // 上市交易日(YYYYMMDD)	                     
    int64_t delist_date;                                                        // 摘牌日(YYYYMMDD)	                         
    int64_t expected_expiry_date;                                               // 预期到期日(YYYYMMDD)	                     
    int64_t actual_expiry_date;                                                 // 实际到期日(YYYYMMDD)	                     
    int32_t coupon_rate_type;                                                   // 利率类型(0-固定利率 1-浮动利率 2-零息利率 3-贴现)                      
    char	base_rate_name[ConstField::kBenchmarkCurveNameLen];                 // 基准利率(单位:%)	                 
    int64_t base_rate_spread;                                                   // 基本利差(单位:%)(类型:比例Rate)     
    int32_t principal_pay_frequency;                                            // 本金支付频率(0-半年 1-年 3-月 4-季 2-到期 5-两周 6-周 7-天)                 
    int64_t next_deposit_ezright_date;                                          // 下一本金除权日(YYYYMMDD)                 
    int32_t coupon_payment_frequency;                                           // 付息频率(3-月 4-季 0-半年 1-年 2-到期)                   
    int64_t next_payment_date;                                                  // 下一付本日(YYYYMMDD)	                     
    int64_t current_interest_rate;                                              // 当前利率(单位:%)(类型:比例Rate)     
    int64_t issue_price;                                                        // 发行价(单位:元;类型:面额Par)         
    char    status;                                                             // 状态(G-非活动 1-活动)                          
    char    special_debt;                                                       // 特殊债(Y-是 N-否)
    char    issuer_name[ConstField::kIssrLen];                                  // 发起人
    char    issuer_agency[ConstField::kIssrLen];                                // 发行机构
    char    custodian_agency[ConstField::kIssrLen];                             // 托管机构
};
/**  @} */

/**
 * @name MDABSHistoryInfo 资产支持证券历史信息(ABS历史)数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDABSHistoryInfo
{
    char	symbol[ConstField::kSymLen];                    	                // 资产支持证券名称	                                                     
    char	security_code[ConstField::kRDISecurityCodeLen];                     // 资产支持证券代码	                                                
    int64_t	update_date;                                                        // 修改日(YYYYMMDD)                                                          
    char	issuer_name[ConstField::kIssrLen];                                  // 发行人	                                                          
    int64_t	principal_price;                                                    // 每百元本金额(单位:元;类型:价格Price)    
    int64_t	circulation_volume; 												// 流通总量(单位:亿;类型:数量RDI-Qty) 
};
/**  @} */

/**
 * @name MDABSCreditRatings 资产支持证券信用评级数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDABSCreditRatings
{
    char	symbol[ConstField::kSymLen];									    // 资产支持证券名称          
    char	security_code[ConstField::kRDISecurityCodeLen];                     // 资产支持证券代码                
    char	credit_rating_agency[ConstField::kCreditRatingAgencyLen];           // 信用评级机构
    char	credit_rating[ConstField::kCrdRtgLen];                              // 信用评级
    int64_t	credit_validity_start_date;                                         // 起始日(YYYYMMDD) 
    int64_t	credit_validity_end_date;                                           // 终止日(YYYYMMDD) 
};
/**  @} */

/**
 * @name MDPreIssuedBondInfo 预发行债券信息数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDPreIssuedBondInfo
{
    char	security_type[ConstField::kDescLen];								// 债券类型                     
    char	issue_type;                                                         // 发行类型(N-新发 S-续发)                   
    char	security_code[ConstField::kRDISecurityCodeLen];                     // 债券代码                     
    char	symbol[ConstField::kSymLen];                                        // 债券名称                     
    int64_t	planned_circulation_volume;                                         // 计划发行量(单位:亿;类型:数量RDI-Qty)            
    int64_t	actual_circulation_volume;                                          // 实际发行量(单位:亿;类型:数量RDI-Qty)               
    int64_t	face_value;                                                         // 面值(单位:元;类型:金额Amt)                  
    int64_t	starting_price;                                                     // 首发价格(单位:元;类型:价格Price)
    int64_t tender_date;                                                        // 招标日(YYYYMMDD)            
    int64_t	issue_price;                                                        // 发行价格(单位:元;类型:价格Price)                
    int64_t	settle_date;                                                        // 起息日(YYYYMMDD)
    int64_t	payment_date;                                                       // 缴款日(YYYYMMDD)                      
    int64_t	list_date;                                                          // 上市流通日(YYYYMMDD)                  
    int64_t	expiry_date;                                                        // 到期日(YYYYMMDD)                       
    int64_t	prerelease_start_date;                                              // 预发行开始日(YYYYMMDD)                 
    int64_t	prerelease_end_date;                                                // 预发行结束日(YYYYMMDD)                 
    int64_t	total_net_sell_limit;                                               // 净卖出总限额(单位:%;类型:金额Amt)             
    int64_t	bond_maturity;                                                      // 债券期限(YYYYMMDD)
    char	issuer_agency[ConstField::kIssrLen];                                // 发行机构                     
    char	custodian_agency[ConstField::kIssrLen];                             // 托管机构
    char	status;                                                             // 状态(1-活动 2-禁用)                   
    char	remark_txt[ConstField::kIssrLen];                                   // 备注                       
    int32_t	coupon_rate_type;                                                   // 息票信息-息票类型(1-浮动利率 0-固定利率 2-零息利率 3-贴现)           
    int32_t	coupon_payment_frequency;                                           // 息票信息-付息频率(3-月 4-季 0-半年 1-年 2-到期)           
    int32_t	coupon_accrual_method;                                              // 息票信息-计算方法(2-频率 3-天数 4-无)            
    char	coupon_interest_basis;                                              // 息票信息-计息基准(日计数基准)(3-实际/365 D-30E/3600-实际/实际1-实际/360 5-实际/365F)
    int64_t	coupon_settle_date;                                                 // 息票信息-起息日(YYYYMMDD)              
    int64_t	coupon_first_payment_date;                                          // 息票信息-首次付息日(YYYYMMDD)         
    char	coupon_base_rate_name[ConstField::kBenchmarkCurveNameLen];          // 浮动附息债-基准利率
};
/**  @} */


struct PreIPOReissueItem
{
    int64_t	reissue_date;                                                       // 增发发行日(YYYYMMDD)	             
    int64_t	reissue_event_date;                                                 // 增发上市日(YYYYMMDD)		               
    int64_t	reissue_volume;                                                     // 增发量(单位:亿;类型:数量RDI-Qty)
};

struct PreIPOExerciseItem
{
    int32_t	exercise_type;                                                      // 行权类型(1-可提前赎回 2-可提前兑付)                 
    int64_t	exercise_date;                                                      // 行权日(YYYYMMDD)	             
    int64_t	exercise_price;                                                     // 行权价格(单位:元;类型:价格Price)
    int64_t	exercise_volume;                                                    // 行权量(单位:亿;类型:数量RDI-Qty)  
};

struct PreIPOCreditItem
{
    char	credit_rating_agency[ConstField::kCreditRatingAgencyLen];                 // 信用评级机构	             
    char	credit_subject_rating[ConstField::kCrdRtgLen];                            // 主体评级	                 
    int64_t	credit_effective_date;                                                    // 生效日(YYYYMMDD)	                   
    int64_t	credit_end_date;                                                          // 终止日(YYYYMMDD)	
};


/**
 * @name MDPreIPOBondInfo 上市前债券信息(银行间)数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDPreIPOBondInfo
{
    char	bond_categories[ConstField::kSecTypLen];							// 债券大类(100-利率债 33-信用债 CD-同业存单)               
    char	security_type[ConstField::kDescLen];                                // 债券类型	                 
    char	security_code[ConstField::kRDISecurityCodeLen];                     // 债券代码	                 
    char	symbol[ConstField::kSymLen];                                        // 债券名称	                 
    int64_t	list_date;                                                          // 上市交易日(YYYYMMDD)	               
    char	status;                                                             // 债券状态(1-活动)                 
    int64_t	delist_date;                                                        // 摘牌日期(YYYYMMDD)	                 
    int64_t	issue_date;                                                         // 发行日期(YYYYMMDD)		                 
    int64_t	settle_date;                                                        // 起息日(YYYYMMDD)		                   
    int64_t	expiry_date;                                                        // 到期日(YYYYMMDD)		                   
    char	compen_period[ConstField::kTermToMaturityLen];                      // 待偿期	                   
    int64_t	issue_price;                                                        // 发行价(单位:元;类型:面额Par)   
    int64_t	face_value;                                                         // 面值(单位:元;类型:金额Amt)    
    char	issuer_agency[ConstField::kIssrLen];                                // 发行机构	                 
    int32_t	coupon_rate_type;                                                   // 息票类型(1-浮动利率 0-固定利率 2-零息利率 3-贴现)	                 
    char	base_rate_name[ConstField::kBenchmarkCurveNameLen];                 // 基准利率名称	             
    int64_t	coupon_rate;                                                        // 票面利率(类型:比例Rate)     
    char    custodian_agency[ConstField::kIDLen];                               // 托管机构
    uint32_t reissue_item_cnt;                                                  // 增发信息重复组长度
    PreIPOReissueItem* reissue_items;                                           // 增发信息重复组
    char	currency[ConstField::kCcyLen];                                      // 币种(CNY:人民币 USD:美元)
    char	lastissue_key_period_bond_flag;                                     // 是否新发关键期限债券(Y-是 N-否)    
    char	green_bond_flag;                                                    // 是否绿色债券(Y-是 N-否) 	             
    char	trad_member_group[ConstField::kIDLen];                              // 交易成员群组	             
    int32_t	circulation_range;                                                  // 流通范围(1-公开)   
    int32_t exercise_item_cnt;                                                  // 行权信息重复组长度
    PreIPOExerciseItem* exercise_items;                                         // 行权信息重复组
    int32_t credit_item_cnt;                                                    // 信用评级信息重复组长度
    PreIPOCreditItem* credit_items;                                             // 信用评级信息重复组

    AMA_EXPORT MDPreIPOBondInfo();
    AMA_EXPORT ~MDPreIPOBondInfo();       
};
/**  @} */

/**
 * @name MDXBondTradeBondInfo X-Bond 可交易债券信息数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDXBondTradeBondInfo
{
    char	centra_quote_bond_indic;											// 集中报价债券(Y-是 N-否)
    char	security_code[ConstField::kRDISecurityCodeLen];                     // 债券代码(上市前债券代码带P传输)
    char	symbol[ConstField::kSymLen];                                        // 债券名称        
    char	security_type[ConstField::kDescLen];                                // 债券类型    
    char	security_type_code[ConstField::kSecuritTypeCodeLen];                // 债券类型代码
    char	standard_compen_period[ConstField::kTermToMaturityStringLen];       // 标准待偿期 
    char    issuer_code[ConstField::kIDLen];                                    // 发行人代码                 
};
/**  @} */

struct PledgedConvertItem
{
    char    term_variety[ConstField::kUndlyLen];                                // 期限品种
    int64_t convert_rate;                                                       // 折算率(类型:比例Rate)
};

/**
 * @name MDPledgedConvertRateACInfo 质押券折算率(匿名点击)信息数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDPledgedConvertRateACInfo
{
    char     symbol[ConstField::kSymLen];                                        // 债券名称	
    char     security_code[ConstField::kRDISecurityCodeLen];                     // 债券代码
    uint32_t convert_item_cnt;                                                   // 质押券折算信息重复组长度
    PledgedConvertItem* convert_items;                                           // 质押券折算信息重复组

    AMA_EXPORT MDPledgedConvertRateACInfo();
    AMA_EXPORT ~MDPledgedConvertRateACInfo();
};
/**  @} */

struct XRepoHierGroupItem
{
    char    group_id[ConstField::kIDLen];                                      // 群组ID
    char    group_name[ConstField::kIDLen];                                    // 群组名称
};

/**
 * @name MDXRepoHierQuoteGroupInfo X-Repo 分层报价群组信息数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDXRepoHierQuoteGroupInfo
{
    char     agency_chinese_symbol[ConstField::kIDLen];                         // 机构中文简称
    char     trade_account_chinese_symbol[ConstField::kIDLen];                  // 交易账户中文简称
    char     trade_account_code[ConstField::kIDLen];                            // 交易账户6位码
    uint32_t group_item_cnt;                                                    // 群组信息重复组长度
    XRepoHierGroupItem* group_items;                                            // 群组信息重复组

    AMA_EXPORT MDXRepoHierQuoteGroupInfo();
    AMA_EXPORT ~MDXRepoHierQuoteGroupInfo();
};
/**  @} */

/**
 * @name MDXRepoContractInfo X-Repo 合约信息数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDXRepoContractInfo
{
    char	security_code[ConstField::kRDISecurityCodeLen];					// 合约名称            
    char	symbol[ConstField::kSymLen];                                    // 交易品种            
    int32_t	repo_method;                                                    // 回购方式(1-双边回购 3-通用回购)          
    int64_t	quote_per_volume;                                               // 每手报价量(千万)  
    int64_t	single_min_quote_volume;                                        // 单笔最小报价量(手)
    int64_t	single_max_quote_volume;                                        // 单笔最大报价量(手)
    char	pre_market_trade_indic;                                         // 是否参与盘前交易(Y-是 N-否)   
    char    trade_limit_days[ConstField::kTradeLimitDaysLen];               // 期限(天)
};
/**  @} */

struct SwapAppTragItem
{
    char    applicable_trdg_model[ConstField::kApplicableTrdgModelLen];           // 适用交易方式(ODM NDM QDM 可多选)
};

/**
 * @name MDSwapFixedFloatInfo 利率互换(固浮)信息数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSwapFixedFloatInfo
{
    char	security_code[ConstField::kRDISecurityCodeLen];					// 产品名称	                 
    char    trade_limit_days[ConstField::kTradeLimitDaysLen];               // 期限	                     
    int64_t	effective_date;                                                 // 生效日(YYYYMMDD)	                   
    int32_t	interest_accural_days_reset;                                    // 计息天数调整(0-不调整 1-实际天数)            
    int64_t	first_interest_settle_date;                                     // 首期起息日	               
    int32_t	payment_date_reset;                                             // 支付日调整(0-上一营业日 1-下一营业日 2-经调整的下一交营业日)
    int32_t	fixed_payment_period;                                           // 固定支付周期(2-到期 7-天 6-周 5-两周 3-月 4-季 1-年 0-半年)	             
    char	fixed_interest_basis;                                           // 固定计息基准(3-实际/365 D-30E/360 0-实际/实际 1-实际/360 5-实际/365F)	             
    char	float_reference_rate[ConstField::kBenchmarkCurveNameLen];       // 参考利率(浮动)	           
    int64_t	float_default_spread;                                           // 默认利差(单位BPs;类型:金额Amt)
    char    first_interest_determin_date[ConstField::kDataLen];             // 首次利率确定日	           
    int32_t	float_payment_cycle;                                            // 浮动支付周期(2-到期 7-天 6-周 5-两周 3-月 4-季 1-年 0-半年)             
    int32_t	float_reset_frequency;                                          // 重置频率(7-天 6-周 5-两周 3-月 4-季 1-年 0-半年)                
    int32_t	interest_accrual_method;                                        // 计息方法(0-单利 1-复利)	                 
    char	float_interest_basis[ConstField::kIDLen];                       // 浮动计息基准(3-实际/365 D-30E/360 0-实际/实际 1-实际/360 5-实际/365F)             
    uint32_t app_trag_item_cnt;                                             // 适用交易方式信息重复组长度
    SwapAppTragItem* app_trag_items;                                        // 适用交易方式信息重复组
    int64_t	single_min_quote_volume;                                        // 最小单笔报价量(手)	     
    int64_t	single_max_quote_volume;                                        // 最大单笔报价量(手)	     
    int64_t	quote_per_unit_volume;                                          // 单位报价量(单位:元)	         
    int32_t	realtime_takerover_indicator;                                   // 实时承接业务标识(1-实时承接 2-非实时承接 3-实时承接和非实时承接)         
    int64_t	unit_match_volume;                                              // 单位撮合量(单位:元)

    AMA_EXPORT MDSwapFixedFloatInfo();
    AMA_EXPORT ~MDSwapFixedFloatInfo();
};
/**  @} */

struct SwapBasisAppTragItem
{
    char    applicable_trdg_model[ConstField::kApplicableTrdgModelLen];       // 适用交易方式(ODM NDM QDM 可多选)
};

/**
 * @name MDSwapFixedFloatBasisContractInfo 利率互换(固浮)期差基差合约信息数据结构
 * 字段类型的扩大倍数参考 华锐高速行情平台AMD转码API开发指南中的 "精度说明"
 * @{ 
 * */
struct MDSwapFixedFloatBasisContractInfo
{
    char	security_code[ConstField::kRDISecurityCodeLen];						// 产品名称(YYYYMMDD)       
    int64_t	effective_date;                                                     // 生效日              
    int32_t	product_type;                                                       // 合约类别(8-期差合约 9-基差合约)           
    uint32_t app_trag_item_cnt;                                                 // 适用交易方式信息重复组长度
    SwapBasisAppTragItem* app_trag_items;                                       // 适用交易方式信息重复组
    int64_t	single_min_quote_volume;                                            // 最小单笔报价量(手)
    int64_t	single_max_quote_volume;                                            // 最大单笔报价量(手)
    int64_t	quote_per_unit_volume;                                              // 单位报价量(单位:元)    
    int32_t	realtime_takerover_indicator;                                       // 实时承接业务标识(1-实时承接 2-非实时承接 3-实时承接和非实时承接)    
    int64_t	unit_match_volume;                                                  // 单位撮合量(单位:元)    
    char	long_term_contract[ConstField::kIDLen];                             // 长端合约            
    char	short_term_contract[ConstField::kIDLen];                            // 短端合约

    AMA_EXPORT MDSwapFixedFloatBasisContractInfo();
    AMA_EXPORT ~MDSwapFixedFloatBasisContractInfo();
};
/**  @} */


struct RDIQueryItem
{
    char security_code[ConstField::kRDISecurityCodeLen];
};

struct BondInfoInterbankList
{
    uint32_t  data_cnt;
    MDBondInfoInterbank* data;
};

struct ABSInfoList
{
    uint32_t  data_cnt;
    MDABSInfo* data;
};

struct ABSHistoryInfoList
{
    uint32_t  data_cnt;
    MDABSHistoryInfo* data;
};

struct ABSCreditRatingsList
{
    uint32_t  data_cnt;
    MDABSCreditRatings* data;
};

struct PreIssuedBondInfoList
{
    uint32_t  data_cnt;
    MDPreIssuedBondInfo* data;
};

struct PreIPOBondInfoList
{
    uint32_t  data_cnt;
    MDPreIPOBondInfo* data;
};


struct XBondTradeBondInfoList
{
    uint32_t  data_cnt;
    MDXBondTradeBondInfo* data;
};

struct PledgedConvertRateACInfoList
{
    uint32_t  data_cnt;
    MDPledgedConvertRateACInfo* data;
};

struct XRepoHierQuoteGroupInfoList
{
    uint32_t  data_cnt;
    MDXRepoHierQuoteGroupInfo* data;
};

struct XRepoContractInfoList
{
    uint32_t  data_cnt;
    MDXRepoContractInfo* data;
};

struct SwapFixedFloatInfoList
{
    uint32_t  data_cnt;
    MDSwapFixedFloatInfo* data;
};

struct SwapFixedFloatBasisContractInfoList
{
    uint32_t  data_cnt;
    MDSwapFixedFloatBasisContractInfo* data;
};



};
// end of ama
};
// end of amd

#pragma pack(pop)
#endif