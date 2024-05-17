#ifndef __IA_ETF_WORKER_BUY_ETF_H__
#define __IA_ETF_WORKER_BUY_ETF_H__

#include "ia_etf_trading_worker.h"

#include <unordered_map>
#include <memory.h>

class IaETFWorkerBuyEtf :
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
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){};     //非交易逻辑的统一实现接口
    
    virtual void OnRspAccountInfo(const TiRspAccountInfo* pData){};

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

public:
    virtual void OnTimer(){};
private:
    int64_t m_req_id;       //当前订单请求id
    Status m_status;

    TiQuoteSnapshotStockField m_open_snap;
    std::unordered_map<int64_t, TiRtnOrderStatus> m_order_map;  // order_id -> order_status
private:
    void updateStatus(){};
    bool hasQueueOrder(){return true;};
public:
    int64_t open();
    bool isOver(){return false;};
public:
    IaETFWorkerBuyEtf(IaEtfQuoteDataCache* m_quote_cache, TiGtTraderClient* client, IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFWorkerBuyEtf(){};
};

typedef std::shared_ptr<IaETFWorkerBuyEtf> IaETFWorkerBuyEtfPtr;

#endif