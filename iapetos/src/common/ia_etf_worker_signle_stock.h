#ifndef __IA_ETF_WORKER_SINGLE_STOCK_H__
#define __IA_ETF_WORKER_SINGLE_STOCK_H__

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

class IaETFWorkerSingleStock:
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
        int32_t wrong_number;
        bool is_limit;  //是否是涨跌停价格
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

    TiQuoteSnapshotStockField m_open_snap;    
    DeleteOrderReqInfo m_canceling_order_info;                      // canceling order
    std::set<int64_t> m_req_id_set;                                 // order_req_id
    std::unordered_map<int64_t, TiRtnOrderStatus> m_order_map;      // nOrderId -> order_status
private:
    void updateExpectCost(TiQuoteSnapshotStockField* pData);
    void updateStatus();
    bool hasQueueOrder();
    void cancelOrder(TiRtnOrderStatus* order);
public:
    int64_t open();
    json getStatusJson();
    bool isOver();
public:
    IaETFWorkerSingleStock(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache, IaEtfSignalFactorPtr factor, std::string account, 
        std::shared_ptr<IaEtfConstituentInfo> constituentInfoPtr, TI_TradeSideType side);
    virtual ~IaETFWorkerSingleStock(){};
};

typedef std::shared_ptr<IaETFWorkerSingleStock> IaETFWorkerSingleStockPtr;

#endif