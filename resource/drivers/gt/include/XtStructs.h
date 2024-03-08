/********************************************************************
    company:    北京睿智融科控股有限公司
    fileName:    XtStructs.h
    author:        xujun
    created:    8:11:2013   15:14
    purpose:    结构定义
*********************************************************************/
#ifndef XtStructs_2013_11_8_H
#define XtStructs_2013_11_8_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XtDef.h"
#include "XtDataType.h"
#include "XtError.h"

namespace xti
{

    ///< 账号详细信息
    struct XT_API_EXPORT CAccountDetail
    {
        CAccountDetail();
        ~CAccountDetail();

        char        m_strAccountID[32];         ///< 资金账号
        int         m_nAccountType;             ///< 账号类型
        char        m_strStatus[32];            ///< 状态
        char        m_strTradingDate[16];       ///< 交易日

        double      m_dFrozenMargin;            ///< 外源性 股票的保证金就是冻结资金 股票不需要
        double      m_dFrozenCash;              ///< 内外源冻结保证金和手续费四个的和
        double      m_dFrozenCommission;        ///< 外源性冻结资金源
        double      m_dRisk;                    ///< 风险度
        double      m_dNav;                     ///< 单位净值
        double      m_dPreBalance;              ///< 期初权益
        double      m_dBalance;                 ///< 动态权益, 总资产
        double      m_dAvailable;               ///< 可用资金
        double      m_dCommission;              ///< 已用手续费
        double      m_dPositionProfit;          ///< 持仓盈亏
        double      m_dCloseProfit;             ///< 平仓盈亏
        double      m_dCashIn;                  ///< 出入金净值
        double      m_dCurrMargin;              ///< 当前占用保证金
        double      m_dInstrumentValue;         ///< 合约价值
        double      m_dDeposit;                 ///< 入金
        double      m_dWithdraw;                ///< 出金
        double      m_dCredit;                  ///< 信用额度
        double      m_dMortgage;                ///< 质押
        double      m_dStockValue;              ///< 股票总市值，期货没有
        double      m_dLoanValue;               ///< 债券总市值，期货没有
        double      m_dFundValue;               ///< 基金总市值，包括ETF和封闭式基金，期货没有
        double      m_dRepurchaseValue;         ///< 回购总市值，所有回购，期货没有
        double      m_dLongValue;               ///< 多单总市值，现货没有
        double      m_dShortValue;              ///< 空单总市值，现货没有
        double      m_dNetValue;                ///< 净持仓总市值，净持仓市值=多-空
        double      m_dAssureAsset;             ///< 净资产
        double      m_dTotalDebit;              ///< 总负债
        double      m_dPremiumNetExpense;       ///< 权利金净支出 用于股票期权
        double      m_dEnableMargin;            ///< 可用保证金 用于股票期权
        double      m_dFetchBalance;            ///< 可取金额
        EDualStatus m_eDualStatus;              ///< 双中心状态
        double      m_dAvailableSH;             ///< 双中心上海可用
        double      m_dAvailableSZ;             ///< 双中心深圳可用
        char        m_strAccountKey[64];        ///< 账号key
        int         m_nProductId;               ///< 迅投产品ID
        double      m_dUsedMargin;              ///< 占用保证金 用于股票期权
        double      m_dRoyalty;                 ///< 权利金支出 用于股票期权
        char        m_strProductName[64];       ///< 迅投产品名称
        double      m_dDaysProfit;              ///< 当日盈亏
        char        m_strAccountName[64];       ///< 账号名称
        char        m_strBrokerID[16];          ///< 经纪公司编号
        char        m_strBrokerName[64];        ///< 经纪公司名称

    };

    ///< 信用账号详细信息
    struct XT_API_EXPORT CCreditAccountDetail
    {
        CCreditAccountDetail();
        ~CCreditAccountDetail();

        double      m_dPerAssurescaleValue;     ///< 个人维持担保比例
        double      m_dEnableBailBalance;       ///< 可用保证金
        double      m_dUsedBailBalance;         ///< 已用保证金
        double      m_dAssureEnbuyBalance;      ///< 可买担保品资金
        double      m_dFinEnbuyBalance;         ///< 可买标的券资金
        double      m_dSloEnrepaidBalance;      ///< 可还券资金
        double      m_dFinEnrepaidBalance;      ///< 可还款资金
        double      m_dFinMaxQuota;             ///< 融资授信额度
        double      m_dFinEnableQuota;          ///< 融资可用额度
        double      m_dFinUsedQuota;            ///< 融资已用额度
        double      m_dFinUsedBail;             ///< 融资已用保证金额
        double      m_dFinCompactBalance;       ///< 融资合约金额
        double      m_dFinCompactFare;          ///< 融资合约费用
        double      m_dFinCompactInterest;      ///< 融资合约利息
        double      m_dFinMarketValue;          ///< 融资市值
        double      m_dFinIncome;               ///< 融资合约盈亏
        double      m_dSloMaxQuota;             ///< 融券授信额度
        double      m_dSloEnableQuota;          ///< 融券可用额度
        double      m_dSloUsedQuota;            ///< 融券已用额度
        double      m_dSloUsedBail;             ///< 融券已用保证金额
        double      m_dSloCompactBalance;       ///< 融券合约金额
        double      m_dSloCompactFare;          ///< 融券合约费用
        double      m_dSloCompactInterest;      ///< 融券合约利息
        double      m_dSloMarketValue;          ///< 融券市值
        double      m_dSloIncome;               ///< 融券合约盈亏
        double      m_dOtherFare;               ///< 其它费用
        double      m_dUnderlyMarketValue;      ///< 标的证券市值
        double      m_dFinEnableBalance;        ///< 可融资金额
        double      m_dDiffEnableBailBalance;   ///< 可用保证金调整值
        double      m_dBuySecuRepayFrozenMargin;        ///< 买券还券冻结资金
        double      m_dBuySecuRepayFrozenCommission;    ///< 买券还券冻结手续费
    };

    ///< 产品净值信息
    struct XT_API_EXPORT CNetValue
    {
        CNetValue();
        ~CNetValue();
        int         m_nProductId;       ///< 迅投产品ID
        int         m_nTypes;           ///< 产品类型 1-普通基金 2-分级基金
        double      m_dTotalNetValue;   ///< 产品净资产, 产品净值
        double      m_dNetValue;        ///< 母基金单位净值
        double      m_dBNetValue;       ///< B级基金单位净值
        int         m_nUpdateTime;      ///< 更新时间
    };

    /**
    @brief 指令信息

    普通单、算法单： 一个单子对应一笔委托，推送一次
    组合单： 一个单子可能对应多笔委托，分多次推送
    */
    struct XT_API_EXPORT COrderInfo
    {
        COrderInfo();
        ~COrderInfo();
        char                            m_strAccountID[256];  ///< 资金账号
        int                             m_nAccountType;      ///< 账号类型
        int                             m_nOrderID;          ///< 指令ID
        int                             m_startTime;         ///< 下达时间
        int                             m_endTime;           ///< 结束时间
        EOrderCommandStatus             m_eStatus;           ///< 状态
        double                          m_dTradedVolume;     ///< 成交量
        double                          m_dTradedPrice;      ///< 成交均价
        double                          m_dTradedAmount;     ///< 成交金额
        char                            m_strMsg[256];       ///< 指令执行信息
        char                            m_canceler[32];      ///< 撤销者
        EXTBrokerType                   m_eBrokerType;       ///< 账号类型
        char                            m_strRemark[128];    ///< 投资备注

        char                            m_strMarket[32];            ///< 市场  
        char                            m_strInstrument[32];        ///< 合约  
        char                            m_strOrderType[32];         ///< 算法名称  
        double                          m_dPrice;                   ///< 基准价  
        int                             m_nVolume;                  ///< 下单总量  
        int                             m_nValidTimeStart;          ///< 有效开始时间  
        int                             m_nValidTimeEnd;            ///< 有效结束时间  
        double                          m_dMaxPartRate;             ///< 量比比例  
        double                          m_dMinAmountPerOrder;       ///< 委托最小金额  
        EOperationType                  m_eOperationType;           ///< 下单操作：买入卖出  
        EStopTradeForOwnHiLow           m_nStopTradeForOwnHiLow;    ///< 涨跌停控制  
        char                            m_strAccountKey[1024];        ///< 账号key
    };

    ///< 委托信息
    struct XT_API_EXPORT COrderDetail
    {
        COrderDetail();
        ~COrderDetail();

        char                        m_strAccountID[32];     ///< 账号
        int                         m_nAccountType;         ///< 账号类型
        char                        m_strExchangeID[16];    ///< 交易所代码
        char                        m_strProductID[32];     ///< 合约品种
        char                        m_strInstrumentID[32];  ///< 合约代码
        double                      m_dLimitPrice;          ///< 限价单的限价，就是报价
        char                        m_strOrderSysID[32];    ///< 委托号
        int                         m_nTradedVolume;        ///< 已成交量
        int                         m_nTotalVolume;         ///< 当前总委托量
        double                      m_dFrozenMargin;        ///< 冻结保证金
        double                      m_dFrozenCommission;    ///< 冻结手续费
        double                      m_dAveragePrice;        ///< 成交均价
        double                      m_dTradeAmount;         ///< 成交额 期货=均价*量*合约乘数

        int                         m_nErrorID;             ///< 错误号
        char                        m_strErrorMsg[256];     ///< 错误描述
        char                        m_strInsertDate[16];    ///< 日期
        char                        m_strInsertTime[32];    ///< 时间
        int                         m_nOrderID;             ///< 指令ID
        EBrokerPriceType            m_nOrderPriceType;      ///< 类型，例如市价单 限价单
        EEntrustBS                  m_nDirection;           ///< 期货多空，该字段与m_eOffsetFlag一起判断期货的报单类型。股票无用
        EOffsetFlagType             m_eOffsetFlag;          ///< 期货开平，股票买卖
        EHedgeFlagType              m_eHedgeFlag;           ///< 投机 套利 套保
        EEntrustSubmitStatus        m_eOrderSubmitStatus;   ///< 提交状态，股票里面不需要报单状态
        EEntrustStatus              m_eOrderStatus;         ///< 委托状态
        EEntrustTypes               m_eEntrustType;         ///< 委托类别
        ECoveredFlag                m_eCoveredFlag;         ///< 备兑标记 '0' - 非备兑，'1' - 备兑
        char                        m_strRemark[128];       ///< 投资备注
        char                        m_strCancelInfo[256];   ///< 废单信息
        char                        m_strInstrumentName[32];///< 合约名称
        char                        m_strAccountKey[64];    ///< 账号key
        char                        m_strStrategyID[32];    ///< 收益互换策略ID
        char                        m_strSecuAccount[32];   ///< 股东号
    };

    ///< 成交信息
    struct XT_API_EXPORT CDealDetail
    {
        CDealDetail();
        ~CDealDetail();

        char                m_strAccountID[32];
        int                 m_nAccountType;             ///< 账号类型
        char                m_strExchangeID[16];        ///< 交易所代码
        char                m_strProductID[32];         ///< 合约品种
        char                m_strInstrumentID[32];      ///< 合约代码
        char                m_strTradeID[32];           ///< 成交编号
        char                m_strOrderSysID[32];        ///< 委托号
        double              m_dAveragePrice;            ///< 成交均价
        int                 m_nVolume;                  ///< 成交量 期货单位手 股票做到股
        char                m_strTradeDate[16];         ///< 成交日期
        char                m_strTradeTime[32];         ///< 成交时间
        double              m_dComssion;                ///< 手续费
        double              m_dAmount;                  ///< 成交额 期货=均价*量*合约乘数
        int                 m_nOrderID;                 ///< 指令ID
        EEntrustBS          m_nDirection;               ///< 期货多空，该字段与m_eOffsetFlag一起判断期货的报单类型。股票无用
        EOffsetFlagType     m_nOffsetFlag;              ///< 期货开平 股票买卖
        EHedgeFlagType      m_nHedgeFlag;               ///< 投机 套利 套保
        EBrokerPriceType    m_nOrderPriceType;          ///< 类型，例如市价单 限价单
        EEntrustTypes       m_eEntrustType;             ///< 委托类别
        ECoveredFlag        m_eCoveredFlag;             ///< 备兑标记 '0' - 非备兑，'1' - 备兑
        char                m_strRemark[128];           ///< 投资备注
        char                m_strInstrumentName[32];    ///< 合约名称
        char                m_strAccountKey[64];        ///< 账号key
        char                m_strStrategyID[32];        ///< 收益互换策略ID
        char                m_strSecuAccount[32];       ///< 股东号
        int                 m_nProductId;               ///< 迅投产品ID
        char                m_strProductName[64];       ///< 迅投产品名称
    };

    ///< 成交统计
    struct XT_API_EXPORT CDealStatics
    {
        CDealStatics();
        ~CDealStatics();

        char                m_strExchangeID[16];        ///< 交易所代码
        char                m_strProductID[32];         ///< 合约品种
        char                m_strInstrumentID[32];      ///< 合约代码
        double              m_dAveragePrice;            ///< 成交均价
        int                 m_nVolume;                  ///< 成交量 期货单位手 股票做到股
        double              m_dComssion;                ///< 手续费
        double              m_dAmount;                  ///< 成交额 期货=均价*量*合约乘数
        EEntrustBS          m_nDirection;               ///< 期货多空，该字段与m_eOffsetFlag一起判断期货的报单类型。股票无用
        EOffsetFlagType     m_nOffsetFlag;              ///< 期货开平 股票买卖
        EHedgeFlagType      m_nHedgeFlag;               ///< 投机 套利 套保
        char                m_strInstrumentName[32];    ///< 合约名称
        int                 m_nCount;                   ///< 相同合约相同方向条数
        int                 m_nProductId;               ///< 迅投产品ID
        char                m_strProductName[64];       ///< 迅投产品名称
    };

    ///< 持仓信息
    struct XT_API_EXPORT CPositionDetail
    {
        CPositionDetail();
        ~CPositionDetail();

        char        m_strAccountID[32];     ///< 资金账号
        char        m_strExchangeID[16];    ///< 市场代码
        char        m_strProductID[32];     ///< 合约品种
        char        m_strInstrumentID[32];  ///< 合约代码
        char        m_strInstrumentName[32];///< 合约名称
        char        m_strOpenDate[16];
        char        m_strTradeID[32];       ///< 最初开仓位的成交编号
        int         m_nVolume;              ///< 持仓量 当前拥股
        double      m_dOpenPrice;           ///< 开仓价
        char        m_strTradingDay[16];    ///< 交易日
        double      m_dMargin;              ///< 使用的保证金 历史的直接用ctp的，新的自己用成本价*存量*系数算  股票不需要
        double      m_dOpenCost;            ///< 开仓成本 等于股票的成本价*第一次建仓的量，后续减持不影响，不算手续费 股票不需要
        double      m_dSettlementPrice;     ///< 结算价 对于股票的当前价
        int         m_nCloseVolume;         ///< 平仓量 等于股票已经卖掉的 股票不需要
        double      m_dCloseAmount;         ///< 平仓额 等于股票每次卖出的量*卖出价*合约乘数（股票为1）的累加 股票不需要
        double      m_dFloatProfit;         ///< 浮动盈亏 当前量*（当前价-开仓价）*合约乘数（股票为1）
        double      m_dCloseProfit;         ///< 平仓盈亏 平仓额 - 开仓价*平仓量*合约乘数（股票为1） 股票不需要
        double      m_dMarketValue;         ///< 市值 合约价值
        double      m_dPositionCost;        ///< 持仓成本 股票不需要
        double      m_dPositionProfit;      ///< 持仓盈亏 股票不需要
        double      m_dLastSettlementPrice; ///< 最新结算价 股票不需要
        double      m_dInstrumentValue;     ///< 合约价值 股票不需要
        bool        m_bIsToday;             ///< 是否今仓
        int         m_nOrderID;             ///< 指令ID
        int         m_nFrozenVolume;        ///< 冻结数量, 期货不用这个字段
        int         m_nCanUseVolume;        ///< 股票的可用数量, 期货不用这个字段
        int         m_nOnRoadVolume;        ///< 股票的在途数量, 期货不用这个字段
        int         m_nYesterdayVolume;     ///< 股票的股份余额, 期货不用这个字段
        double      m_dLastPrice;           ///< 结算价 对于股票的当前价
        EHedgeFlagType      m_nHedgeFlag;   ///< 投机 套利 套保
        EEntrustBS          m_nDirection;   ///< 期货多空，该字段与m_eOffsetFlag一起判断期货的报单类型。股票无用
        int         m_nCoveredAmount;       ///< 备兑数量
        int         m_nAccountType;         ///< 账号类型
        double      m_dProfitRate;          ///< 持仓盈亏比例
        char        m_strAccountKey[64];    ///< 账号key
        char        m_strStrategyID[32];    ///< 收益互换策略ID
        char        m_strSecuAccount[32];   ///< 股东号
    };

    ///< 持仓统计
    struct XT_API_EXPORT CPositionStatics
    {
        CPositionStatics();
        ~CPositionStatics();

        char        m_strAccountID[32];     ///< 资金账号
        char        m_strExchangeID[16];    ///< 交易所
        char        m_strProductID[32];     ///< 合约品种
        char        m_strInstrumentID[32];  ///< 合约代码
        char        m_strInstrumentName[32];///< 合约名称
        bool        m_bIsToday;             ///< 是否今仓
        int         m_nPosition;            ///< 持仓 需要
        double      m_dOpenCost;            ///< 非任务平冻结
        double      m_dPositionCost;        ///< 持仓成本 detail的汇总
        double      m_dAveragePrice;        ///< 算法待找
        double      m_dPositionProfit;      ///< 持仓盈亏 detail的汇总
        double      m_dFloatProfit;         ///< 浮动盈亏 detail的汇总
        double      m_dOpenPrice;           ///< 开仓均价 股票不需要
        int         m_nCanCloseVolume;      ///< 可平
        double      m_dUsedMargin;          ///< 已使用保证金
        double      m_dUsedCommission;      ///< 已使用的手续费
        double      m_dFrozenMargin;        ///< 冻结保证金
        double      m_dFrozenCommission;    ///< 冻结手续费
        double      m_dInstrumentValue;     ///< 合约价值
        int         m_nOpenTimes;           ///< 开仓次数
        int         m_nOpenVolume;          ///< 总开仓量 中间平仓不减
        int         m_nCancelTimes;         ///< 撤单次数
        int         m_nFrozenVolume;        ///< 期货不用这个字段，冻结数量
        int         m_nCanUseVolume;        ///< 期货不用这个字段，股票的可用数量
        int         m_nOnRoadVolume;        ///< 期货不用这个字段，股票的在途数量
        int         m_nYesterdayVolume;     ///< 期货不用这个字段，股票的股份余额
        double      m_dSettlementPrice;     ///< 前收盘价
        double      m_dProfitRate;          ///< 持仓盈亏比例
        EEntrustBS          m_nDirection;   ///< 期货多空，该字段与m_eOffsetFlag一起判断期货的报单类型。股票无用
        EHedgeFlagType      m_nHedgeFlag;   ///< 投机 套利 套保
        int         m_nCoveredAmount;       ///< 备兑数量
        int         m_nAccountType;         ///< 账号类型
        double      m_dLastPrice;           ///< 最新价
        char        m_strStrategyID[32];    ///< 收益互换策略ID
        char        m_strAccountKey[64];    ///< 账号key
        char        m_strSecuAccount[32];   ///< 股东号
    };

    ///< 指令错误
    struct XT_API_EXPORT COrderError
    {
        COrderError();
        ~COrderError();

        char                m_strAccountID[32]; ///< 账号
        int                 m_nErrorID;         ///< 错误号
        char                m_strErrorMsg[256]; ///< 错误描述
        int                 m_nOrderID;         ///< 指令ID
        int                 m_nRequestID;       ///< 请求ID
        char                m_strRemark[128];   ///< 投资备注
        char                m_strAccountKey[64];    ///< 账号key
        EEntrustBS          m_nDirection;   ///< 期货多空，该字段与m_eOffsetFlag一起判断期货的报单类型。股票无用
        EOffsetFlagType     m_nOffsetFlag;              ///< 期货开平 股票买卖
        EHedgeFlagType      m_nHedgeFlag;   ///< 投机 套利 套保
        char                m_strExchangeID[16];    ///< 交易所代码
        char                m_strInstrumentID[32];  ///< 合约代码
        int                 m_nVolume;                  ///< 委托量
    };

    ///< 撤单错误
    struct XT_API_EXPORT CCancelError
    {
        CCancelError();
        ~CCancelError();

        char        m_strAccountID[32]; ///< 账号
        int         m_nErrorID;         ///< 错误号
        char        m_strErrorMsg[256]; ///< 错误描述
        int         m_nOrderID;         ///< 指令ID
        int         m_nRequestID;       ///< 请求ID
        char        m_strRemark[128];   ///< 投资备注
        char        m_strAccountKey[64];    ///< 账号key
    };

    ///< 行情数据
    struct XT_API_EXPORT CPriceData
    {
        CPriceData();
        ~CPriceData();

        char        m_strTradingDay[16];        ///< 交易日
        char        m_strExchangeID[16];        ///< 交易所代码
        char        m_strInstrumentID[32];      ///< 合约代码
        char        m_strInstrumentName[64];    ///< 合约名称
        char        m_strExchangeInstID[16];    ///< 合约在交易所的代码
        double      m_dLastPrice;               ///< 最新价
        double      m_dUpDown;                  ///< 涨跌
        double      m_dUpDownRate;              ///< 涨跌幅
        double      m_dAveragePrice;            ///< 当日均价
        int         m_nVolume;                  ///< 数量
        double      m_dTurnover;                ///< 成交金额
        double      m_dPreClosePrice;           ///< 昨收盘
        double      m_dPreSettlementPrice;      ///< 上次结算价
        double      m_dPreOpenInterest;         ///< 昨持仓量
        double      m_dOpenInterest;            ///< 持仓量
        double      m_dSettlementPrice;         ///< 本次结算价
        double      m_dOpenPrice;               ///< 今开盘
        double      m_dHighestPrice;            ///< 最高价
        double      m_dLowestPrice;             ///< 最低价
        double      m_dClosePrice;              ///< 今收盘
        double      m_dUpperLimitPrice;         ///< 涨停板价
        double      m_dLowerLimitPrice;         ///< 跌停板价
        double      m_dPreDelta;                ///< 昨虚实度
        double      m_dCurrDelta;               ///< 今虚实度
        char        m_strUpdateTime[32];        ///< 最后修改时间
        int         m_nUpdateMillisec;          ///< 最后修改毫秒
        double      m_dBidPrice1;               ///< 申买价一
        int         m_nBidVolume1;              ///< 申买量一
        double      m_dAskPrice1;               ///< 申卖价一
        int         m_nAskVolume1;              ///< 申卖量一
        double      m_dBidPrice2;               ///< 申买价二
        int         m_nBidVolume2;              ///< 申买量二
        double      m_dAskPrice2;               ///< 申卖价二
        int         m_nAskVolume2;              ///< 申卖量二
        double      m_dBidPrice3;               ///< 申买价三
        int         m_nBidVolume3;              ///< 申买量三
        double      m_dAskPrice3;               ///< 申卖价三
        int         m_nAskVolume3;              ///< 申卖量三
        double      m_dBidPrice4;               ///< 申买价四
        int         m_nBidVolume4;              ///< 申买量四
        double      m_dAskPrice4;               ///< 申卖价四
        int         m_nAskVolume4;              ///< 申卖量四
        double      m_dBidPrice5;               ///< 申买价五
        int         m_nBidVolume5;              ///< 申买量五
        double      m_dAskPrice5;               ///< 申卖价五
        int         m_nAskVolume5;              ///< 申卖量五
        double      m_dPrePrice;                ///< 前一次的价格
        int         m_nStockStatus;             ///< 股票状态
    };

    ///< 普通单下单参数
    struct XT_API_EXPORT COrdinaryOrder
    {
        COrdinaryOrder();
        ~COrdinaryOrder();
        char                m_strAccountID[32];            ///< 资金账户ID，如果为子账户，则为子账户ID
        double              m_dPrice;                      ///< 指定价，仅在报价方式为PRTP_FIX(指定价)时有效;
        double              m_dSuperPriceRate;             ///< 单笔超价百分比
        int                 m_nVolume;                     ///< 委托量, 直接还券的数量
        char                m_strMarket[32];               ///< 合约市场
        char                m_strInstrument[32];           ///< 委托合约.
        EPriceType          m_ePriceType;                  ///< 报价方式： 指定价，最新价 对手价……
        EOperationType      m_eOperationType;              ///< 下单类型，开、平、买、卖…
        EHedgeFlagType      m_eHedgeFlag;                  ///< 套利标志
        double              m_dOccurBalance;               ///< 直接还款的金额, 仅直接还款用
        ETimeCondition      m_eTimeCondition;              ///< 期货条件单时间条件
        EVolumeCondition    m_eVolumeCondition;            ///< 期货条件单数量条件
        char                m_strSecondInstrument[32];     ///< 期权组合委托合约
        double              m_dSecondPrice;                ///< 期权组合委托价
        ESideFlag           m_eFirstSideFlag;              ///< 第一腿合约持仓类型
        ESideFlag           m_eSecondSideFlag;             ///< 第二腿合约持仓类型
        char                m_strCombID[32];               ///< 组合持仓编号，用于解除组合策略
        char                m_strRemark[128];              ///< 投资备注
        EOpTriggerType      m_eTriggerType;                ///< 触价类型
        double              m_dTriggerPrice;               ///< 触价价格
        double              m_dSuperPrice;                 ///< 单笔超价,和m_dSuperPriceRate只用设置一个，优先使用m_dSuperPriceRate
        int                 m_nPortfolioID;                ///< 投组类型编号
        char                m_strPortfolioStrategyName[64];///< 投组策略名称，优先使用m_nPortfolioID
        char                m_strPortfolioName[64];        ///< 投组组合名称，优先使用m_nPortfolioID
        EAbroadDurationType m_eAbroadDurationType;         ///< 外盘期货报价类型
        char                m_strStrategyID[32];            ///< 收益互换策略ID
        char                m_strSecuAccount[32];           ///< 股票多股东时指定下单的股东号
        EAdjustOrderNum     m_eAdjustOrderNum;              ///< 下单根据可用调整可下单量，仅api直连pb时有效
    };

    ///< 算法单下单参数
    struct XT_API_EXPORT CAlgorithmOrder
    {
        CAlgorithmOrder();
        ~CAlgorithmOrder();
        char                m_strAccountID[32];         ///< 账号
        char                m_strMarket[32];            ///< 市场
        char                m_strInstrument[32];        ///< 合约
        double              m_dPrice;
        double              m_dSuperPriceRate;          ///< 单笔超价比率
        int                 m_nSuperPriceStart;         ///< 超价起始笔数
        int                 m_nVolume;                  ///< 量
        double              m_dSingleVolumeRate;        ///< 单比下单比率
        int                 m_nSingleNumMin;            ///< 单比下单量最小值
        int                 m_nSingleNumMax;            ///< 单比下单量最大值
        int                 m_nLastVolumeMin;           ///< 尾单最小量
        double              m_dPlaceOrderInterval;      ///< 下单间隔
        double              m_dWithdrawOrderInterval;   ///< 撤单间隔
        int                 m_nMaxOrderCount;           ///< 最大下单次数，与下单间隔相对应
        int                 m_nValidTimeStart;          ///< 有效开始时间 来自股票，待定
        int                 m_nValidTimeEnd;            ///< 有效结束时间 来自股票，待定
        EOperationType      m_eOperationType;           ///< 下单操作：开平、多空……
        EVolumeType         m_eSingleVolumeType;        ///< 单笔下单基准
        EPriceType          m_ePriceType;               ///< 报价方式：对手、最新……
        EHedgeFlagType      m_eHedgeFlag;               ///< 套利标志
        EXtOverFreqOrderMode m_eOverFreqOrderMode;      ///< 委托频率过快时的处理方式
        ETimeCondition      m_eTimeCondition;           ///< 期货条件单时间条件
        EVolumeCondition    m_eVolumeCondition;         ///< 期货条件单数量条件
        char                m_strRemark[128];           ///< 投资备注
        EOpTriggerType      m_eTriggerType;             ///< 触价类型
        double              m_dTriggerPrice;            ///< 触价价格
        double              m_dSuperPrice;              ///< 单笔超价,和m_dSuperPriceRate只用设置一个，优先使用m_dSuperPriceRate
        EAlgoPriceType      m_eAlgoPriceType;           ///< 订单类型
        int                 m_nExtraLimitType;          ///< 扩展限价类型
        double              m_dExtraLimitValue;         ///< 扩展限价
        char                m_strStrategyID[32];        ///< 收益互换策略ID
        EPriceType          m_eUndealtEntrustRule;      ///< 未成委托处理
    };

    ///< 智能算法单下单参数
    struct XT_API_EXPORT CIntelligentAlgorithmOrder
    {
        CIntelligentAlgorithmOrder();
        ~CIntelligentAlgorithmOrder();
        char                    m_strAccountID[32];             ///< 账号
        char                    m_strMarket[32];                ///< 市场
        char                    m_strInstrument[32];            ///< 合约
        char                    m_strOrderType[32];             ///< 算法名称，VWAP，TWAP，VP，PINLINE，DMA，FLOAT，MSO，SWITCH，ICEBERG，MOC，GRID，VWAPPLUS，MOO，IS，STWAP，SLOS，VPPLUS，XTFAST，SLOH，MOOPLUS，IVWAP，VWAPPLUS2
        double                  m_dPrice;                       ///< 基准价
        int                     m_nVolume;                      ///< 下单总量
        int                     m_nValidTimeStart;              ///< 有效开始时间 
        int                     m_nValidTimeEnd;                ///< 有效结束时间 
        double                  m_dMaxPartRate;                 ///< 量比比例, 用户设定, 当MaxPartRate==100%, 表示没有限制
        double                  m_dMinAmountPerOrder;           ///< 委托最小金额
        EOperationType          m_eOperationType;               ///< 下单操作：买入卖出
        EPriceType              m_ePriceType;                   ///< 报价方式：市价和指定价
        char                    m_strRemark[128];               ///< 投资备注
        double                  m_dOrderRateInOpenAcution;      ///< 开盘集合竞价参与比例(取值0-1) 仅开盘+算法有用
        int                     m_dPriceOffsetBpsForAuction;    ///< 开盘集合竞价价格偏移量(取值0-10000) 仅开盘+算法有用
        EStopTradeForOwnHiLow   m_nStopTradeForOwnHiLow;        ///< 涨跌停控制
        bool                    m_bOnlySellAmountUsed;          ///< 仅用卖出金额  0,1  换仓特有
        double                  m_dBuySellAmountDeltaPct;       ///< 买卖偏差上限0.03-1  换仓特有
        int                     m_nMaxTradeDurationAfterET;     ///< 过期后是否继续执行， 0不继续，非0继续
        EOrderStrategyType      m_eOrderStrategyType;           ///< 算法下单方式
        char                    m_strStrategyID[32];            ///< 收益互换策略ID
        EHedgeFlagType          m_eHedgeFlag;                   ///< 套利标志
        EOpTriggerType          m_eTriggerType;                 ///< 触价类型
        double                  m_dTriggerPrice;                ///< 触价价格
    };

    ///< 外部算法单下单参数
    struct XT_API_EXPORT CExternAlgorithmOrder
    {
        CExternAlgorithmOrder();
        ~CExternAlgorithmOrder();
        char                    m_strAccountID[32];         ///< 账号
        char                    m_strMarket[32];            ///< 市场
        char                    m_strInstrument[32];        ///< 合约
        char                    m_strOrderType[32];         ///< 算法名称，FTAIWAP，ALGOINTERFACE, ZEUS....
        double                  m_dPrice;                   ///< 基准价
        int                     m_nVolume;                  ///< 下单总量
        int                     m_nValidTimeStart;          ///< 有效开始时间 
        int                     m_nValidTimeEnd;            ///< 有效结束时间 
        double                  m_dMaxPartRate;             ///< 量比比例, 用户设定, 当MaxPartRate==100%, 表示没有限制
        double                  m_dMinAmountPerOrder;       ///< 委托最小金额
        EOperationType          m_eOperationType;           ///< 下单操作：买入卖出
        char                    m_strRemark[128];           ///< 投资备注
        EStopTradeForOwnHiLow   m_nStopTradeForOwnHiLow;    ///< 涨跌停控制
        EOrderStrategyType      m_eOrderStrategyType;       ///< 算法下单方式
        double                  m_dAvailable;               ///< t0策略最大可用资金
        int                     m_nMaxTradeDurationAfterET; ///< 过期后是否继续执行， 0不继续，非0继续
        char                    m_strStrategyID[32];        ///< 收益互换策略ID
    };

    ///< 中信建投算法单下单参数
    struct XT_API_EXPORT CZxjtAlgorithmOrder
    {
        CZxjtAlgorithmOrder();
        ~CZxjtAlgorithmOrder();
        char                    m_strAccountID[32];         ///< 账号
        char                    m_strInstrument[32];        ///< 合约
        char                    m_strMarket[32];            ///< 市场
        char                    m_strOrderType[32];         ///< 算法名称，TWAP,VWAP,TWAPPLUS,POV,STEP
        EPriceType              m_ePriceType;               ///< 报价方式：市价和指定价
        char                    m_strategyParams[32];       ///< 算法参数(默认为空)
        char                    m_strReportID[32];          ///< 申报号
        EMoneyType              m_eMoneyType;               ///< 币种
        int                     m_nVolume;                  ///< 下单总量
        double                  m_dPrice;                   ///< 限价
        EOperationType          m_eOperationType;           ///< 下单类型，开、平、买、卖…
        char                    TransactTime[32];           ///< 发送时间（UTC时间）
        char                    SecondaryCIOrdID[32];       ///< 信息标识
        char                    MarginTradeType[32];        ///< 两融标志
        char                    m_strAccountKey[256];       ///< 账号Key
        char                    m_strValidTime[32];         ///< 有效时间 09:30:00-15:00:00
        double                  m_dMarketPartRate;          ///< 市场参与率 0.00-100.00
        EBidType                m_strJoinOpenBid;           ///< 参与开盘竞价
        EBidType                m_strJoinCloseBid;          ///< 参与收盘竞价
        int                     m_nMaxPlateGear;            ///< 最大盘口档位  0-10
        double                  m_dRadicalnessPrice;        ///< 激进价格  0.00-10000000.00
        ELastPriceCompare       m_nLastPriceCompare;        ///< 最新价  0.00-10000000.00
        char                    m_strRemark[128];           ///< 投资备注
        char                    m_strRemark1[128];          ///< 投资备注1

    };

    ///< 组合单下单参数
    struct XT_API_EXPORT CGroupOrder
    {
        CGroupOrder();
        ~CGroupOrder();
        CAlgorithmOrder  m_orderParam;               ///< 下单配置
        char              m_strMarket[1000][32];      ///< 市场列表
        char              m_strInstrument[1000][32];  ///< 证券代码
        int               m_nVolume[1000];            ///< 每只股票的下单量
        EOperationType    m_eOperationType[1000];     ///< 每只股票的下单类型
        int               m_nOrderNum;               ///< 股票只数
        char              m_strRemark[128];          ///< 投资备注
    };

    ///< 算法组合单下单参数
    struct XT_API_EXPORT CAlgGroupOrder
    {
        CAlgGroupOrder();
        ~CAlgGroupOrder();
        CIntelligentAlgorithmOrder    m_orderParam;              ///< 下单配置
        char                          m_strMarket[1000][32];      ///< 市场列表
        char                          m_strInstrument[1000][32];  ///< 证券代码
        int                           m_nVolume[1000];            ///< 每只股票的下单量
        EOperationType                m_eOperationType[1000];     ///< 每只股票的下单类型
        int                           m_nOrderNum;               ///< 股票只数
        char                          m_strRemark[128];          ///< 投资备注
    };

    ///< 外部算法组合单下单参数
    struct XT_API_EXPORT CExternAlgGroupOrder
    {
        CExternAlgGroupOrder();
        ~CExternAlgGroupOrder();
        CExternAlgorithmOrder         m_orderParam;              ///< 下单配置
        char                          m_strMarket[1000][32];      ///< 市场列表
        char                          m_strInstrument[1000][32];  ///< 证券代码
        int                           m_nVolume[1000];            ///< 每只股票的下单量
        EOperationType                m_eOperationType[1000];     ///< 每只股票的下单类型
        int                           m_nOrderNum;               ///< 股票只数
        char                          m_strRemark[128];          ///< 投资备注
    };

    ///< 随机单下单参数
    struct XT_API_EXPORT CRandomOrder
    {
        CRandomOrder();
        ~CRandomOrder();
        char                m_strAccountID[32];     ///< 账号
        char                m_strMarket[32];        ///< 市场
        char                m_strInstrument[32];    ///< 合约
        double              m_dPrice;               ///< 基准价
        int                 m_nVolume;              ///< 下单总量
        int                 m_nValidTimeElapse;     ///< 下单间隔
        int                 m_nSingleNumMin;        ///< 单比下单量最小值
        int                 m_nSingleNumMax;        ///< 单比下单量最大值
        EOperationType      m_eOperationType;       ///< 下单操作：开平、多空……
        EPriceType          m_ePriceType;           ///< 报价方式：对手、最新……
        char                m_strRemark[128];       ///< 投资备注
    };

    ///< 普通组合单下单参数
    struct XT_API_EXPORT COrdinaryGroupOrder
    {
        COrdinaryGroupOrder();
        ~COrdinaryGroupOrder();
        char                          m_strAccountID[32];        ///< 资金账户ID，如果为子账户，则为子账户ID
        double                        m_dSuperPriceRate;         ///< 单笔超价百分比
        EPriceType                    m_ePriceType;              ///< 报价方式： 指定价，最新价 对手价……
        EHedgeFlagType                m_eHedgeFlag;              ///< 套利标志
        EXtOverFreqOrderMode          m_eOverFreqOrderMode;      ///< 委托频率过快时的处理方式
        ETimeCondition                m_eTimeCondition;          ///< 期货条件单时间条件
        EVolumeCondition              m_eVolumeCondition;        ///< 期货条件单数量条件

        char                          m_strMarket[1000][32];      ///< 市场列表
        char                          m_strInstrument[1000][32];  ///< 证券代码
        int                           m_nVolume[1000];            ///< 每只股票的下单量
        double                        m_dPrice[1000];             ///< 每只股票的下单价格
        EOperationType                m_eOperationType[1000];     ///< 每只股票的下单类型
        int                           m_nOrderNum;               ///< 股票只数

        char                          m_strRemark[128];          ///< 投资备注
        double                        m_dSuperPrice;             ///< 单笔超价,和m_dSuperPriceRate只用设置一个，优先使用m_dSuperPriceRate
        char                          m_strStrategyID[32];       ///< 收益互换策略ID
    };

    ///< 两融负债信息
    struct XT_API_EXPORT CStkCompacts
    {
        CStkCompacts();
        ~CStkCompacts();
        char            m_strAccountID[32];         ///< 账号
        char            m_strExchangeID[32];        ///< 交易所
        char            m_strInstrumentID[32];      ///< 证券代码
        int             m_nOpenDate;                ///< 合约开仓日期
        char            m_strCompactId[32];         ///< 合约编号
        double          m_dCrdtRatio;               ///< 融资融券保证金比例
        char            m_strEntrustNo[32];         ///< 委托编号
        double          m_dEntrustPrice;            ///< 委托价格
        int             m_nEntrustVol;              ///< 委托数量
        int             m_nBusinessVol;             ///< 合约开仓数量
        double          m_dBusinessBalance;         ///< 合约开仓金额
        double          m_dBusinessFare;            ///< 合约开仓费用
        EXTCompactType  m_eCompactType;             ///< 合约类型
        EXTCompactStatus    m_eCompactStatus;       ///< 合约状态
        double          m_dRealCompactBalance;      ///< 未还合约金额
        int             m_nRealCompactVol;          ///< 未还合约数量
        double          m_dRealCompactFare;         ///< 未还合约费用
        double          m_dRealCompactInterest;     ///< 未还合约利息
        double          m_dRepaidInterest;          ///< 已还利息
        int             m_nRepaidVol;               ///< 已还数量
        double          m_dRepaidBalance;           ///< 已还金额
        double          m_dCompactInterest;         ///< 合约总利息
        double          m_dUsedBailBalance;         ///< 占用保证金
        double          m_dYearRate;                ///< 合约年利率
        int             m_nRetEndDate;              ///< 归还截止日
        char            m_strDateClear[32];         ///< 了结日期
        double          m_dPrice;                   ///< 最新价
        int             m_nOpenTime;                ///< 开仓的时间，不展示，部分券商可能用的上
        int             m_nCancelVol;               ///< 合约对应的委托的撤单数量
        EBrokerPriceType  m_nOrderPriceType;        ///< 类型，例如市价单 限价单
        EOffsetFlagType   m_nOffsetFlag;            ///< 操作,开平,买卖,操作
    };

    ///< 两融标的信息
    struct XT_API_EXPORT CStkSubjects
    {
        CStkSubjects();
        ~CStkSubjects();
        char                m_strAccountID[32];         ///< 账号
        char                m_strExchangeID[32];        ///< 市场
        char                m_strInstrumentID[32];      ///< 合约
        double              m_dSloRatio;                ///< 融资融券保证金比例
        EXTSubjectsStatus   m_eSloStatus;               ///< 融券状态
        int                 m_nEnableAmount;            ///< 融券可融数量
        double              m_dFinRatio;                ///< 融资保证金比例
        EXTSubjectsStatus   m_eFinStatus;               ///< 融资状态
        double              m_dAssureRatio;             ///< 担保品折算比例
        EXTSubjectsStatus   m_eAssureStatus;            ///< 是否可做担保
        double              m_dFinRate;                 ///< 融资日利率
        double              m_dSloRate;                 ///< 融券日利率
        double              m_dFinPenaltyRate;          ///< 融资日罚息利率
        double              m_dSloPenaltyRate;          ///< 融券日罚息利率
    };

    ///< 个股期权备兑持仓信息
    struct XT_API_EXPORT CCoveredStockPosition
    {
        CCoveredStockPosition();
        ~CCoveredStockPosition();

        char        m_strAccountID[32];         ///< 账号
        char        m_strExchangeID[16];        ///< 交易类别
        char        m_strExchangeName[16];      ///< 市场名字
        char        m_strInstrumentID[32];      ///< 合约代码
        char        m_strInstrumentName[64];    ///< 合约名称
        int         m_nTotalAmount;             ///< 总持仓量
        int         m_nLockAmount;              ///< 锁定量
        int         m_nUnlockAmount;            ///< 未锁定量
        int         m_nCoveredAmount;           ///< 备兑数量
    };

    ///< 产品信息
    struct XT_API_EXPORT CProductData
    {
        CProductData();
        ~CProductData();
        int         m_nProductId;                       ///<  迅投产品ID
        char        m_strProductName[64];               ///<  迅投产品名称
        char        m_strProductCode[64];               ///<  迅投产品代码
        char        m_strCreateDate[64];                ///<  迅投产品创建日期
        double      m_dTotalNetValue;                   ///<  产品净资产, 产品净值
        double      m_dBalance;                         ///<  当前资金余额（期货的动态权益和证券的可用）
        double      m_dPreBalance;                      ///<  期初资金余额（期货静态权益和证券的资金余额）
        double      m_dAvaliableFuture;                 ///<  期货帐号的可用资金之和
        double      m_dCurrMargin;                      ///<  期货账号占用保证金
        double      m_dBalancefuture;                   ///<  期货动态权益之和
        double      m_dStockValue;                      ///<  股票总市值
        double      m_dLoanValue;                       ///<  债券总市值，期货没有
        double      m_dFundValue;                       ///<  基金总市值，包括ETF和封闭式基金，期货没有
        double      m_dRepurchaseValue;                 ///<  回购总市值，所有回购，期货没有
        double      m_dTotalDebt;                       ///<  总负债

    };

    ///< 股票（合约）信息
    struct XT_API_EXPORT CInstrumentDetail
    {
        CInstrumentDetail();
        ~CInstrumentDetail();
        char        m_strInstrumentID[32];      ///< 合约代码
        char        m_strInstrumentName[32];    ///< 合约名称
        char        m_strProductID[32];         ///< 品种代码
        char        m_strExchangeID[16];        ///< 交易所代码
        char        m_strOptUndlCode[16];       ///< 期权标的代码
        char        m_strOptUndlName[32];       ///< 期权标的证券名称
        char        m_strEndDelivDate[32];      ///< 最后日期
        double      m_dPriceTick;               ///< 价格波动
        int         m_nOptUnit;                 ///< 期权合约单位
        double      m_dMarginUnit;              ///< 保证金单位
        double      m_dUpStopPrice;             ///< 涨停价
        double      m_dDownStopPrice;           ///< 跌停价
        double      m_dSettlementPrice;         ///< 前结算
        double      m_dOptExercisePrice;        ///< 期权行权价格
        int         m_nVolumeMultiple;          ///< 合约乘数
        EXtSuspendedType m_eSuspendedType;      ///< 停牌状态
        int         m_nCallOrPut;               ///< 合约种类(个股期权：0认购，1认沽)

    };

    ///< 风险试算信息
    struct XT_API_EXPORT CCheckData
    {
        CCheckData();
        ~CCheckData();
        XtError m_singleResult;       ///< 单只风险试算结果
        XtError m_allResult;          ///< 全局风险试算结果
    };

    ///< 账号主键
    struct XT_API_EXPORT CAccountKey
    {
        CAccountKey();
        ~CAccountKey();
        int                 m_nPlatformID;              ///< 经纪公司编号
        EXTBrokerType       m_eBrokerType;              ///< 经纪公司类别
        int                 m_nAccountType;             ///< 账号类型编号
        char                m_strAccountID[32];         ///< 资金账号
        char                m_strSubAccount[32];        ///< 资产单元账号
        char                m_strAccountKey[256];       ///< 资金账号对应唯一主键
    };

    ///< 订阅行情请求
    struct XT_API_EXPORT CSubscribData
    {
        CSubscribData();
        ~CSubscribData();
        int                 m_nPlatformID;               ///< 平台ID
        char                m_strExchangeID[16];         ///< 市场代码
        char                m_strInstrumentID[32];       ///< 合约代码,当其值为allCode时，订阅整个市场
        EXTOfferStatus      m_eOfferStatus;              ///< 报盘状态
    };

    ///< 投资组合可用持仓信息
    struct XT_API_EXPORT CPFPositionStatics
    {
        CPFPositionStatics();
        ~CPFPositionStatics();

        char        m_strAccountID[32];         ///< 账号
        char        m_strExchangeID[16];        ///< 市场代码
        char        m_strInstrumentID[32];      ///< 合约代码
        char        m_strInstrumentName[32];    ///< 合约名称
        int         m_nCanOpenVolume;           ///< 可买量
        int         m_nMaxRevolvingVolume;      ///< 最大可开多量
        int         m_nCanCloseVol;             ///< 可平量
        int         m_nOriginAllocVolume;       ///< 初始分配量
        int         m_nNetVolume;               ///< 配平量
        double      m_dCloseProfit;             ///< 平仓盈亏
        double      m_dRealizeProfitl;          ///< 实现盈亏
        double      m_dTradeProfit;             ///< 交易盈亏

    };

    ///< 股票期权组合策略持仓信息
    struct XT_API_EXPORT CStockOptionCombPositionDetail
    {
        CStockOptionCombPositionDetail();
        ~CStockOptionCombPositionDetail();

        char        m_strAccountID[32];         ///< 账号
        char        m_strExchangeID[16];        ///< 交易类别
        char        m_strExchangeName[16];      ///< 市场名字
        char        m_strContractAccount[32];   ///< 合约账号
        char        m_strCombID[32];            ///< 组合编号
        char        m_strCombCode[16];          ///< 组合策略编码
        char        m_strCombCodeName[32];      ///< 组合策略名称
        int         m_nVolume;                  ///< 总持仓量
        int         m_nFrozenVolume;            ///< 冻结数量
        int         m_nCanUseVolume;            ///< 可用数量
        char        m_strFirstCode[32];         ///< 合约一
        EOptionType m_eFirstCodeType;           ///< 合约一类型
        char        m_strFirstCodeName[32];     ///< 合约一名称
        ESideFlag   m_eFirstCodePosType;        ///< 合约一持仓类型
        int         m_nFirstCodeAmt;            ///< 合约一数量
        char        m_strSecondCode[32];        ///< 合约二
        EOptionType m_eSecondCodeType;          ///< 合约二类型
        char        m_strSecondCodeName[32];    ///< 合约二名称
        ESideFlag   m_eSecondCodePosType;       ///< 合约二持仓类型
        int         m_nSecondCodeAmt;           ///< 合约二数量
        double      m_dCombBailBalance;         ///< 占用保证金
    };

    ///< 汇率信息
    struct XT_API_EXPORT CReferenceRate
    {
        CReferenceRate();
        ~CReferenceRate();

        char        m_strAccountID[32];         ///< 账号
        char        m_strExchangeID[16];        ///< 市场代码
        char        m_strExchangeName[16];      ///< 市场名字
        EMoneyType  m_eMoneyType;               ///< 币种
        double      m_bBidReferenceRate;        ///< 买入参考汇率
        double      m_bAskReferenceRate;        ///< 卖出参考汇率
        double      m_bBidSettlementRate;       ///< 买入结算汇率
        double      m_bAskSettlementRate;       ///< 卖出结算汇率
        double      m_dDayBuyRiseRate;          ///< 日间买入参考汇率浮动比例
        double      m_dNightBuyRiseRate;        ///< 夜市买入参考汇率浮动比例
        double      m_dDaySaleRiseRate;         ///< 日间卖出参考汇率浮动比例
        double      m_dNightSaleRiseRate;       ///< 日间卖出参考汇率浮动比例
        double      m_bMidReferenceRate;        ///< 参考汇率中间价
    };

    ///< 信用账号信息
    struct XT_API_EXPORT CCreditDetail
    {
        CCreditDetail();
        ~CCreditDetail();

        char   m_strAccountID[32];          ///< 账号
        double m_dPerAssurescaleValue;      ///< 维持担保比例
        double m_dBalance;                  ///< 动态权益
        double m_dTotalDebt;                ///< 总负债
        double m_dAssureAsset;              ///< 净资产
        double m_dMarketValue;              ///< 合约价值
        double m_dEnableBailBalance;        ///< 可用保证金
        double m_dAvailable;                ///< 可用资金
        double m_dFinDebt;                  ///< 融资负债
        double m_dFinDealAvl;               ///< 融资本金
        double m_dFinFee;                   ///< 融资息费
        double m_dSloDebt;                  ///< 融券负债
        double m_dSloMarketValue;           ///< 融券市值
        double m_dSloFee;                   ///< 融券息费
        double m_dOtherFare;                ///< 其它费用
        double m_dFinMaxQuota;              ///< 融资授信额度
        double m_dFinEnableQuota;           ///< 融资可用额度
        double m_dFinUsedQuota;             ///< 融资冻结额度
        double m_dSloMaxQuota;              ///< 融券授信额度
        double m_dSloEnableQuota;           ///< 融券可用额度
        double m_dSloUsedQuota;             ///< 融券冻结额度
        double m_dSloSellBalance;           ///< 融券卖出资金
        double m_dUsedSloSellBalance;       ///< 已用融券卖出资金
        double m_dSurplusSloSellBalance;    ///< 剩余融券卖出资金
        double m_dStockValue;               ///< 股票总市值，期货没有
        double m_dFundValue;                ///< 基金总市值，包括ETF和封闭式基金，期货没有
        double m_dMaxQuota;                 ///< 总授信额度
    };

    ///< 指令级成交统计
    struct XT_API_EXPORT COrderStat
    {
        COrderStat();
        ~COrderStat();

        int    m_nOrderID;                  ///< 指令ID
        char   m_strExchangeID[16];         ///< 交易所代码
        char   m_strInstrumentID[32];       ///< 合约代码
        double m_dAveragePrice;             ///< 成交均价
        int    m_nTradedVolume;             ///< 已成交量
        int    m_nTradeNum;                 ///< 成交笔数
        char   m_strRemark[128];            ///< 投资备注
    };

    //任务操作
    struct XT_API_EXPORT CTaskOpRecord
    {
        CTaskOpRecord();
        ~CTaskOpRecord();

        int                m_nCmdId;         ///< 指令ID
        ETaskFlowOperation m_eOperator;      ///< 任务操作
        char               m_strReason[512]; ///< 操作原因
    };

    ///< 新股额度数据
    struct XT_API_EXPORT CSubscribeInfo
    {
        CSubscribeInfo();
        ~CSubscribeInfo();

        char   m_strAccountID[32];          ///< 账号ID
        char   m_strAccountKey[64];         ///< 账号key
        char   m_strExchangeID[16];         ///< 交易所代码
        char   m_strInstrumentID[32];       ///< 合约代码
        int    m_dPurchaseAmount;           ///<可申购额度
        int    m_dTechPurchaseAmount;       ///<科创板可申购额度
    };

    ///< 北交所新股询价额度数据
    struct XT_API_EXPORT CEnquirySubscribeInfo
    {
        CEnquirySubscribeInfo();
        ~CEnquirySubscribeInfo();

        char   m_strAccountID[32];          ///< 账号ID
        char   m_strAccountKey[64];         ///< 账号key
        char   m_strExchangeID[16];         ///< 交易所代码
        char   m_strInstrumentID[32];       ///< 合约代码
        char   m_strBizNo[32];              ///< 成交编号
        char   m_strBizDate[32];            ///< 成交日期
        int    m_nBizTime;                  ///< 成交时间
        int    m_nBizAmount;                ///< 成交数量
        int    m_nPurchaseMultiple;         ///< 申购倍数
        double m_dBizPrice;                 ///< 成交价格
        EPurchaseStatus  m_nReturnType;     ///< 返回业务类别
        int    m_nDate;                     ///< 交易日期
        char   m_strCancelInfo[256];        ///< 撤单原因
        char   m_strMatchType[256];         ///< 询价结果
    };

    ///< 未了结负债数据
    struct XT_API_EXPORT CStkUnClosedCompacts
    {
        CStkUnClosedCompacts();
        ~CStkUnClosedCompacts();

        char                   m_strAccountID[32];      ///< 账号ID
        char                   m_strAccountKey[64];     ///< 账号key
        char                   m_strExchangeID[16];     ///< 交易所代码
        char                   m_strInstrumentID[32];   ///< 合约代码
        EXTCompactType         m_eCompactType;          ///< 合约类型
        EXTCompactBrushSource  m_eCashgroupProp;        ///< 头寸来源
        int                    m_nOpenDate;             ///< 开仓日期
        int                    m_nBusinessVol;          ///< 合约证券数量
        int                    m_nRealCompactVol;       ///< 未还合约数量
        int                    m_nRetEndDate;           ///< 到期日
        double                 m_dBusinessBalance;      ///< 合约金额
        double                 m_dBusinessFare;         ///< 合约息费
        double                 m_dRealCompactBalance;   ///< 未还合约金额
        double                 m_dRealCompactFare;      ///< 未还合约息费
        double                 m_dRepaidFare;           ///< 已还息费
        double                 m_dRepaidBalance;        ///< 已还金额
        char                   m_strCompactId[32];      ///< 合约编号
        char                   m_strEntrustNo[32];      ///< 委托编号
        int                    m_nRepayPriority;        ///< 偿还优先级
        char                   m_strPositionStr[32];    ///< 定位串
    };

    ///< 已了结负债数据
    struct XT_API_EXPORT CStkClosedCompacts
    {
        CStkClosedCompacts();
        ~CStkClosedCompacts();

        char                   m_strAccountID[32];      ///< 账号ID
        char                   m_strAccountKey[64];     ///< 账号key
        char                   m_strExchangeID[16];     ///< 交易所代码
        char                   m_strInstrumentID[32];   ///< 合约代码
        EXTCompactType         m_eCompactType;          ///< 合约类型
        EXTCompactBrushSource  m_eCashgroupProp;        ///< 头寸来源
        int                    m_nOpenDate;             ///< 开仓日期
        int                    m_nBusinessVol;          ///< 合约证券数量
        int                    m_nRetEndDate;           ///< 到期日
        int                    m_nDateClear;            ///< 了结日期
        int                    m_nEntrustVol;           ///< 委托数量
        double                 m_dEntrustBalance;       ///< 委托金额
        double                 m_dBusinessBalance;      ///< 合约金额
        double                 m_dBusinessFare;         ///< 合约息费
        double                 m_dRepaidFare;           ///< 已还息费
        double                 m_dRepaidBalance;        ///< 已还金额
        char                   m_strCompactId[32];      ///< 合约编号
        char                   m_strEntrustNo[32];      ///< 委托编号
        char                   m_strPositionStr[32];    ///< 定位串
    };

    ///< 结算单信息
    struct XT_API_EXPORT CDeliveryDetail
    {
        CDeliveryDetail();
        ~CDeliveryDetail();

        char                m_strAccountID[32];             ///< 账号ID
        char                m_strAccountKey[64];            ///< 账号key
        int                 m_nAccountType;                 ///< 账号类型
        int                 m_nBizDate;                     ///< 成交日期
        int                 m_nBizTime;                     ///< 成交时间
        char                m_strExchangeID[16];            ///< 交易所代码
        char                m_strExchangeName[32];          ///< 交易所名称
        char                m_strInstrumentID[32];          ///< 合约代码
        char                m_strInstrumentName[32];        ///< 合约名称
        char                m_strEntrustBS[16];             ///< 操作
        char                m_strEntrustBSName[32];         ///< 操作名称
        double              m_dBizAmount;                   ///< 成交数量
        double              m_dBizPrice;                    ///< 成交价格
        double              m_dBizBalance;                  ///< 成交金额
        char                m_strEntrustNo[32];             ///< 委托号
        char                m_strBizNo[32];                 ///< 成交序号
        double              m_dCommission;                  ///< 手续费
        double              m_dStampTax;                    ///< 印花税

        int                 m_nEntrustDate;                 ///< 委托日期  股票期权和港股特有
        int                 m_nEntrustTime;                 ///< 委托时间  股票期权和港股特有
        char                m_strPositionStr[32];           ///< 记录序列号  股票期权和港股特有
        char                m_strMoneyType[8];              ///< 币种类型  股票期权和港股特有
        char                m_strBizName[32];               ///< 业务类型  股票期权和港股特有
        double              m_dTransFee;                    ///< 其他杂费  股票期权和股票特有
        char                m_strStockAccount[32];          ///< 股东账号  股票和港股特有
        double              m_dPostBalance;                 ///< 资金余额  股票和港股特有
        double              m_dPostAmount;                  ///< 股份余额  股票和港股特有

        char                m_strContractAccount[32];       ///< 合约账号  股票期权特有
        char                m_strMoneyName[16];             ///< 币种  股票期权特有
        char                m_strOpenFlag[8];               ///< 开平标识  股票期权特有
        char                m_strOpenFlagName[16];          ///< 开平标识名称  股票期权特有
        char                m_strCoveredFlag[8];            ///< 备兑标记  股票期权特有
        char                m_strCoveredFlagName[16];       ///< 备兑标记名称  股票期权特有
        char                m_strBizType[8];                ///< 成交类型  股票期权特有
        char                m_strRemark[128];               ///< 备注  股票期权特有

        int                 m_nClearDate;                   ///< 清算日期  港股特有
        int                 m_nBizFlag;                     ///< 业务代码  港股特有
        char                m_strCustName[64];              ///< 客户姓名  港股特有
        char                m_strOrderId[64];               ///< 合同序号  港股特有
        int                 m_nEntrustAmount;               ///< 委托数量  港股特有
        double              m_dEntrustPrice;                ///< 委托价格  港股特有
        int                 m_nbizTimes;                    ///< 成交笔数  港股特有
        char                m_strBizStatus[8];              ///< 成交状态  港股特有
        double              m_dFundEffect;                  ///< 清算金额  港股特有
        double              m_dStandardCommission;          ///< 标准手续费  港股特有
        double              m_dShareFee;                    ///< 股份交收费  港股特有
        double              m_dTradeFee;                    ///< 交易费  港股特有
        double              m_dFrontFee;                    ///< 前台费用  港股特有
        double              m_dSysUsageFee;                 ///< 交易系统使用费用  港股特有
        double              m_dLevyFee;                     ///< 交易征费  港股特有
        double              m_dSettRate;                    ///< 结算汇率  港股特有

        char                m_strDate[16];                  ///< 结算单日期  期货和期货期权特有
        char                m_StrSettlementInfo[104448];    ///< 结算单表  期货和期货期权特有

    };

    ///< 市场状态信息
    struct XT_API_EXPORT CExchangeStatus
    {
        CExchangeStatus();
        ~CExchangeStatus();

        char                m_strExchangeId[32];            ///< 交易所代码
        char                m_strProductId[32];             ///< 合约品种
        char                m_strInstrumentId[32];          ///< 合约代码
        char                m_strEnterTime[32];             ///< 进入本状态时间
        char                m_strEnterReason[128];          ///< 进入本状态原因
        EXTExchangeStatus   m_eInstrumentStatus;            ///< 市场状态
        int                 m_nExchangeTimeDelta;           ///< 交易所时间相对于服务器时间的时间差

    };

    ///< 合约信息
    struct XT_API_EXPORT CInstrumentInfo
    {
        CInstrumentInfo();
        ~CInstrumentInfo();
        char            m_strInstrumentID[32];                              ///< 合约代码
        char            m_strInstrumentName[32];                            ///< 合约名称
        char            m_strExchangeID[16];                                ///< 交易所代码
        char            m_strExchangeInstID[32];                            ///< 合约在交易所的代码
        char            m_strProductID[32];                                 ///< 品种代码
        char            m_strCreateDate[16];                                ///< 创建日
        char            m_strOpenDate[16];                                  ///< 上市日
        char            m_strExpireDate[16];                                ///< 到期日
        char            m_strStartDelivDate[16];                            ///< 开始交割日
        char            m_strEndDelivDate[16];                              ///< 结束交割日
        char            m_strOptUndlCode[16];                               ///< 期权标的代码
        char            m_strOptUndlMarket[16];                             ///< 期权标的市场
        EProductClass   m_eProductClass;                                    ///< 合约类型，仅期货和期货期权合约用
        int             m_nDeliveryYear;                                    ///< 交割年份
        int             m_nDeliveryMonth;                                   ///< 交割月
        int             m_nMaxMarketOrderVolume;                            ///< 市价单最大下单量
        int             m_nMinMarketOrderVolume;                            ///< 市价单最小下单量
        int             m_nMaxLimitOrderVolume;                             ///< 限价单最大下单量
        int             m_nMinLimitOrderVolume;                             ///< 限价单最小下单量
        int             m_nVolumeMultiple;                                  ///< 合约数量乘数
        int             m_nIsTrading;                                       ///< 当前是否交易
        double          m_dPriceTick;                                       ///< 最小变动价位
        double          m_dLongMarginRatio;                                 ///< 多头保证金率
        double          m_dShortMarginRatio;                                ///< 空头保证金率
        double          m_dUpStopPrice;                                     ///< 涨停价
        double          m_dDownStopPrice;                                   ///< 跌停价
        double          m_dSettlementPrice;                                 ///< 前结算
        EXtMaxMarginSideAlgorithmType m_eMaxMarginSideAlgorithm;            ///< 是否使用大额单边保证金算法
        EXtSuspendedType m_eSuspendedType;                                  ///< 停牌状态
        EXtExDivdendType m_eExDivdendType;                                  ///< 除权除息标志
        double          m_dOptExercisePrice;                                ///< 期权行权价
        int             m_nCallOrPut;                                       ///< 合约种类(个股期权：0认购，1认沽)
        double          m_dMarginUnit;                                      ///< 保证金单位
    };

    ///< 查询可下单量
    struct XT_API_EXPORT COpVolumeReq
    {
        COpVolumeReq();
        ~COpVolumeReq();
        char             m_strAccountID[32];            ///<  资金账户ID，如果为子账户，则为子账户ID
        char             m_strMarket[32];               ///<  合约市场
        char             m_strInstrument[32];           ///<  委托合约.
        double           m_dPrice;                      ///<  价格
        EOperationType   m_eOperationType;              ///<  下单类型，开、平、买、卖…
        EHedgeFlagType   m_eHedgeFlag;                  ///<  投机 套利 套保
    };

    ///< 两融担保品
    struct XT_API_EXPORT CCreditAssure
    {
        CCreditAssure();
        ~CCreditAssure();
        char                m_strAccountID[32];         ///< 账号
        char                m_strExchangeID[32];        ///< 市场
        char                m_strInstrumentID[32];      ///< 合约
        double              m_dAssureRatio;             ///< 担保品折算比例
        EXTSubjectsStatus   m_eAssureStatus;            ///< 是否可做担保
    };

    ///< 两融融资融券标的
    struct XT_API_EXPORT CCreditSubjects
    {
        CCreditSubjects();
        ~CCreditSubjects();
        char                m_strAccountID[32];         ///< 账号
        char                m_strExchangeID[32];        ///< 市场
        char                m_strInstrumentID[32];      ///< 合约
        double              m_dSloRatio;                ///< 融券保证金比例
        EXTSubjectsStatus   m_eSloStatus;               ///< 融券状态
        double              m_dFinRatio;                ///< 融资保证金比例
        EXTSubjectsStatus   m_eFinStatus;               ///< 融资状态
    };

    ///< 可融券数量
    struct XT_API_EXPORT CCreditSloCode
    {
        CCreditSloCode();
        ~CCreditSloCode();
        char                    m_strAccountID[32];         ///< 账号
        char                    m_strExchangeID[32];        ///< 市场
        char                    m_strInstrumentID[32];      ///< 合约
        int                     m_nEnableAmount;            ///< 融券可融数量
        EXTCompactBrushSource   m_eCashgroupProp;           ///< 头寸来源
    };

    ///< 银证转账银行信息
    struct XT_API_EXPORT CQueryBankInfo
    {
        CQueryBankInfo();
        ~CQueryBankInfo();
        char             m_strAccountID[32];            ///<  资金账户ID，如果为子账户，则为子账户ID
        char             m_strAccountKey[64];           ///<  账号key
        char             m_strBankNo[32];               ///<  银行代码
        char             m_strBankAccount[32];          ///<  银行账号
        EMoneyType       m_eMoneyType;                  ///<  币种
        char             m_strBankName[32];             ///<  银行名称，查询时可不送
    };

    ///< 银证转账银行余额信息
    struct XT_API_EXPORT CQueryBankAmount
    {
        CQueryBankAmount();
        ~CQueryBankAmount();
        char             m_strAccountID[32];            ///<  资金账户ID，如果为子账户，则为子账户ID
        char             m_strAccountKey[64];           ///<  账号key
        char             m_strBankAccount[32];          ///<  银行账号
        EMoneyType       m_eMoneyType;                  ///<  币种
        double           m_dBalance;                    ///<  银行余额
        double           m_dEnableBalance;              ///<  可转金额
    };

    ///< 银证转账
    struct XT_API_EXPORT CTransferReq
    {
        CTransferReq();
        ~CTransferReq();
        char             m_strAccountID[32];            ///<  资金账户ID，如果为子账户，则为子账户ID
        char             m_strBankNo[32];               ///<  银行代码
        char             m_strBankAccount[32];          ///<  银行账号
        char             m_strFundPwd[32];              ///<  资金密码
        EMoneyType       m_eMoneyType;                  ///<  币种
        ETransDirection  m_eTransDirection;             ///<  银证转账方向
        double           m_dOccurBalance;               ///<  转账金额
    };

    ///< 查询银证转账银行流水
    struct XT_API_EXPORT CTransferSerial
    {
        CTransferSerial();
        ~CTransferSerial();
        char                m_strAccountID[32];             ///< 资金账户ID，如果为子账户，则为子账户ID
        char                m_strAccountKey[64];            ///< 账号key
        char                m_strTransferDate[16];          ///< 转账日期
        char                m_strTransferTime[16];          ///< 转账时间
        EMoneyType          m_eMoneyType;                   ///< 币种
        char                m_strTransferNo[32];            ///< 流水号
        ETransDirection     m_eTransDirection;              ///< 转账方向
        double              m_dTransferBalance;             ///< 转账金额
        char                m_strBankAccount[32];           ///< 银行账号
        char                m_strBankName[32];              ///< 银行名称
        char                m_strBankNo[32];                ///< 银行代码
        char                m_strRemark[128];               ///< 备注
        int                 m_nTransferResult;              ///< 处理状态
    };

    ///< 资金股份划拨
    struct XT_API_EXPORT CSecuFundTransferReq
    {
        CSecuFundTransferReq();
        ~CSecuFundTransferReq();
        char                    m_strAccountID[32];             ///<  资金账户ID，如果为子账户，则为子账户ID
        char                    m_strExchangeID[32];            ///<  股份划拨市场
        char                    m_strInstrumentID[32];          ///<  股份划拨合约
        int                     m_nOccurAmount;                 ///<  股份划拨数量
        ETransTypeCreditFlag    m_eCredttransType;              ///<  股份划拨信用调拨类别
        ESecuFundTransDirection m_eTransDirection;              ///<  划拨方向
        double                  m_dOccurBalance;                ///<  资金划拨金额
    };

    ///< 普通柜台资金信息
    struct XT_API_EXPORT CStockComFund
    {
        CStockComFund();
        ~CStockComFund();
        char            m_strAccountID[32];             ///<  资金账户ID，如果为子账户，则为子账户ID
        char            m_strAccountKey[64];            ///<  账号key
        double          m_dAvailable;                   ///< 可用资金
    };

    ///< 普通柜台持仓信息
    struct XT_API_EXPORT CStockComPosition
    {
        CStockComPosition();
        ~CStockComPosition();
        char            m_strAccountID[32];             ///<  资金账户ID，如果为子账户，则为子账户ID
        char            m_strAccountKey[64];            ///<  账号key
        char            m_strExchangeID[32];            ///< 市场代码
        char            m_strInstrumentID[32];          ///< 合约代码
        char            m_strInstrumentName[32];        ///< 合约名称
        int             m_nVolume;                      ///< 持仓量
        int             m_nCanUseVolume;                ///< 可用数量
        double          m_dLastPrice;                   ///< 最新价
        double          m_dCostPrice;                   ///< 成本价
        double          m_dIncome;                      ///< 盈亏
        double          m_dIncomeRate;                  ///< 盈亏比例
        char            m_strHandFlag[4];               ///< 股手标记
        char            m_strStockAccount[16];          ///< 证券账号
        double          m_dInstrumentValue;             ///< 市值
        double          m_dCostBalance;                 ///< 成本总额
        int             m_nOnRoadVolume;                ///< 在途量
        int             m_nPREnableVolume;              ///< 申赎可用量
        bool            m_bFast;                        ///< 是否是从极速接口查到的
    };

    //保证金率
    struct XT_API_EXPORT CCommissionRateDetail
    {
        CCommissionRateDetail();
        ~CCommissionRateDetail();
        char            m_strExchangeID[32];            ///< 市场代码
        char            m_strInstrumentID[32];          ///< 合约代码
        double          m_dOpenRatioByMoney;            ///< 按金额开仓费率
        double          m_dOpenRatioByVolume;           ///< 按数量开仓费率
        double          m_dCloseRatioByMoney;           ///< 按金额平仓费率
        double          m_dCloseRatioByVolume;          ///< 按数量平仓费率
        double          m_dCloseTodayRatioByMoney;      ///< 按金额平今费率
        double          m_dCloseTodayRatioByVolume;     ///< 按数量平今费率
        char            m_strTradeDate[16];             ///< 交易日

    };

    //手续费率
    struct XT_API_EXPORT CMarginRateDetail
    {
        CMarginRateDetail();
        ~CMarginRateDetail();
        char            m_strExchangeID[32];            ///< 市场代码
        char            m_strInstrumentID[32];          ///< 合约代码
        int             m_nHedgeFlag;                   ///< 投机套保标志
        double          m_dLongMarginRatioByMoney;      ///< 按金额多头手续费
        double          m_dLongMarginRatioByVolume;     ///< 按数量多头手续费
        double          m_dShortMarginRatioByMoney;     ///< 按金额空头手续费率
        double          m_dShortMarginRatioByVolume;    ///< 按数量空头手续费
        int             m_nIsRelative;                  ///< 是否相对交易所收取
        char            m_strTradeDate[16];             ///< 交易日
    };
    
    //创建新投资组合参数
    struct XT_API_EXPORT CNewPortfolioReq
    {
        CNewPortfolioReq();
        ~CNewPortfolioReq();
        char            m_strName[256];             ///< 投资组合名称
        char            m_strStrategy[256];         ///< 策略名称
        char            m_strRemark[256];           ///< 备注
        char            m_strAccountKey[64];        ///< 账号key
        int             m_nProductID;               ///< 产品编号
        double          m_dRawBalance;              ///< 初始资产
    };

    ///< 投资组合信息
    struct XT_API_EXPORT CPortfolioInfo
    {
        CPortfolioInfo();
        ~CPortfolioInfo();
        int             m_nPortfolioID;             ///< 投组类型编号
        char            m_strName[256];             ///< 投资组合名称
        char            m_strStrategy[256];         ///< 策略名称
        char            m_strRemark[256];           ///< 备注
        char            m_strAccountKey[64];        ///< 账号key
        int             m_nProductID;               ///< 产品编号
        int             m_nCreateDate;              ///< 创建日期
        int             m_nUserId;                  ///< 用户编号
        int             m_nStatus;                  ///< 投资组合停用状态
        double          m_dInitBalance;             ///< 初始资产
        EPortfolioType  m_eType;                    ///< 投组类型
    };

    ///< 框架号信息
    struct XT_API_EXPORT CStrategyInfo
    {
        CStrategyInfo();
        ~CStrategyInfo();
        char            m_strStrategyID[32];        ///< 收益互换策略ID
        char            m_strstrategyName[256];     ///< 策略名称
        char            m_strAccountKey[64];        ///< 账号key
    };

    ///< 股东号信息
    struct XT_API_EXPORT CSecuAccount
    {
        CSecuAccount();
        ~CSecuAccount();
        char            m_strExchangeID[32];            ///< 市场代码
        char            m_strSecuAccount[32];           ///< 股东号
        EMainFlag       m_eMainFlag;                    ///< 主副标记
    };

    ///< 投资组合数据信息
    struct XT_API_EXPORT CPortfolioStat
    {
        CPortfolioStat();
        ~CPortfolioStat();
        int           m_nPortfolioID;               ///< 投组类型编号
        double        m_dInitBalance;               ///< 初始资产
        double        m_dBalance;                   ///< 总资产
        double        m_dAvailable;                 ///< 可用资金
        double        m_dPositionCost;              ///< 持仓成本
        double        m_dPositionProfit;            ///< 持仓盈亏
        double        m_dCloseProfit;               ///< 平仓盈亏
        double        m_dUsedMargin;                ///< 已用保证金
        double        m_dDeposit;                   ///< 入金
        double        m_dWithdraw;                  ///< 出金
        double        m_dRevenue;                   ///< 收入
        double        m_dExpenditure;               ///< 支出
        double        m_dShare;                     ///< 份额
        double        m_dNav;                       ///< 单位净值
        char          m_strTradeDate[32];           ///< 交易日期
        double        m_dPrevBalance;               ///< 期初权益
        double        m_dDailyProfit;               ///< 当日盈亏（结）
        double        m_dDailyCloseProfit;          ///< 当日盈亏（收）
        double        m_dCommission;                ///< 手续费
        double        m_dDebt;                      ///< 总负债
        double        m_dNetAsset;                  ///< 净资产
        double        m_dDailyDeposit;              ///< 当日入金
        double        m_dDailyWithdraw;             ///< 当日出金
        double        m_dFundBalance;               ///< 现金资产
        double        m_dPortfolioProfit;           ///< 组合盈亏
    };

    ///< 券源预约
    struct XT_API_EXPORT CAppointOrder
    {
        CAppointOrder();
        ~CAppointOrder();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strFinType[32];         ///< 金融品种
        int                           m_nDate;                  ///< 期限天数
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        int                           m_nVolume;                ///< 委托数量
        double                        m_dApplyRate;             ///< 资券申请利率
        double                        m_dUsedRate;              ///< 资券使用利率
        double                        m_dUnUsedRate;            ///< 资券占用未使用利率
    };

    ///< 券源撤单
    struct XT_API_EXPORT CAppointCancel
    {
        CAppointCancel();
        ~CAppointCancel();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strFinType[32];         ///< 金融品种
        char                          m_strEntrustNo[32];       ///< 委托编号
    };

    ///< 议价申请
    struct XT_API_EXPORT CNegotiateOrder
    {
        CNegotiateOrder();
        ~CNegotiateOrder();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strFinType[32];         ///< 金融品种
        int                           m_nDate;                  ///< 期限天数
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        int                           m_nVolume;                ///< 委托数量
        double                        m_dApplyRate;             ///< 资券申请利率
        double                        m_dUsedRate;              ///< 资券使用利率
        double                        m_dUnUsedRate;            ///< 资券占用未使用利率
        int                           m_nValidDtae;             ///< 有效日期
        char                          m_strFsmpSrcGroupId[256]; ///< 来源组编号

    };

    ///< 议价行情查询请求
    struct XT_API_EXPORT CNegotiateQuoterDetailReq
    {
        CNegotiateQuoterDetailReq();
        ~CNegotiateQuoterDetailReq();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strFinType[32];         ///< 金融品种
        int                           m_nDate;                  ///< 期限天数
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strExchangeID[16];      ///< 交易所代码
    };

    ///< 议价行情查询返回
    struct XT_API_EXPORT CNegotiateQuoterDetailResp
    {
        CNegotiateQuoterDetailResp();
        ~CNegotiateQuoterDetailResp();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strFinType[32];         ///< 金融品种
        char                          m_strStockType[8];       ///< 证券类型
        int                           m_nDate;                  ///< 期限天数
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strInstrumentName[256]; ///< 合约名称
        char                          m_strExchangeID[16];      ///< 交易所代码
        double                        m_dFsmpOccupedRate;       ///< 资券占用利率
        double                        m_dFineRate;              ///< 罚息利率
        double                        m_dFsmpPreendRate;        ///< 资券提前归还利率
        double                        m_dUsedRate;              ///< 资券使用利率
        double                        m_dUnUsedRate;            ///< 资券占用未使用利率
        int                           m_nInitDate;              ///< 交易日期
        int                           m_nEndDate;               ///< 到期日期
        double                        m_dEnableSloAmountT0;     ///< T+0可融券数量
        double                        m_dEnableSloAmountT3;     ///< T+3可融券数量
        char                          m_strFsmpSrcGroupId[256]; ///< 来源组编号
        char                          m_strApplyMode[4];        ///< 资券申请方式
    };

    ///< 约券合约查询
    struct XT_API_EXPORT CCashcompact
    {
        CCashcompact();
        ~CCashcompact();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        int                           m_nCreateDate;            ///< 创建日期
        char                          m_strCashcompactId[128];   ///< 头寸合约编号
        char                          m_strOriCashcompactId[128];///< 原头寸合约编号
        char                          m_strApplyId[32];         ///< 资券申请编号
        char                          m_strFsmpSrcGroupId[256]; ///< 来源组编号
        char                          m_strComGroupId[128];      ///< 资券组合编号
        char                          m_strFinType[32];         ///< 金融品种
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strInstrumentName[32];  ///< 合约名称
        int                           m_nDate;                  ///< 期限天数
        double                        m_dBeginCompacAmount;     ///< 期初合约数量
        double                        m_dBeginCompacBalance;    ///< 期初合约金额
        double                        m_dCompacAmount;          ///< 合约数量
        double                        m_dCompacBalance;         ///< 合约金额
        double                        m_dReturnAmount;          ///< 返还数量
        double                        m_dReturnBalance;         ///< 返还金额
        double                        m_dRealBuyAmount;         ///< 回报买入数量
        double                        m_dFsmpOccupedRate;       ///< 资券占用利率
        double                        m_dCompactInterest;       ///< 合约利息金额
        double                        m_dCompactFineInterest;   ///< 合约罚息金额
        double                        m_dRepaidInterest;        ///< 已还利息
        double                        m_dRepaidFineInterest;    ///< 归还罚息
        double                        m_dFineRate;              ///< 罚息利率
        double                        m_dPreendRate;            ///< 资券提前归还利率
        char                          m_strCompactType[4];      ///< 资券合约类型
        int                           m_nPostponeTimes;         ///< 展期次数
        char                          m_strCompactStatus[4];    ///< 资券合约状态
        int                           m_nLastInterestDate;      ///< 上次结息日期
        int                           m_nInterestEndDate;       ///< 记息结束日期
        int                           m_nValidDate;             ///< 有效日期
        int                           m_nDateClear;             ///< 清算日期
        double                        m_dUsedAmount;            ///< 已使用数量
        double                        m_dUsedBalance;           ///< 已使用金额
        double                        m_dUsedRate;              ///< 资券使用利率
        double                        m_dUnUsedRate;            ///< 资券占用未使用利率
        char                          m_strSrcGroupName[256];    ///< 来源组名称
        int                           m_nRepaidDate;            ///< 归还日期
        double                        m_dPreOccupedInterest;    ///< 当日实际应收利息
        double                        m_dCompactInterestx;      ///< 合约总利息
        double                        m_dEnPostponeAmount;      ///< 可展期数量
        char                          m_strPostponeStatus[4];   ///< 合约展期状态
        char                          m_strApplyMode[4];        ///< 资券申请方式
    };

    ///< 约券申请查询
    struct XT_API_EXPORT CApplyDetail
    {
        CApplyDetail();
        ~CApplyDetail();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        int                           m_nInitDate;              ///< 交易日期
        int                           m_nCurrDate;              ///< 当前日期
        int                           m_nCurrTime;              ///< 当前时间
        char                          m_strApplyId[32];         ///< 资券申请编号
        char                          m_strFsmpSrcGroupId[256]; ///< 来源组编号
        char                          m_strCashcompactId[32];   ///< 头寸合约编号
        char                          m_strApplyMode[4];       ///< 资券申请方式
        char                          m_strFinType[32];         ///< 金融品种
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strInstrumentName[256];  ///< 合约名称
        int                           m_nDate;                  ///< 期限天数
        double                        m_dApplyRate;             ///< 资券申请利率
        double                        m_dEntrustBalance;        ///< 委托金额
        int                           m_dEntrustAmount;         ///< 委托数量
        double                        m_dBusinessBalance;       ///< 成交金额
        double                        m_dBusinessAmount;        ///< 成交数量
        int                           m_nValidDate;             ///< 有效日期
        int                           m_nDateClear;             ///< 清算日期
        char                          m_strEntrustNo[32];       ///< 委托编号
        char                          m_strApplyStatus[4];     ///< 资券申请状态
        double                        m_dUsedRate;              ///< 资券使用利率
        double                        m_dUnUsedRate;            ///< 资券占用未使用利率
        char                          m_strComGroupId[256];      ///< 资券组合编号
    };

    ///< 券源归还申请
    struct XT_API_EXPORT CReturnApply
    {
        CReturnApply();
        ~CReturnApply();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strFinType[32];         ///< 金融品种
        char                          m_strFsmpSrcGroupId[256]; ///< 来源组编号
        char                          m_strCompactIdstr[256];   ///< 合约编号串
        double                        m_dOccurAmount;           ///< 发生数量
        double                        m_dOccurBalance;          ///< 发生金额

    };

    ///< 券源归还申请返回
    struct XT_API_EXPORT CReturnApplyResp
    {
        CReturnApplyResp();
        ~CReturnApplyResp();
        char                          m_strCashcompactID[256];  ///< 头寸合约编号
        double                        m_dOccurAmount;           ///< 发生数量
        double                        m_dOccurBalance;          ///< 发生金额

    };

    ///< 券源可约券单查询
    struct XT_API_EXPORT CStockSource
    {
        CStockSource();
        ~CStockSource();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strInstrumentName[32];  ///< 合约名称
        char                          m_strSrcProp[32];         ///< 来源属性
        double                        m_dEnableLendAmount;      ///< 发生数量发生数量
        double                        m_dEnableLendData;        ///< 可出借期限
        char                          m_strRuleNote[1024];      ///< 规则说明
    };

    ///< 融券通出借行情发布
    struct XT_API_EXPORT CSvrFsmpslpHqIssue
    {
        CSvrFsmpslpHqIssue();
        ~CSvrFsmpslpHqIssue();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strSlphqType[32];       ///< 行情类别
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        int                           m_nEntrustTerm;           ///< 委托期限
        int                           m_nEntrustTermMin;        ///< 委托期限下限
        int                           m_nEntrustTermMax;        ///< 委托期限上限
        int                           m_nIssueAmount;           ///< 发布数量
        double                        m_dEntrustRate;           ///< 委托利率
        int                           m_nValidDate;             ///< 有效日期
        char                          m_strRemark[256];         ///< 备注
        char                          m_strCsfcCompactId[32];   ///< 证金合约编号
        int                           m_nOneLowRptAmount;       ///< 单笔最低申报数量
        int                           m_nSlphqEndDate;          ///< 行情截止日期
        char                          m_strReflenderCode[32];   ///< 出借人代码
    };

    ///< 融券通出借行情查询
    struct XT_API_EXPORT CSvrFsmpslpHqQryReq
    {
        CSvrFsmpslpHqQryReq();
        ~CSvrFsmpslpHqQryReq();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strReflenderCode[32];   ///< 出借人代码
        char                          m_strSlphqType[32];       ///< 行情类别
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strSlphqNo[128];         ///< 行情编号
        char                          m_strEnSlphqStatus[32];   ///< 允许行情发布状态
        char                          m_strSlpSourceType[32];   ///< 订单发起者类型
        char                          m_strActionIn[32];        ///< 操作控制值

    };

    ///< 融券通出借行情查询返回
    struct XT_API_EXPORT CSvrFsmpslpHqQryResp
    {
        CSvrFsmpslpHqQryResp();
        ~CSvrFsmpslpHqQryResp();
        int                           m_nInitDate;              ///< 交易日期
        int                           m_nSerialNo;              ///< 流水序号
        char                          m_strSlphqType[32];       ///< 行情类别
        char                          m_strSlphqNo[128];         ///< 行情编号
        char                          m_strReflenderCode[32];   ///< 出借人代码
        char                          m_strFsmpSrcGroupId[256];    ///< 来源组编号
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strStockAccount[16];    ///< 证券账号
        char                          m_strSeatNo[16];          ///< 席位编号
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        int                           m_nEntrustTerm;           ///< 委托期限
        int                           m_nEntrustTermMin;        ///< 出借期限下限
        int                           m_nEntrustTermMax;        ///< 出借期限上限
        int                           m_nIssueAmount;           ///< 发布数量
        double                        m_dBusinessAmount;        ///< 成交数量
        double                        m_dEntrustRate;           ///< 委托利率
        double                        m_dFsmpPreendRate;        ///< 资券提前归还利率
        double                        m_dFineRate;              ///< 罚息利率
        int                           m_nValidDate;             ///< 有效日期
        char                          m_strSlphqStatus[32];     ///< 行情发布状态
        char                          m_strRemark[128];         ///< 备注
        char                          m_strSlpSourceType[32];   ///< 订单发起者类型
        char                          m_strCsfcCompactId[32];   ///< 证金合约编号
        char                          m_strCashcompactId[32];   ///< 头寸合约编号
        char                          m_strOppCsfcCompactId[32];///< 对手方证金合约编号
        int                           m_nOneLowRptAmount;       ///< 单笔最低申报数量
        int                           m_nSlphqEndDate;          ///< 行情截止日期
        int                           m_nDateClear;             ///< 清算日期
        int                           m_nEnableAmount;          ///< 可用数量
        double                        m_dFyrRefborrRate;        ///< 参考借入利率
        double                        m_dFyrFineRate;           ///< 参考罚息利率
        double                        m_dFyrRefpreeneRate;      ///< 参考提交归还利率
    };

    ///< 融券通申请
    struct XT_API_EXPORT CExtFsmpslpApply
    {
        CExtFsmpslpApply();
        ~CExtFsmpslpApply();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strSlphqNo[128];         ///< 行情编号
        int                           m_dEntrustAmount;         ///< 委托数量
        int                           m_nEntrustTerm;           ///< 委托期限
        char                          m_strStockAccount[16];    ///< 证券账号
        int                           m_nValidDate;             ///< 有效日期
    };

    ///< 融券通行情确认
    struct XT_API_EXPORT CSvrFsmpslpHqComfirm
    {
        CSvrFsmpslpHqComfirm();
        ~CSvrFsmpslpHqComfirm();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strReflenderCode[32];   ///< 出借人代码
        char                          m_strSlphqNo[128];         ///< 行情编号
        char                          m_strActionFlag[32];      ///< 操作标志

    };
    ///< 约定号查询
    struct XT_API_EXPORT CSvrFsmpslpRefbreportQryReq
    {
        CSvrFsmpslpRefbreportQryReq();
        ~CSvrFsmpslpRefbreportQryReq();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strReflenderCode[32];   ///< 出借人代码
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strSlphqNo[128];         ///< 行情编号
    };

    ///< 约定号查询返回
    struct XT_API_EXPORT CSvrFsmpslpRefbreportQryResp
    {
        CSvrFsmpslpRefbreportQryResp();
        ~CSvrFsmpslpRefbreportQryResp();
        int                           m_nInitDate;              ///< 交易日期
        int                           m_nReportNo;              ///< 申请编号
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strRefbusiCode[32];     ///< 转融通业务代码
        int                           m_nRefTerm;               ///< 转融通期限天数
        char                          m_strCbpconferId[32];     ///< 约定号
        int                           m_nReportAmount;          ///< 申报数量
        char                          m_strReportStatus[4];     ///< 申报状态
        char                          m_strReflenderCode[32];   ///< 出借人代码

    };

    ///< 融券通借入行情发布
    struct XT_API_EXPORT CExtFsmpslphqIssue
    {
        CExtFsmpslphqIssue();
        ~CExtFsmpslphqIssue();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strSlphqType[32];       ///< 行情类别
        char                          m_strCashcompactId[32];   ///< 头寸合约编号
        int                           m_nEntrustTerm;           ///< 委托期限
        int                           m_nIssueAmount;           ///< 发布数量
        double                        m_dEntrustRate;           ///< 委托利率
        int                           m_nSlphqEndDate;          ///< 行情截止日期
        char                          m_strRemark[128];         ///< 投资备注
        int                           m_nValidDate;             ///< 有效日期
        double                        m_dPostPoneFsmppreendRate;///< 展期提前归还利率
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
    };

    ///< 出借人信息查询
    struct XT_API_EXPORT CSvrFsmpslqLendacct
    {
        CSvrFsmpslqLendacct();
        ~CSvrFsmpslqLendacct();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strReflenderCode[32];   ///< 出借人代码
        char                          m_strFsmpSrcGroupId[256]; ///< 来源组编号
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strStockAccount[16];    ///< 证券账号
        char                          m_strSeatNo[16];          ///< 席位编号
    };

    ///< 对手方行情利率获取
    struct XT_API_EXPORT CExtFsmpslpOpphqRateReq
    {
        CExtFsmpslpOpphqRateReq();
        ~CExtFsmpslpOpphqRateReq();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strSlphqNo[128];         ///< 行情编号
        int                           m_nTerm;                  ///< 期现天数
    };

    ///< 对手方行情利率获取返回
    struct XT_API_EXPORT CExtFsmpslpOpphqRateResp
    {
        CExtFsmpslpOpphqRateResp();
        ~CExtFsmpslpOpphqRateResp();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strSlphqNo[128];         ///< 行情编号
        int                           m_nRate;                  ///< 对手方委托利率
    };

    ///< agent融券通出借行情查询
    struct XT_API_EXPORT CtAgentSvrFsmpslpHqQryReq
    {
        CtAgentSvrFsmpslpHqQryReq();
        ~CtAgentSvrFsmpslpHqQryReq();
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strReflenderCode[32];   ///< 出借人代码
        char                          m_strSlphqType[32];       ///< 行情类别
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        char                          m_strSlphqNo[128];         ///< 行情编号
        char                          m_strEnSlphqStatus[32];   ///< 允许行情发布状态
        char                          m_strSlpSourceType[32];   ///< 订单发起者类型
        char                          m_strActionIn[32];        ///< 操作控制值
    };

    ///< agent融券通出借行情查询返回
    struct XT_API_EXPORT CtAgentSvrFsmpslpHqQryResp
    {
        CtAgentSvrFsmpslpHqQryResp();
        ~CtAgentSvrFsmpslpHqQryResp();
        int                           m_nInitDate;              ///< 交易日期
        int                           m_nSerialNo;              ///< 流水序号
        char                          m_strSlphqType[32];       ///< 行情类别
        char                          m_strSlphqNo[128];         ///< 行情编号
        char                          m_strReflenderCode[32];   ///< 出借人代码
        char                          m_strFsmpSrcGroupId[256];    ///< 来源组编号
        char                          m_strAccountID[32];       ///< 资金账户ID，如果为子账户，则为子账户ID
        char                          m_strStockAccount[16];    ///< 证券账号
        char                          m_strSeatNo[16];          ///< 席位编号
        char                          m_strExchangeID[16];      ///< 交易所代码
        char                          m_strInstrumentID[32];    ///< 合约代码
        int                           m_nEntrustTerm;           ///< 委托期限
        int                           m_nEntrustTermMin;        ///< 出借期限下限
        int                           m_nEntrustTermMax;        ///< 出借期限上限
        int                           m_nIssueAmount;           ///< 发布数量
        double                        m_dBusinessAmount;        ///< 成交数量
        double                        m_dEntrustRate;           ///< 委托利率
        double                        m_dFsmpPreendRate;        ///< 资券提前归还利率
        double                        m_dFineRate;              ///< 罚息利率
        int                           m_nValidDate;             ///< 有效日期
        char                          m_strSlphqStatus[32];     ///< 行情发布状态
        char                          m_strRemark[128];         ///< 备注
        char                          m_strSlpSourceType[32];   ///< 订单发起者类型
        char                          m_strCsfcCompactId[32];   ///< 证金合约编号
        char                          m_strCashcompactId[32];   ///< 头寸合约编号
        char                          m_strOppCsfcCompactId[32];///< 对手方证金合约编号
        int                           m_nOneLowRptAmount;       ///< 单笔最低申报数量
        int                           m_nSlphqEndDate;          ///< 行情截止日期
        int                           m_nDateClear;             ///< 清算日期
        int                           m_nEnableAmount;          ///< 可用数量
        double                        m_dOppoEntrustRate;      ///< 参考提交归还利率

    };
}

#endif
