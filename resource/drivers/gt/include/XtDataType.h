/********************************************************************
    company:    北京睿智融科控股有限公司
    fileName:	XtDataType.h
    author:		xujun
    created:	8:11:2013   15:13
    purpose:	数据定义
*********************************************************************/
#ifndef XtDataType_2013_11_8_H
#define XtDataType_2013_11_8_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XtDef.h"

namespace xti
{
    ///< 错误号
    enum XT_API_EXPORT EErrorType
    {
        XT_ERROR_SUCCESS = 0,                          ///< 无错误
        XT_ERROR_NET_DISCONNECTED = 200000,            ///< 未建立连接
        XT_ERROR_NOT_LOGINED = 200001,                 ///< 未登录
        XT_ERROR_NOT_INITIALIZED = 200002,             ///< 初始化未完成
        XT_ERROR_TIME_OUT = 200003,                    ///< 超时
        XT_ERROR_NOT_FIND_ACCOUNT = 200004,            ///< 未找到账号
        XT_ERROR_NOT_FIND_FUNCTION = 200005,           ///< 未找到处理函数
        XT_ERROR_INVALID_PARAM = 200006,               ///< 参数有误
        XT_ERROR_DEFAULT = 300000,                     ///< 默认错误号
        XT_ERROR_UNKNOWN = 300001,                     ///< 未知错误
        XT_ERROR_REPEAT_LOGIN = 300002,                ///< 重复登录
        XT_ERROR_LOGIN_MD5_NOT_MATCH = 300003,         ///< md5值不匹配
        XT_ERROR_LOGIN_API_VERSINO_TOO_LOW = 300004,   ///< api版本较低
        XT_ERROR_LOGIN_USER_NOT_ALLOWEDLOGIN = 300005, ///< 用户不允许登陆
        XT_ERROR_CREATECOMMOND_PARAM,                  ///< 创建指令时，解析参数失败
        XT_ERROR_CREATECOMMOND_QUOTE,                  ///< 创建指令时，获取行情数据失败
        XT_ERROR_CREATECOMMOND_PRICE_TYPE,             ///< 创建指令时，获取有关报价类型的价格失败（如最新价等）
        XT_ERROR_ORDER_FLOW_CONTROL,                   ///< 下单超过流量控制
        XT_ERROR_ORDER_NOT_AUTHORIZE_STOCK_FUTURE,     ///< 股票期货业务没有授权
        XT_ERROR_ORDER_NOT_AUTHORIZE_STOCKOPTION,      ///< 股票期权业务没有授权
        XT_ERROR_ORDER_NOT_AUTHORIZE_CREDIT,           ///< 信用业务没有授权
        XT_ERROR_ORDER_NOT_AUTHORIZE_GOLD,             ///< 黄金业务没有授权
        XT_ERROR_ORDER_NOT_AUTHORIZE_HGT,              ///< 沪港通业务没有授权
        XT_ERROR_ORDER_NOT_AUTHORIZE_SGT,              ///< 深港通业务没有授权
        XT_ERROR_QUERYDATA_FLOW_CONTROL,               ///< 请求超过流量控制
        XT_ERROR_LOGIN_MAC_NOT_MATCH,                  ///< mac不匹配
        XT_ERROR_LOGIN_MAC_NOT_AUTHORIZE,              ///< mac未授权
        XT_ERROR_SUBSCRIBE_QUOTE,                      ///< 订阅行情时报错
        XT_ERROR_CREATECOMMOND_TRANSUNABLE,            ///< 创建指令时，合约不可交易
    };

    ///< 账号类型
    enum XT_API_EXPORT EXTBrokerType
    {
        AT_FUTURE = 1,          ///< 期货账号
        AT_STOCK = 2,           ///< 股票账号
        AT_CREDIT = 3,          ///< 信用账号
        AT_GOLD = 4,            ///< 贵金属账号
        AT_FUTURE_OPTION = 5,   ///< 期货期权账号
        AT_STOCK_OPTION = 6,    ///< 股票期权账号
        AT_HUGANGTONG = 7,      ///< 沪港通账号
        AT_INCOME_SWAP = 8,     ///< 美股收益互换账号
        AT_NEW3BOARD = 10,      ///< 全国股转账号
        AT_SHENGANGTONG = 11,   ///< 深港通账号
        AT_FICC_COMMODITY = 14, ///< 期货电子盘账号
        AT_FICC_INTEREST = 15,  ///< 利率电子盘账号
        AT_ABROAD_FUTURE = 16,  ///< 外盘期货账号
    };

    ///< 下单价格类型
    enum XT_API_EXPORT EPriceType 
    {
        PRTP_INVALID = -1, 
        PRTP_SALE5,      ///< 卖5
        PRTP_SALE4,      ///< 卖4
        PRTP_SALE3,      ///< 卖3
        PRTP_SALE2,      ///< 卖2
        PRTP_SALE1,      ///< 卖1
        PRTP_LATEST,     ///< 最新价
        PRTP_BUY1,       ///< 买1
        PRTP_BUY2,       ///< 买2
        PRTP_BUY3,       ///< 买3
        PRTP_BUY4,       ///< 买4
        PRTP_BUY5,       ///< 买5
        PRTP_FIX,        ///< 指定价
        PRTP_MARKET,     ///< 市价
        PRTP_HANG,       ///< 挂单价 跟盘价
        PRTP_COMPETE,    ///< 对手价

        ///< 期货市价
        PRTP_MARKET_BEST = 18,                  ///< 市价_最优价
        PRTP_MARKET_CANCEL,                     ///< 市价_即成剩撤
        PRTP_MARKET_CANCEL_ALL,                 ///< 市价_全额成交或撤
        PRTP_MARKET_CANCEL_1,                   ///< 市价_最优1档即成剩撤
        PRTP_MARKET_CANCEL_5,                   ///< 市价_最优5档即成剩撤
        PRTP_MARKET_CONVERT_1,                  ///< 市价_最优1档即成剩转
        PRTP_MARKET_CONVERT_5,                  ///< 市价_最优5档即成剩转

        PRTP_STK_OPTION_FIX_CANCEL_ALL = 26,    ///< 限价即时全部成交否则撤单
        ///< 上海股票期权市价
        PRTP_STK_OPTION_MARKET_CACEL_LEFT = 27, ///< 市价即时成交剩余撤单
        PRTP_STK_OPTION_MARKET_CANCEL_ALL,      ///< 市价即时全部成交否则撤单
        PRTP_STK_OPTION_MARKET_CONVERT_FIX,     ///< 市价剩余转限价

        ///< 上海股票市价
        PRTP_MARKET_SH_CONVERT_5_CANCEL = 42,   ///< 最优五档即时成交剩余撤销
        PRTP_MARKET_SH_CONVERT_5_LIMIT,         ///< 最优五档即时成交剩转限价

        ///< 深圳股票期权和深圳股票市价
        PRTP_MARKET_PEER_PRICE_FIRST,           ///< 对手方最优价格委托，可用于上海科创板市价
        PRTP_MARKET_MINE_PRICE_FIRST,           ///< 本方最优价格委托，可用于上海科创板市价
        PRTP_MARKET_SZ_INSTBUSI_RESTCANCEL,     ///< 即时成交剩余撤销委托
        PRTP_MARKET_SZ_CONVERT_5_CANCEL,        ///< 最优五档即时成交剩余撤销委托
        PRTP_MARKET_SZ_FULL_REAL_CANCEL,        ///< 全额成交或撤销委托
        PRTP_AFTER_FIX_PRICE,                   ///< 盘后定价申报

        ///< 股票期权组合保证金策略
        PRTP_OPTION_COMB_STRATEGY_CNSJC,        ///< 股票期权-认购牛市价差策略
        PRTP_OPTION_COMB_STRATEGY_PXSJC,        ///< 股票期权-认沽熊市价差策略
        PRTP_OPTION_COMB_STRATEGY_PNSJC,        ///< 股票期权-认沽牛市价差策略
        PRTP_OPTION_COMB_STRATEGY_CXSJC,        ///< 股票期权-认购熊市价差策略
        PRTP_OPTION_COMB_STRATEGY_KS,           ///< 股票期权-跨式空头
        PRTP_OPTION_COMB_STRATEGY_KKS,          ///< 股票期权-宽跨式空头
        PRTP_OPTION_COMB_STRATEGY_ZBD,          ///< 股票期权-保证金开仓转备兑开仓
        PRTP_OPTION_COMB_STRATEGY_ZXJ,          ///< 股票期权-备兑开仓转保证金开仓
        _C_PRTP_COUNT,
    };

    ///< 委托明细价格类型
    enum XT_API_EXPORT EBrokerPriceType
    {
        BROKER_PRICE_ANY = 49,                  ///< 市价
        BROKER_PRICE_LIMIT,                     ///< 限价
        BROKER_PRICE_BEST,                      ///< 最优价
        BROKER_PRICE_PROP_ALLOTMENT = 52,       ///< 配股
        BROKER_PRICE_PROP_REFER,                ///< 转托
        BROKER_PRICE_PROP_SUBSCRIBE,            ///< 申购
        BROKER_PRICE_PROP_BUYBACK,              ///< 回购
        BROKER_PRICE_PROP_PLACING,              ///< 配售
        BROKER_PRICE_PROP_DECIDE,               ///< 指定
        BROKER_PRICE_PROP_EQUITY,               ///< 转股
        BROKER_PRICE_PROP_SELLBACK,             ///< 回售
        BROKER_PRICE_PROP_DIVIDEND,             ///< 股息
        BROKER_PRICE_PROP_SHENZHEN_PLACING = 68,///< 深圳配售确认
        BROKER_PRICE_PROP_CANCEL_PLACING,       ///< 配售放弃
        BROKER_PRICE_PROP_WDZY,                 ///< 无冻质押
        BROKER_PRICE_PROP_DJZY,                 ///< 冻结质押
        BROKER_PRICE_PROP_WDJY,                 ///< 无冻解押
        BROKER_PRICE_PROP_JDJY,                 ///< 解冻解押
        BROKER_PRICE_PROP_VOTE = 75,            ///< 投票
        BROKER_PRICE_PROP_YYSGYS,               ///< 要约收购预售
        BROKER_PRICE_PROP_YSYYJC,               ///< 预售要约解除
        BROKER_PRICE_PROP_FUND_DEVIDEND,        ///< 基金设红
        BROKER_PRICE_PROP_FUND_ENTRUST,         ///< 基金申赎
        BROKER_PRICE_PROP_CROSS_MARKET,         ///< 跨市转托
        BROKER_PRICE_PROP_ETF,                  ///< ETF申购
        BROKER_PRICE_PROP_EXERCIS = 83,         ///< 权证行权
        BROKER_PRICE_PROP_PEER_PRICE_FIRST,     ///< 对手方最优价格
        BROKER_PRICE_PROP_L5_FIRST_LIMITPX,     ///< 最优五档即时成交剩余转限价
        BROKER_PRICE_PROP_MIME_PRICE_FIRST,     ///< 本方最优价格
        BROKER_PRICE_PROP_INSTBUSI_RESTCANCEL,  ///< 即时成交剩余撤销
        BROKER_PRICE_PROP_L5_FIRST_CANCEL,      ///< 最优五档即时成交剩余撤销
        BROKER_PRICE_PROP_FULL_REAL_CANCEL,     ///< 全额成交并撤单
        BROKER_PRICE_PROP_FUND_CHAIHE = 90,     ///< 基金拆合
        BROKER_PRICE_PROP_DIRECT_SECU_REPAY = 101,  ///< 直接还券
        BROKER_PRICE_PROP_TIMELY_SECU_REPAY = 103,  ///< 即时还券
        BROKER_PRICE_PROP_MARKET_BEST = 131,  ///< 市价_最优价
        BROKER_PRICE_PROP_MARKET_CANCEL = 132,  ///< 市价_即成剩撤
        BROKER_PRICE_PROP_MARKET_CANCEL_ALL = 133,  ///< 市价_全额成交或撤
        BROKER_PRICE_PROP_MARKET_CANCEL_1 = 134,  ///< 市价_最优1档即成剩撤
        BROKER_PRICE_PROP_MARKET_CANCEL_5 = 135,  ///< 市价_最优5档即成剩撤
        BROKER_PRICE_PROP_MARKET_CONVERT_1 = 136,  ///< 市价_最优1档即成剩转
        BROKER_PRICE_PROP_MARKET_CONVERT_5 = 137,  ///< 市价_最优5档即成剩转
        BROKER_PRICE_PROP_STK_OPTION_ASK = 138,  ///< 股票期权-询价
        BROKER_PRICE_PROP_STK_OPTION_FIX_CANCEL_ALL = 139,  ///< 股票期权-限价即时全部成交否则撤单
        BROKER_PRICE_PROP_STK_OPTION_MARKET_CACEL_LEFT = 140,  ///< 股票期权-市价即时成交剩余撤单
        BROKER_PRICE_PROP_STK_OPTION_MARKET_CANCEL_ALL = 141,  ///< 股票期权-市价即时全部成交否则撤单
        BROKER_PRICE_PROP_STK_OPTION_MARKET_CONVERT_FIX = 142,  ///< 股票期权-市价剩余转限价
        BROKER_PRICE_PROP_OPTION_COMB_EXERCISE = 164, ///< 股票期权-组合行权
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_CNSJC, ///< 股票期权-构建认购牛市价差策略
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_PXSJC, ///< 股票期权-构建认沽熊市价差策略
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_PNSJC, ///< 股票期权-构建认沽牛市价差策略
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_CXSJC, ///< 股票期权-构建认购熊市价差策略
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_KS, ///< 股票期权-构建跨式空头策略
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_KKS, ///< 股票期权-构建宽跨式空头策略
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_ZBD, ///< 股票期权-普通转备兑
        BROKER_PRICE_PROP_OPTION_BUILD_COMB_STRATEGY_ZXJ, ///< 股票期权-备兑转普通
        BROKER_PRICE_PROP_OPTION_RELEASE_COMB_STRATEGY_CNSJC, ///< 股票期权-解除认购牛市价差策略
        BROKER_PRICE_PROP_OPTION_RELEASE_COMB_STRATEGY_PXSJC, ///< 股票期权-解除认沽熊市价差策略
        BROKER_PRICE_PROP_OPTION_RELEASE_COMB_STRATEGY_PNSJC, ///< 股票期权-解除认沽牛市价差策略
        BROKER_PRICE_PROP_OPTION_RELEASE_COMB_STRATEGY_CXSJC, ///< 股票期权-解除认购熊市价差策略
        BROKER_PRICE_PROP_OPTION_RELEASE_COMB_STRATEGY_KS, ///< 股票期权-解除跨式空头策略
        BROKER_PRICE_PROP_OPTION_RELEASE_COMB_STRATEGY_KKS, ///< 股票期权-解除宽跨式空头策略
        _C_BRPT_COUNT,
    };

    ///< 下单量类型
    enum XT_API_EXPORT EVolumeType
    {
        VOLUME_INVALID = -1, 
        VOLUME_SALE12345,
        VOLUME_SALE1234,
        VOLUME_SALE123,
        VOLUME_SALE12,
        VOLUME_SALE1,
        VOLUME_BUY1,        ///< 买一
        VOLUME_BUY12,
        VOLUME_BUY123,
        VOLUME_BUY1234,
        VOLUME_BUY12345,
        VOLUME_FIX,
        VOLUME_LEFT,
        VOLUME_POSITION,    ///< 持仓数量
        _C_VOLUME_COUNT,
    };

    ///< 下单类型
    enum XT_API_EXPORT EOperationType
    {
        OPT_INVALID = -1, 
        ///< 期货业务
        OPT_OPEN_LONG = 0,                              ///< 开多 0
        OPT_CLOSE_LONG_HISTORY,                         ///< 平昨多 1
        OPT_CLOSE_LONG_TODAY,                           ///< 平今多 2
        OPT_OPEN_SHORT,                                 ///< 开空 3
        OPT_CLOSE_SHORT_HISTORY,                        ///< 平昨空 4
        OPT_CLOSE_SHORT_TODAY,                          ///< 平今空 5
        OPT_CLOSE_LONG_TODAY_FIRST,                     ///< 平多，优先平今  6
        OPT_CLOSE_LONG_HISTORY_FIRST,                   ///< 平多，优先平昨 7
        OPT_CLOSE_SHORT_TODAY_FIRST,                    ///< 平空，优先平今 8
        OPT_CLOSE_SHORT_HISTORY_FIRST,                  ///< 平空，优先平昨 9
        ///< 期货两键
        OPT_CLOSE_LONG_TODAY_HISTORY_THEN_OPEN_SHORT,   ///< 开空，优先平今多
        OPT_CLOSE_LONG_HISTORY_TODAY_THEN_OPEN_SHORT,   ///< 开空，优先平昨多
        OPT_CLOSE_SHORT_TODAY_HISTORY_THEN_OPEN_LONG,   ///< 开多，优先平今空
        OPT_CLOSE_SHORT_HISTORY_TODAY_THEN_OPEN_LONG,   ///< 开多，优先平昨空
        OPT_SELL_PRIORITY_OPEN,                         ///< 卖出，优先开仓
        OPT_BUY_PRIORITY_OPEN,                          ///< 买入，优先开仓
        OPT_SELL_OPTIMAL_COMSSION,                      ///< 卖出，最优手续费
        OPT_BUY_OPTIMAL_COMSSION,                       ///< 买入，最优手续费
        ///< 股票业务
        OPT_BUY = 18,                   ///< 买入，担保品买入， 18
        OPT_SELL,                       ///< 卖出，担保品卖出， 19
        ///< 信用交易
        OPT_FIN_BUY,                    ///< 融资买入
        OPT_SLO_SELL,                   ///< 融券卖出
        OPT_BUY_SECU_REPAY,             ///< 买券还券
        OPT_DIRECT_SECU_REPAY,          ///< 直接还券
        OPT_SELL_CASH_REPAY,            ///< 卖券还款
        OPT_DIRECT_CASH_REPAY,          ///< 直接还款
        ///< 分级基金
        OPT_FUND_SUBSCRIBE,             ///< 基金申购
        OPT_FUND_REDEMPTION,            ///< 基金赎回
        OPT_FUND_MERGE,                 ///< 基金合并
        OPT_FUND_SPLIT,                 ///< 基金分拆
        ///< 正回购
        OPT_PLEDGE_IN,                  ///< 质押入库
        OPT_PLEDGE_OUT,                 ///< 质押出库
        ///< 期权业务
        OPT_OPTION_BUY_OPEN,            ///< 买入开仓
        OPT_OPTION_SELL_CLOSE,          ///< 卖出平仓
        OPT_OPTION_SELL_OPEN,           ///< 卖出开仓
        OPT_OPTION_BUY_CLOSE,           ///< 买入平仓
        OPT_OPTION_COVERED_OPEN,        ///< 备兑开仓
        OPT_OPTION_COVERED_CLOSE,       ///< 备兑平仓
        OPT_OPTION_CALL_EXERCISE,       ///< 认购行权
        OPT_OPTION_PUT_EXERCISE,        ///< 认沽行权
        OPT_OPTION_SECU_LOCK,           ///< 证券锁定
        OPT_OPTION_SECU_UNLOCK,         ///< 证券解锁
        ///< 期货期权
        OPT_FUTURE_OPTION_EXERCISE = 50,///< 期货期权行权
        OPT_CONVERT_BONDS,              ///< 可转债转股
        OPT_SELL_BACK_BONDS,            ///< 可转债回售
        ///< 担保品划转
        OPT_COLLATERAL_TRANSFER_IN = 55,///< 担保品划入
        OPT_COLLATERAL_TRANSFER_OUT,    ///< 担保品划出
        ///< ETF业务
        OPT_ETF_PURCHASE = 1004,        ///< ETF申购
        OPT_ETF_REDEMPTION,             ///< ETF赎回
        ///< 科创板和创业板买卖
        OPT_AFTER_FIX_BUY = 1043,       ///< 盘后定价买入
        OPT_AFTER_FIX_SELL,             ///< 盘后定价卖出
        ///< 期权组合
        OPT_OPTION_COMB_EXERCISE = 1089,    ///< 组合行权
        OPT_OPTION_BUILD_COMB_STRATEGY,     ///< 构建组合策略
        OPT_OPTION_RELEASE_COMB_STRATEGY,   ///< 解除组合策略
        ///< 信用专项交易
        OPT_SLO_SELL_SPECIAL = 1010,        ///< 专项融券卖出
        OPT_BUY_SECU_REPAY_SPECIAL,         ///< 专项买券还券
        OPT_DIRECT_SECU_REPAY_SPECIAL,      ///< 专项直接还券
        OPT_FIN_BUY_SPECIAL = 1022,         ///< 专项融资买入
        OPT_SELL_CASH_REPAY_SPECIAL,        ///< 专项卖券还款
        OPT_DIRECT_CASH_REPAY_SPECIAL,       ///< 专项直接还款
        ///< 股票期权
        OPT_OPTION_BUY_CLOSE_THEN_OPEN = 1154, ///< 买入优先平仓
        OPT_OPTION_SELL_CLOSE_THEN_OPEN, ///< 卖出优先平仓
    };

    ///< 开平标志
    enum XT_API_EXPORT EOffsetFlagType
    {
        EOFF_THOST_FTDC_OF_INVALID = -1,        ///< 未知
        EOFF_THOST_FTDC_OF_Open = 48,           ///< 开仓，对应股票买入
        EOFF_THOST_FTDC_OF_Close = 49,          ///< 平仓，对应股票卖出
        EOFF_THOST_FTDC_OF_ForceClose = 50,     ///< 强平
        EOFF_THOST_FTDC_OF_CloseToday = 51,     ///< 平今
        EOFF_THOST_FTDC_OF_CloseYesterday = 52, ///< 平昨
        EOFF_THOST_FTDC_OF_ForceOff = 53,       ///< 强减
        EOFF_THOST_FTDC_OF_LocalForceClose = 54,///< 本地强平
        EOFF_THOST_FTDC_OF_PLEDGE_IN = 81,      ///< 质押入库
        EOFF_THOST_FTDC_OF_PLEDGE_OUT = 66,     ///< 质押出库
        EOFF_THOST_FTDC_OF_ALLOTMENT = 67,      ///< 股票配股
        EOFF_THOST_FTDC_OF_OPTION_EXERCISE = 68,///< 行权
        EOFF_THOST_FTDC_OF_OPTION_LOCK = 69,    ///< 锁定
        EOFF_THOST_FTDC_OF_OPTION_UNLOCK = 70,  ///< 解锁
    };

    ///< 投保标志
    enum XT_API_EXPORT EHedgeFlagType
    {
        HEDGE_FLAG_SPECULATION = 49,  ///< 投机
        HEDGE_FLAG_ARBITRAGE = 50,    ///< 套利
        HEDGE_FLAG_HEDGE = 51,        ///< 套保
    };

    ///< 账号状态
    enum XT_API_EXPORT EBrokerLoginStatus
    {
        BROKER_LOGIN_STATUS_INALID = -1, 
        BROKER_LOGIN_STATUS_OK = 0,         ///< 可用，初始化完成
        BROKER_LOGIN_STATUS_WAITING_LOGIN,  ///< 连接中
        BROKER_LOGIN_STATUS_LOGINING,       ///< 登录中
        BROKER_LOGIN_STATUS_FAIL,           ///< 失败
        BROKER_LOGIN_STATUS_INITING,        ///< 在初始化中
        BROKER_LOGIN_STATUS_CORRECTING,     ///< 数据刷新校正中
        BROKER_LOGIN_STATUS_CLOSED,         ///< 收盘后
    };

    ///< 买卖方向
    enum XT_API_EXPORT EEntrustBS
    {
        ENTRUST_BUY = 48,           ///< 买入
        ENTRUST_SELL = 49,          ///< 卖出
        ENTRUST_COVERED = 50,       ///< 备兑
        ENTRUST_PLEDGE_IN = 81,     ///< 质押入库
        ENTRUST_PLEDGE_OUT = 66,    ///< 质押出库
    };

    ///< 指令状态
    enum XT_API_EXPORT EOrderCommandStatus
    {
        OCS_CHECKING = -1,          ///< 风控检查中
        OCS_APPROVING,              ///< 审批中
        OCS_REJECTED,               ///< 已驳回
        OCS_RUNNING,                ///< 运行中
        OCS_CANCELING,              ///< 撤销中
        OCS_FINISHED,               ///< 已完成
        OCS_STOPPED,                ///< 已停止
        OCS_FROCE_COMPLETED,        ///< 强制撤销
        OCS_CHECKFAILED,            ///< 风控驳回
        OCS_CANCELING_APPROVING,    ///< 撤销审批中
        OCS_CANCELING_REJECTED,     ///< 撤销驳回
        OCS_PAUSE_PAUSEORDER = 14,  ///< 暂停指令并暂停任务
    };

    ///< 委托类型
    enum XT_API_EXPORT EEntrustTypes
    {
        ENTRUST_BUY_SELL = 48,      ///< 买卖
        ENTRUST_QUERY,              ///< 查询
        ENTRUST_CANCEL,             ///< 撤单
        ENTRUST_APPEND,             ///< 补单
        ENTRUST_COMFIRM,            ///< 确认
        ENTRUST_BIG,                ///< 大宗
        ENTRUST_FIN = 54,           ///< 融资委托
        ENTRUST_SLO = 55,           ///< 融券委托
        ENTRUST_CLOSE = 56,         ///< 信用平仓
        ENTRUST_CREDIT_NORMAL = 57, ///< 信用普通委托
        ENTRUST_CANCEL_OPEN = 58,   ///< 撤单补单
        ENTRUST_TYPE_OPTION_EXERCISE = 59,              ///< 行权
        ENTRUST_TYPE_OPTION_SECU_LOCK = 60,             ///< 锁定
        ENTRUST_TYPE_OPTION_SECU_UNLOCK = 61,           ///< 解锁
        ENTRUST_TYPE_OPTION_COMB_EXERCISE = 65,         ///< 组合行权
        ENTRUST_TYPE_OPTION_BUILD_COMB_STRATEGY = 66,   ///< 构建组合策略持仓
        ENTRUST_TYPE_OPTION_RELEASE_COMB_STRATEGY = 67, ///< 解除组合策略持仓
    };

    ///< 期货交易类型
    enum XT_API_EXPORT EFutureTradeType
    {
        FUTRUE_TRADE_TYPE_COMMON = 48,          ///< 普通成交
        FUTURE_TRADE_TYPE_OPTIONSEXECUTION,     ///< 期权成交ptionsExecution
        FUTURE_TRADE_TYPE_OTC,                  ///< OTC成交
        FUTURE_TRADE_TYPE_EFPDIRVED,            ///< 期转现衍生成交
        FUTURE_TRADE_TYPE_COMBINATION_DERIVED,  ///< 组合衍生成交
    };

    ///< 委托状态
    enum XT_API_EXPORT EEntrustStatus
    {
        ENTRUST_STATUS_WAIT_END = 0,        ///< 委托状态已经在ENTRUST_STATUS_CANCELED或以上，但是成交数额还不够，等成交回报来
        ENTRUST_STATUS_UNREPORTED = 48,     ///< 未报
        ENTRUST_STATUS_WAIT_REPORTING,      ///< 待报
        ENTRUST_STATUS_REPORTED,            ///< 已报
        ENTRUST_STATUS_REPORTED_CANCEL,     ///< 已报待撤
        ENTRUST_STATUS_PARTSUCC_CANCEL,     ///< 部成待撤
        ENTRUST_STATUS_PART_CANCEL,         ///< 部撤
        ENTRUST_STATUS_CANCELED,            ///< 已撤
        ENTRUST_STATUS_PART_SUCC,           ///< 部成
        ENTRUST_STATUS_SUCCEEDED,           ///< 已成
        ENTRUST_STATUS_JUNK,                ///< 废单
        ENTRUST_STATUS_ACCEPT,              ///< 已受理
        ENTRUST_STATUS_CONFIRMED,           ///< 已确认 担保品划转已确认状态
        ENTRUST_STATUS_DETERMINED = 86,     ///< 已确认 协议回购已确认状态
        ENTRUST_STATUS_PREPARE_ORDER,       ///< 预埋
        ENTRUST_STATUS_PREPARE_CANCELED,    ///< 预埋已撤
        ENTRUST_STATUS_UNKNOWN = 255,       ///< 未知
    };

    ///< 期货委托发送状态
    enum XT_API_EXPORT EEntrustSubmitStatus
    {
        ENTRUST_SUBMIT_STATUS_InsertSubmitted = 48,  ///< 已经提交
        ENTRUST_SUBMIT_STATUS_CancelSubmitted,       ///< 撤单已经提交
        ENTRUST_SUBMIT_STATUS_ModifySubmitted,       ///< 修改已经提交
        ENTRUST_SUBMIT_STATUS_OSS_Accepted,          ///< 已经接受
        ENTRUST_SUBMIT_STATUS_InsertRejected,        ///< 报单已经被拒绝
        ENTRUST_SUBMIT_STATUS_CancelRejected,        ///< 撤单已经被拒绝
        ENTRUST_SUBMIT_STATUS_ModifyRejected,        ///< 改单已经被拒绝
    };

    ///< 两融标的状态
    enum XT_API_EXPORT EXTSubjectsStatus
    {
        SUBJECTS_STATUS_NORMAL = 48,  ///< 正常
        SUBJECTS_STATUS_PAUSE = 49,   ///< 暂停
        SUBJECTS_STATUS_NOT,          ///< 非标的
    };

    ///< 两融负债类型
    enum XT_API_EXPORT EXTCompactType
    {
        COMPACT_TYPE_ALL = 32,  ///< 不限制
        COMPACT_TYPE_FIN = 48,  ///< 融资
        COMPACT_TYPE_SLO = 49,  ///< 融券
    };

    ///< 两融负债状态
    enum XT_API_EXPORT EXTCompactStatus
    {
        COMPACT_STATUS_ALL = 32,                ///< 不限制
        COMPACT_STATUS_UNDONE = 48,             ///< 未归还
        COMPACT_STATUS_PART_DONE = 49,          ///< 部分归还
        COMPACT_STATUS_DONE = 50,               ///< 已归还
        COMPACT_STATUS_DONE_BY_SELF = 51,       ///< 自行了结
        COMPACT_STATUS_DONE_BY_HAND = 52,       ///< 手工了结
        COMPACT_STATUS_NOT_DEBT = 53,           ///< 未形成负债
        COMPACT_STATUS_EXPIRY_NOT_CLOSE = 54,   ///< 到期未平仓
    };

    ///< 下单超过流控时处理方式
    enum XT_API_EXPORT EXtOverFreqOrderMode
    {
        OFQ_FORBID = 0,     ///< 禁止
        OFQ_QUEUE = 1,      ///< 队列
    };

    ///< 停牌标志
    enum XT_API_EXPORT EXtSuspendedType
    {
        XT_NO_SUSPENDED,    ///< 不停牌
        XT_SUSPENDED,       ///< 停牌
    };
    
    ///< 备兑标志
    enum XT_API_EXPORT ECoveredFlag
    {
        COVERED_FLAG_FALSE = '0',   ///< 非备兑
        COVERED_FLAG_TRUE = '1',    ///< 备兑
    };

    ///< 期权持仓类型
    enum XT_API_EXPORT ESideFlag
    {
        SIDE_FLAG_RIGHT = '0',      ///< 权利
        SIDE_FLAG_DUTY = '1',       ///< 义务
        SIDE_FLAG_COVERED = '2',    ///< 备兑
    };

    ///< 订阅行情平台类型
    enum XT_API_EXPORT EXTOfferStatus
    {
        XT_OFFER_STATUS_SP = 48,    ///< 实盘
        XT_OFFER_STATUS_MN = 49,    ///< 模拟盘
    };

    ///< 期货时间条件单类型
    enum XT_API_EXPORT ETimeCondition
    {
        TIME_CONDITION_IOC = '1',  ///< 立即完成，否则撤销
        TIME_CONDITION_GFS = '2',  ///< 本节有效
        TIME_CONDITION_GFD = '3',  ///< 当日有效
        TIME_CONDITION_GTD = '4',  ///< 指定日期前有效
        TIME_CONDITION_GTC = '5',  ///< 撤销前有效
        TIME_CONDITION_GFA = '6',  ///< 集合竞价有效
    };

    ///< 期货数量条件单类型
    enum XT_API_EXPORT EVolumeCondition
    {
        VOLUME_CONDITION_ANY = '1',  ///< 任何数量
        VOLUME_CONDITION_MIN = '2',  ///< 最小数量
        VOLUME_CONDITION_TOTAL = '3',///< 全部数量
    };

    ///< 股票期权合约类型
    enum XT_API_EXPORT EOptionType
    {
        OPTION_TYPE_CALL = 48,  ///< 认购合约
        OPTION_TYPE_PUT = 49,   ///< 认沽合约
    };

    ///< 币种
    enum XT_API_EXPORT EMoneyType
    {
        MONEY_TYPE_RMB = 48,  ///< 人民币
        MONEY_TYPE_USD = 49,  ///< 美元
        MONEY_TYPE_HK = 50,   ///< 港币
    };

    ///< 任务操作
    enum XT_API_EXPORT ETaskFlowOperation
    {
        TFO_PAUSE = 5, ///< 暂停
        TFO_RESUME = 6, ///< 恢复
    };

    ///< 涨跌停控制
    enum XT_API_EXPORT EStopTradeForOwnHiLow
    {
        STOPTRADE_NONE = 0,                 ///< 主动算法是无，智能算法是最优价尽快执行
        STOPTRADE_NO_BUY_SELL = 1,          ///< 涨停不卖跌停不买
        STOPTRADE_SCHEDULED = 2,            ///< 原定时长执行
        STOPTRADE_NOT_CHASE = 3,            ///< 涨停不买跌停不卖
    };

    ///< 两融负债头寸来源
    enum XT_API_EXPORT EXTCompactBrushSource
    {
        COMPACT_BRUSH_SOURCE_ALL = 32,  ///< 不限制
        COMPACT_BRUSH_SOURCE_NORMAL = 48,  ///< 普通头寸
        COMPACT_BRUSH_SOURCE_SPECIAL = 49,  ///< 专项头寸
    };

    ///< 触价类型
    enum XT_API_EXPORT EOpTriggerType
    {
        OTT_NONE = 0,  ///< 不使用触价
        OTT_UP = 1,  ///< 向上触价
        OTT_DOWN = 2,  ///< 向下触价
    };

    ///< 市场状态
    enum XT_API_EXPORT EXTExchangeStatus
    {
        EXCHANGE_STATUS_INVALID = 0,
        EXCHANGE_STATUS_IN_BEFORE_TRADING = 48,         ///< 开盘前
        EXCHANGE_STATUS_NOTRADING,                      ///< 非交易
        EXCHANGE_STATUS_IN_CONTINOUS,                      ///< 连续交易
        EXCHANGE_STATUS_AUCTION_ORDERING,               ///< 集合竞价报单
        EXCHANGE_STATUS_AUCTION_BALANCE,                ///< 集合竞价价格平衡
        EXCHANGE_STATUS_AUCTION_MATCH,                  ///< 集合竞价撮合
        EXCHANGE_STATUS_IN_CLOSED,                         ///< 收盘
        EXCHANGE_STATUS_ONLY_CANCEL,                    ///< 仅允许撤单
        EXCHANGE_STATUS_DAZONG_ORDERING,                ///< 大宗交易申报
        EXCHANGE_STATUS_DAZONG_INTENTION_ORDERING,      ///< 大宗交易意向申报
        EXCHANGE_STATUS_DAZONG_CONFIRM_ORDERING,        ///< 大宗交易成交申报
        EXCHANGE_STATUS_DAZONG_CLOSE_PRICE_ORDERING,    ///< 大宗交易盘后定价申报
        EXCHANGE_STATUS_ONLY_GOLD_DELIVERY,             ///< 交割申报
        EXCHANGE_STATUS_ONLY_GOLD_MIDDLE,               ///< 中立仓申报
        EXCHANGE_STATUS_AFTER_HOURS_SALE,               ///< 盘后协议买卖
        EXCHANGE_STATUS_CLOSING_AUCTION_MATCH,          ///< 收盘集合竞价
        EXCHANGE_STATUS_CLOSE_PRICE_ORDERING,           ///< 盘后定价申报
    };

    ///< 是否使用大额单边保证金算法
    enum XT_API_EXPORT EXtMaxMarginSideAlgorithmType
    {
        XT_FTDC_MMSA_NO = 48,    ///< 不使用大额单边保证金算法
        XT_FTDC_MMSA_YES,        ///< 使用大额单边保证金算法
    };

    ///< 普通算法订单类型
    enum XT_API_EXPORT EAlgoPriceType
    {
        EALGO_PRT_MARKET = 0,   ///< 市价
        EALGO_PRT_FIX = 1,      ///< 限价
    };

    ///< 银证转账方向
    enum XT_API_EXPORT ETransDirection
    {
        TRANS_DIRECTION_BANK_TO_SECURITIES = 49,    ///< 银行转证券
        TRANS_DIRECTION_SECURITIES_TO_BANK = 50,    ///< 证券转银行
        TRANS_DIRECTION_QUICK_TO_CENTER = 51,       ///< 快速转集中
        TRANS_DIRECTION_CENTER_TO_QUICK = 52,       ///< 集中转快速
        TRANS_DIRECTION_QUERY = 53,                 ///< 查询
    };

    ///< 算法下单方式
    enum XT_API_EXPORT EOrderStrategyType
    {
        E_ORDER_STRATEGY_TYPE_NORMAL = -1,  ///< 普通
        E_ORDER_STRATEGY_TYPE_APPROACH,     ///< 近场交易，需要服务支持
    };

    ///< 双中心状态
    enum XT_API_EXPORT EDualStatus
    {
        E_DUAL_STATUS_NONE = -1,    ///< 无双中心
        E_DUAL_STATUS_SH,           ///< 仅有上海
        E_DUAL_STATUS_SZ,           ///< 仅有深圳
        E_DUAL_STATUS_ALL,          ///< 上海深圳双中心均有
    };

    ///< 股份划拨信用划拨类别
    enum XT_API_EXPORT ETransTypeCreditFlag
    {
        TRANS_TRANSFER_SHARE = 0,               ///< 操作客户股份划拨，默认送0
        TRANS_TRANSFER_SPECIAL_POSITIONS = 1,   ///< 操作客户专向头寸调拨
        TRANS_TRANSFER_CREDIT_SHARE = 2,        ///< 融资融券客户股份调拨
    };

    ///< 资金股份划拨划拨方向
    enum XT_API_EXPORT ESecuFundTransDirection
    {
        SECUFUNDTRANS_TRANSFER_NORMAL_TO_FAST = 0,     ///< 从普通柜台拨入到极速柜台
        SECUFUNDTRANS_TRANSFER_FAST_TO_NORMAL = 1,     ///< 从极速柜台拨出到普通柜台
        SECUFUNDTRANS_TRANSFER_SH_TO_SZ = 2,           ///< 上海划到深圳(节点划拨)
        SECUFUNDTRANS_TRANSFER_SZ_TO_SH = 3,           ///< 深圳划到上海(节点划拨)
    };

    ///< 投组类型
    enum XT_API_EXPORT EPortfolioType
    {
        PF_TYPE_NORMAL = 0,         ///< 普通
        PF_TYPE_USER = 1,           ///< 用户默认
        PF_TYPE_ACCOUNT = 2,        ///< 账号默认
    };

    ///< 合约类型
    enum XT_API_EXPORT EProductClass
    {
        PRODECT_CLASS_NORMAL = 0,           ///< 其他合约
        PRODECT_CLASS_FUTURE = 1,           ///< 期货合约
        PRODECT_CLASS_FUTURE_OPTION = 2,    ///< 期货期权合约
    };

    ///< 外盘期货报单类型
    enum XT_API_EXPORT EAbroadDurationType
    {
        TYPE_DURATION_MKT = 48,                 ///< 市价单
        TYPE_DURATION_LMT,                      ///< 限价单
        TYPE_DURATION_AO,                       ///< 竞价单
        TYPE_DURATION_ALO,                      ///< 竞价限价单
        TYPE_DURATION_ELO,                      ///< 增强限价单
        TYPE_DURATION_SLO,                      ///< 特别限价盘单
        TYPE_DURATION_OLI,                      ///< 碎股限价单
    };

    ///< 除权除息标志
    enum XT_API_EXPORT EXtExDivdendType
    {
        XT_EXD_NORMAL,      ///< 不是除权除息
        XT_EXD_RIGHT,       ///< 除权
        XT_EXD_DIVDEND,     ///< 除息
    };

    ///< 主副标记
    enum XT_API_EXPORT EMainFlag
    {
        MAIN_FLAG_VICE = 48,  ///< 副账户
        MAIN_FLAG_MAIN = 49,  ///< 主账户
    };

    ///< 下单根据可用调整可下单量
    enum XT_API_EXPORT EAdjustOrderNum
    {
        ADJUST_FLAG_OFF = 0,   ///< 不允许
        ADJUST_FLAG_ON = 1,    ///< 允许
    };

    ///< 参与竞价(中信建投算法)
    enum XT_API_EXPORT EBidType
    {
        NOT_BID = 0,     ///< 不参与
        JOIN_BID = 1,    ///< 参与
    };

    ///< 最新价比较（中信建投算法）
    enum XT_API_EXPORT ELastPriceCompare
    {
        LESS_OR_EQUAL = 0,     ///< 小于等于
        LESS = 1,              ///< 小于
        MORE_OR_EQUAL = 2,     ///< 大于等于
        MORE = 3,              ///< 大于
    };

    ///< 申报状态（中信建投算法）
    enum EPurchaseStatus
    {
        PURCHASE_STATUS_MUST = 48,  //必须参与申报
        PURCHASE_STATUS_FORBID = 49,  //禁止参与申报
        PURCHASE_STATUS_ENABLE = 50,  //可以参与申报
    };

    ///< 股票状态
    enum EStockStatus
    {
        STOCK_STATUS_DEFAULT = 0,
        STOCK_STATUS_BEFORE_TRADING = 1, //开盘前
        STOCK_STATUS_AUCTION = 2, //集合竞价
        STOCK_STATUS_CONTINOUS= 3, //连续交易
        STOCK_STATUS_BREAK = 4, //休市
        STOCK_STATUS_CLOSED = 5, //闭市
        STOCK_STATUS_VOLATILITY_DISRUPTION = 6, //波动性中断
        STOCK_STATUS_SIDECAR = 7, //临时停牌
        STOCK_STATUS_CLOSING_BIDDING = 8, //收盘集合竞价
        STOCK_STATUS_CALL_AUCTION = 9, //盘中集合竞价
        STOCK_STATUS_HALT_TRADING = 10, //暂停交易至闭市
        STOCK_STATUS_ERROR = 11, //获取状态异常
        STOCK_STATUS_POST_TRADE = 12, //盘后固定价格交易
        STOCK_STATUS_POST_TRADE_CLOSE = 13, //盘后固定价格行情完毕
        STOCK_STATUS_CLOSE_REMOVE = 14, //盘后固定价格恢复闭市
        STOCK_STATUS_FAKED = 15, //伪造的状态
    };
}


#endif
