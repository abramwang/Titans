#ifndef __IA_ETF_WORKER_BATCH_STOCK_H__
#define __IA_ETF_WORKER_BATCH_STOCK_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_info_struct.h"
#include <unordered_map>
#include <memory.h>

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

class IaETFWorkerBatchStock:
    public IaETFTradingWorker
{
public:
    struct Status
    {
        std::string symbol;
        std::string exchange;
        double expect_cost;
        int32_t volume;
        double real_cost;
        int32_t finish_volume;
    }; 
public:
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnTimer();
private:
    TI_TradeSideType m_side;
    Status m_status;
    int64_t m_check_time;   //检查时间

    TiQuoteSnapshotStockField m_open_snap;    
    std::set<int64_t> m_req_id_set;                                 // order_req_id
    std::unordered_map<std::string, TiRtnOrderStatus> m_order_map;  // szOrderStreamId -> order_status
private:
    void updateExpectCost(TiQuoteSnapshotStockField* pData);
    void updateStatus();
    bool hasQueueOrder();
public:
    int64_t open();
    json getStatusJson();
    bool isOver();
public:
    IaETFWorkerBatchStock(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache, IaEtfSignalFactorPtr factor, std::string account, 
       TI_TradeSideType side);
    virtual ~IaETFWorkerBatchStock(){};
};

typedef std::shared_ptr<IaETFWorkerBatchStock> IaETFWorkerBatchStockPtr;

#endif