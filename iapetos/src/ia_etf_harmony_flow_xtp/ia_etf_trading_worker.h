#ifndef __IA_ETF_TRADING_WORKER_H__
#define __IA_ETF_TRADING_WORKER_H__

#include "ti_trader_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_factor_callback.h"
#include "ti_trader_client.h"

class IaETFTradingWorker
    : public TiTraderCallback, TiQuoteDepthCallback, TiFactorCallback
{
public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){}; 
    
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){};
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

    
    virtual void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids){};
    
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData){};

    virtual void OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data){};

    virtual void OnTimer(){};
protected:
    TiTraderClient* m_client;
public:
    IaETFTradingWorker(TiTraderClient* client);
    virtual ~IaETFTradingWorker(){};
};

#endif