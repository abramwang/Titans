#ifndef TI_TRADER_STRUCT_H
#define TI_TRADER_STRUCT_H

#include "ti_common_struct.h"
#pragma pack(1)

const char TI_TradeSideType_Sell       = 'S';  // 卖出
const char TI_TradeSideType_Buy        = 'B';  // 买入
const char TI_TradeSideType_Purchase   = 'P';  // 申购
const char TI_TradeSideType_Redemption = 'R';  // 赎回
typedef char TI_TradeSideType;

const char TI_OffsetType_Open  = 'O';  // 开仓
const char TI_OffsetType_Close = 'C';  // 平仓
typedef char TI_OffsetType;

const char TI_BusinessType_Stock  = 'S';  // 股票买卖业务
const char TI_BusinessType_ETF    = 'E';  // etf申赎业务
typedef char TI_BusinessType;

const int8_t TI_OrderStatusType_fail     = -10;
const int8_t TI_OrderStatusType_removed  = -9;
const int8_t TI_OrderStatusType_dealt    = -8;
const int8_t TI_OrderStatusType_unAccept = 0;    /// 未接受
const int8_t TI_OrderStatusType_accepted = 1;    /// 已接受未受理
const int8_t TI_OrderStatusType_queued   = 2;    /// 正在排队  (已受理状态)
const int8_t TI_OrderStatusType_toRemove = 3;    /// 待报撤单
const int8_t TI_OrderStatusType_removing = 4;    /// 已报待撤
typedef int8_t TI_OrderStatusType;

/*  所有结构的定义索引
// struct TiReqOrderInsert;        //  下单请求
// struct TiRspOrderInsert;        //  下单响应
// struct TiReqOrderDelete;        //  撤单请求
// struct TiRspOrderDelete;        //  撤销订单响应
// struct TiReqQryOrder;           //  委托查询请求
// struct TiRspQryOrder;           //  委托查询响应
// struct TiReqQryMatch;           //  成交查询请求
// struct TiRspQryMatch;           //  成交查询响应
// struct TiReqQryPosition;        //  持仓查询请求
// struct TiRspQryPosition;        //  持仓查询回调
// struct TiRtnOrderStatus;        //  订单状态变化通知
// struct TiRtnOrderMatch;         //  成交回调通知
*/

/// 请求
struct TiBase_Msg
{
    int32_t         nReqId;      //  请求ID（有客户端API维护的唯一ID）
    int32_t         nUserInt;    //  用户自定义整型
    TI_Text         szUseStr;    //  用户自定义字符串
    int64_t         nUserId;     //  用户帐号Id
    TI_ClientIdType szClientId;  //  客户端编号
    TI_Text         szErr;       //  错误
};

/// 下单请求
struct TiReqOrderInsert : TiBase_Msg
{
    TI_SymbolType    szSymbol;       //  证券合约代码
    TI_ExchangeType  szExchange;     //  交易所
    TI_AccountType   szAccount;      //  资金帐号
    TI_TradeSideType nTradeSideType; //  交易类型 买、卖
    TI_OffsetType    nOffsetType;    //  交易类型 开、平
    TI_BusinessType  nBusinessType;  //  业务类型
    double           nOrderPrice;    //  下单价
    int32_t          nOrderVol;      //  下单量
    int64_t          nReqTimestamp;  //  下单本地时间
};

/// 下单回调
struct TiRtnOrderStatus : TiReqOrderInsert
{
    int64_t            nOrderId;              //  订单编号
    int32_t            nSubmitVol;            //  提交申报量
    double             nDealtPrice;           //  成交价
    int32_t            nDealtVol;             //  成交总量
    int32_t            nTotalWithDrawnVol;    //  撤单总量
    int32_t            nInValid;              //  废单数量
    TI_OrderStatusType nStatus;               //  状态
    int64_t            nInsertTimestamp;      //  下单接受时间时间
    int64_t            nLastUpdateTimestamp;  //  最后更新时间戳
    int64_t            nUsedTime;             //  花费时间 完全成交，或者撤单花费的时间
    double             nFee;                  //  手续费
    TI_OrderIdType     szOrderStreamId;       //  委托编号（broker 或交易所的唯一编号）
};

/// 撤单
struct TiReqOrderDelete : TiBase_Msg
{
    int64_t        nOrderId;         //  原始订单服务器唯一Id
    TI_OrderIdType szOrderStreamId;  //  委托编号（broker 或交易所的唯一编号）
};

/// 撤销订单响应
typedef TiReqOrderDelete TiRspOrderDelete;

/// 委托查询请求
struct TiReqQryOrder : TiBase_Msg
{
    TI_SymbolType   szSymbol;    //  证券合约代码
    TI_ExchangeType szExchange;  //  交易所
    int32_t         nIndex;      //  起始位置(不填默认从头开始)
    int32_t         nNum;        //  数量（不填默认查全部）
};

/// 成交查询请求
struct TiReqQryMatch : TiBase_Msg
{
    TI_SymbolType   szSymbol;    //  证券合约代码
    TI_ExchangeType szExchange;  //  交易所
    int32_t         nIndex;      //  起始位置(不填默认从头开始)
    int32_t         nNum;        //  数量（不填默认查全部）
};

/// 持仓查询请求
struct TiReqQryPosition : TiBase_Msg
{
    TI_SymbolType szSymbol;  //  证券合约代码
};

/// 持仓查询回调
struct TiRspQryPosition : TiBase_Msg
{
    TI_SymbolType szSymbol;        //  证券合约代码
    int32_t       nPosition;       //  持仓总量
    double        nPrice;          //  持仓均价
    double        nProfit;         //  浮盈（不含手续费）
    double        nSettledProfit;  //  已结算的盈利（不含手续费）
};

/// 成交回调通知
struct TiRtnOrderMatch : TiBase_Msg
{
    int64_t          nOrderId;           //  委托编号
    TI_OrderIdType   szStreamId;         //  成交编号（broker 或交易所的唯一编号）
    double           nMatchPrice;        //  成交价
    int32_t          nMatchVol;          //  成交量
    TI_SymbolType    szSymbol;           //  证券代码
    TI_ExchangeType  szExchange;         //  交易所
    TI_AccountType   szAccount;          //  资金帐号
    int64_t          nMatchTimestamp;    //  成交时间戳
    TI_TradeSideType nTradeSideType;     //  交易类型 买、卖
};

/// 委托查询响应
typedef TiRtnOrderStatus TiRspQryOrder;
/// 成交查询响应
typedef TiRtnOrderMatch TiRspQryMatch;

#pragma pack()
#endif