#ifndef __API_KY_MARKET_STRUCT_H__
#define __API_KY_MARKET_STRUCT_H__

#include <stdint.h>
#include <stdbool.h>

#pragma pack(push,1)

typedef struct KY_DATA_REPLY
{
	int     nID;                    // 请求对应的ID
	char    szCommand[32];          // 请求的命令
	int     nRtn;                   // 请求返回的值 0 为成功 其他为错误编码
	char    szErr[255];             // 请求返回的错误信息 
	char    *szMsg;                 // 请求返回的数据信息
	int     *nSize;                 // 请求返回的数据长度
}KY_DATA_REPLY;

typedef struct KY_DATA_CODE
{
	char    szWindCode[32];         //Wind Code: AG1302.SHF
	char    szMarket[8];            //market code: SHF
	char    szCode[32];             //original code:ag1302
	char    szENName[32];
	char    szCNName[32];           //chinese name: 沪银1302
	int     nType;
}KY_DATA_CODE;

typedef struct KY_DATA_OPTION_CODE
{
	KY_DATA_CODE basicCode;

	char szContractID[32];          // 期权合约代码
	char szUnderlyingSecurityID[32];// 标的证券代码
	char chCallOrPut;               // 认购认沽C1        认购，则本字段为“C”；若为认沽，则本字段为“P”
	int  nExerciseDate;             // 期权行权日，YYYYMMDD

	//扩充字段
	char chUnderlyingType;          // 标的证券类型C3    0-A股 1-ETF (EBS – ETF， ASH – A 股)
	char chOptionType;              // 欧式美式C1        若为欧式期权，则本字段为“E”；若为美式期权，则本字段为“A”

	char chPriceLimitType;          // 涨跌幅限制类型C1 ‘N’表示有涨跌幅限制类型, ‘R’表示无涨跌幅限制类型
	int  nContractMultiplierUnit;   // 合约单位,         经过除权除息调整后的合约单位, 一定是整数
	int  nExercisePrice;            // 期权行权价,       经过除权除息调整后的期权行权价，右对齐，精确到厘
	int  nStartDate;                // 期权首个交易日,YYYYMMDD
	int  nEndDate;                  // 期权最后交易日/行权日，YYYYMMDD
	int  nExpireDate;               // 期权到期日，YYYYMMDD
}KY_DATA_OPTION_CODE;

// 备注: 本结构体各字段中的单位为参考单位, 如有异议, 请以市面上常见的行情软件的盘口单位为准
typedef struct KY_DATA_MARKET
{
	char            szWindCode[32];         //600001.SH
	char            szCode[32];             //原始Code
	int             nActionDay;             //日期(YYYYMMDD)
	int             nTime;                  //时间(HHMMSSmmm)
	int             nStatus;                //状态
	unsigned int    nPreClose;              //前收盘价=实际价格(单位: 元/股)x10000
	unsigned int    nOpen;                  //开盘价=实际价格(单位: 元/股)x10000
	unsigned int    nHigh;                  //最高价=实际价格(单位: 元/股)x10000
	unsigned int    nLow;                   //最低价=实际价格(单位: 元/股)x10000
	unsigned int    nMatch;                 //最新价=实际价格(单位: 元/股)x10000
	unsigned int    nAskPrice[10];          //申卖价=实际价格(单位: 元/股)x10000
	unsigned int    nAskVol[10];            //申卖量=实际股数(单位: 股)
	unsigned int    nBidPrice[10];          //申买价=实际价格(单位: 元/股)x10000
	unsigned int    nBidVol[10];            //申买量=实际股数(单位: 股)
	unsigned int    nNumTrades;             //成交笔数=实际笔数(单位: 笔)
	int64_t         iVolume;                //成交总量=实际股数(单位: 股)
	int64_t         iTurnover;              //成交总金额=实际金额(单位: 元)
	int64_t         nTotalBidVol;           //委托买入总量=实际股数(单位: 股)
	int64_t         nTotalAskVol;           //委托卖出总量=实际股数(单位: 股)
	unsigned int    nWeightedAvgBidPrice;   //加权平均委买价格=实际价格(单位: 元/股)x10000
	unsigned int    nWeightedAvgAskPrice;   //加权平均委卖价格=实际价格(单位: 元/股)x10000
	int             nIOPV;                  //IOPV净值估值
	int             nYieldToMaturity;       //到期收益率
	unsigned int    nHighLimited;           //涨停价=实际价格(单位: 元/股)x10000
	unsigned int    nLowLimited;            //跌停价=实际价格(单位: 元/股)x10000
} KY_DATA_MARKET;

// 备注: 本结构体各字段中的单位为参考单位, 如有异议, 请以市面上常见的行情软件的盘口单位为准
typedef struct KY_DATA_INDEX
{
	char        szWindCode[32];         //600001.SH
	char        szCode[32];             //原始Code
	int         nActionDay;             //日期(YYYYMMDD)
	int         nTime;                  //时间(HHMMSSmmm)
	int         nOpenIndex;             //今开盘指数=实际价格(单位: 元/股)x10000
	int         nHighIndex;             //最高指数=实际价格(单位: 元/股)x10000
	int         nLowIndex;              //最低指数=实际价格(单位: 元/股)x10000
	int         nLastIndex;             //最新指数=实际价格(单位: 元/股)x10000
	int64_t     iTotalVolume;           //参与计算相应指数的交易数量=实际股数(单位: 股)
	int64_t     iTurnover;              //参与计算相应指数的成交金额=实际金额(单位: 元)
	int         nPreCloseIndex;         //前盘指数=实际价格(单位: 元/股)x10000
}KY_DATA_INDEX;

// 备注: 本结构体为通用期货数据结构体, 期货品种不同则交易单位不同, 如有异议, 请以市面上常见的行情软件的盘口单位为准
typedef struct KY_DATA_FUTURE
{
	char            szWindCode[32];         //600001.SH
	char            szCode[32];             //原始Code
	int             nActionDay;             //日期(YYYYMMDD)
	int             nTime;                  //时间(HHMMSSmmm)
	int             nStatus;                //状态
	int64_t         iPreOpenInterest;       //昨持仓
	unsigned int    nPreClose;              //昨收盘价=实际价格(单位: 元/吨)x10000
	unsigned int    nPreSettlePrice;        //昨结算=实际价格(单位: 元/吨)x10000
	unsigned int    nOpen;                  //开盘价=实际价格(单位: 元/吨)x10000
	unsigned int    nHigh;                  //最高价=实际价格(单位: 元/吨)x10000
	unsigned int    nLow;                   //最低价=实际价格(单位: 元/吨)x10000
	unsigned int    nMatch;                 //最新价=实际价格(单位: 元/吨)x10000
	int64_t         iVolume;                //成交总量=实际手数(单位: 手)
	int64_t         iTurnover;              //成交总金额=实际金额(单位: 元)
	int64_t         iOpenInterest;          //持仓总量=实际手数(单位: 手)
	unsigned int    nClose;                 //今收盘=实际价格(单位: 元/吨)x10000
	unsigned int    nSettlePrice;           //今结算=实际价格(单位: 元/吨)x10000
	unsigned int    nHighLimited;           //涨停价=实际价格(单位: 元/吨)x10000
	unsigned int    nLowLimited;            //跌停价=实际价格(单位: 元/吨)x10000
	unsigned int    nAskPrice[5];           //申卖价=实际价格(单位: 元/股)x10000
	unsigned int    nAskVol[5];             //申卖量=实际手数(单位: 手)
	unsigned int    nBidPrice[5];           //申买价=实际价格(单位: 元/吨)x10000
	unsigned int    nBidVol[5];             //申买量=实际手数(单位: 手)
}KY_DATA_FUTURE;

// 备注: 本结构体各字段中的单位为参考单位, 如有异议, 请以市面上常见的行情软件的盘口单位为准
typedef struct KY_DATA_ORDER_QUEUE
{
	char    szWindCode[32]; //600001.SH
	char    szCode[32];     //原始Code
	int     nActionDay;     //日期(YYYYMMDD)
	int     nTime;          //时间(HHMMSSmmm)
	int     nSide;          //买卖方向('B':Bid 'A':Ask)
	int     nPrice;         //委托价格=实际价格(单位: 元/手)x10000
	int     nOrders;        //挂单挡位
	int     nABItems;       //委托单数
	int     nABVolume[200]; //委托数量=实际股数(单位: 股)
}KY_DATA_ORDER_QUEUE;

typedef struct KY_DATA_MMP_QUEUE
{
	char    szWindCode[32]; //600001.SH
	char    szCode[32];     //原始Code
	int     nActionDay;     //日期(YYYYMMDD)
	int     nTime;          //时间(HHMMSSmmm)
	int     nBuyPrice;         //委托价格=实际价格(单位: 元/手)x10000
	int     nBuyItems;         // 实际卖一委托单数量
	int     nBuyVolume[50];    // 最大50个（股）
	int     nSellPrice;        //委托价格=实际价格(单位: 元/手)x10000
	int     nSellItems;        // 实际卖一委托单数量
	int     nSellVolume[50];   // 最大50个（股）
}KY_DATA_MMP_QUEUE;
// 备注: 本结构体各字段中的单位为参考单位, 如有异议, 请以市面上常见的行情软件的盘口单位为准
typedef struct KY_DATA_TRANSACTION
{
	char    szWindCode[32]; //600001.SH
	char    szCode[32];     //原始Code
	int     nActionDay;     //成交日期(YYYYMMDD)
	int     nTime;          //成交时间(HHMMSSmmm)
	int     nIndex;         //成交编号
	int     nPrice;         //成交价格=实际价格(单位: 元/股)x10000
	int     nVolume;        //成交数量=实际股数(单位: 股)
	int     nTurnover;      //成交金额=实际金额(单位: 元)
	int     nBSFlag;        //买卖方向(买：66-'B', 卖：83-'S', 不明：78-'N'(上海集合竞价常见, 或者不支持nBSFlag), 撤单: 32-' '(深圳撤单 这种情况下nPrice/nTurnover均为0))
	char    chOrderKind;    //成交类别
	int     nAskOrder;      //叫卖方委托序号
	int     nBidOrder;      //叫买方委托序号
}KY_DATA_TRANSACTION;

// 备注: 本结构体各字段中的单位为参考单位, 如有异议, 请以市面上常见的行情软件的盘口单位为准
typedef struct KY_DATA_ORDER
{
	char    szWindCode[32]; //600001.SH
	char    szCode[32];     //原始Code
	int     nActionDay;     //委托日期(YYYYMMDD)
	int     nTime;          //委托时间(HHMMSSmmm)
	int     nOrder;         //委托号
	int     nPrice;         //委托价格=实际价格(单位: 元/股)x10000
	int     nVolume;        //委托数量=实际股数(单位: 股)
	char    chOrderKind;    //委托类别
	char    chFunctionCode; //委托代码('B','S','C')
}KY_DATA_ORDER;

#pragma pack(pop)

#endif