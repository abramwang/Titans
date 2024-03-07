#ifndef TI_GT_TRADER_ACCOUNT_H
#define TI_GT_TRADER_ACCOUNT_H

#include "ti_trader_callback.h"

class TiGtTraderAccount: 
    public TiTraderCallback
{
private:
    TiTraderCallback* m_cb;

public:
    TiGtTraderAccount(TI_BrokerType broker_type, TI_AccountType account, TiTraderCallback* userCb);
    virtual ~TiGtTraderAccount();

public:
    void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){};     //非交易逻辑的统一实现接口
    
    void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){};
    void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

};

#endif // TI_GT_TRADER_ACCOUNT_H