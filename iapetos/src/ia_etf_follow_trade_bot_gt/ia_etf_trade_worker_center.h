#ifndef __IA_ETF_TRADE_WORKER_CENTER_H__
#define __IA_ETF_TRADE_WORKER_CENTER_H__

#include "ti_trader_client.h"
#include "ti_trader_callback.h"

#include "ti_gt_trader_client.h"
#include "ia_etf_info_mysql.h"
#include "ia_etf_quote_data_cache.h"

class IaEtfTradeWorkerCenter
    : public TiTraderCallback
{

public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){};     //非交易逻辑的统一实现接口
    
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){};
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

private:
    TiGtTraderClient* m_trade_client;
    IaEtfInfoMysql* m_mysql;
    IaEtfQuoteDataCache* m_quote_cache;
public:
    IaEtfTradeWorkerCenter(TiGtTraderClient* trade_client, IaEtfQuoteDataCache* quote_cache, IaEtfInfoMysql* mysql);
    ~IaEtfTradeWorkerCenter();

private:
    void init_etf_info();

};

#endif // __IA_ETF_TRADE_WORKER_CENTER_H__