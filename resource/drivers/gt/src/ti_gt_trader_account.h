#ifndef TI_GT_TRADER_ACCOUNT_H
#define TI_GT_TRADER_ACCOUNT_H

#include "ti_trader_callback.h"

class TiGtTraderAccount: 
    public TiTraderCallback
{
private:
    TiTraderCallback* m_cb;

    std::unordered_map<std::string, std::shared_ptr<TiRtnOrderStatus>>  m_order_sys_map;     //nOrderSysID, order_status
    std::unordered_map<int64_t, std::shared_ptr<TiRtnOrderStatus>>      m_order_map;         //order_id, order_status
    std::multimap<int64_t, std::shared_ptr<TiRtnOrderStatus>>           m_order_batch_map;   //order_id, batch_order_status
    std::multimap<int64_t, std::shared_ptr<TiRtnOrderMatch>>            m_matches_map;       //order_id, m_matches_map

public:
    TiGtTraderAccount(TI_BrokerType broker_type, TI_AccountType account, TiTraderCallback* userCb);
    virtual ~TiGtTraderAccount();

public:
    void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str);     //非交易逻辑的统一实现接口
    
    void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){};
    void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

public:
    void enterOrder(std::shared_ptr<TiRtnOrderStatus> order);
    void enterBatchOrder(std::shared_ptr<TiRtnOrderStatus> order);
    
    TiRtnOrderStatus* getOrderStatus(int64_t order_id);
    TiRtnOrderStatus* getOrderStatus(int64_t order_id, std::string symbol);
    TiRtnOrderStatus* getOrderStatus(std::string order_stream_id);

};

#endif // TI_GT_TRADER_ACCOUNT_H