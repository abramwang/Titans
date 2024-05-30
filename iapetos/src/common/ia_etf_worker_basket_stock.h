#ifndef __IA_ETF_WORKER_BASKET_STOCK_H__
#define __IA_ETF_WORKER_BASKET_STOCK_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_info_struct.h"
#include "ia_etf_worker_signle_stock.h"
#include <unordered_map>
#include <memory.h>
#include <map>

/*
{
    symbol: "000001",               //股票代码
    exchange: "SH",                 //股票市场
    expect_cost: 1000000,           //期望交易成本 包含成交额和手续费
    volume: 1000000,                //交易数量
    real_cost: 1000000,             //实际交易成本 包含成交额和手续费
    finish_volume: 1000000,         //实际成交数量
    orders: [
        {
            req_id,
            order_id,
        }
    ],                    //股票交易信息
}
*/

class IaETFWorkerBasketStock:
    public IaETFTradingWorker
{
public:
    struct Status
    {
        std::string symbol;
        std::string exchange;
        int stock_num;
        int over_num;
    }; 
public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){};     //非交易逻辑的统一实现接口
    
    virtual void OnRspAccountInfo(const TiRspAccountInfo* pData){};

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

    virtual void OnTimer();
private:
    TI_TradeSideType m_side;
    Status m_status;
    int64_t m_check_time;   //检查时间

    std::map<std::string, IaETFWorkerSingleStockPtr> m_trading_worker_map;
public:
    int64_t open();
    json getStatusJson();
    bool isOver();
public:
    IaETFWorkerBasketStock(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache, IaEtfSignalFactorPtr factor, std::string account, 
        TI_TradeSideType side);
    virtual ~IaETFWorkerBasketStock(){};
};

typedef std::shared_ptr<IaETFWorkerBasketStock> IaETFWorkerBasketStockPtr;

#endif