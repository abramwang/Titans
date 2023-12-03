#ifndef TI_TRADER_CALLBACK_H
#define TI_TRADER_CALLBACK_H

#include "ti_trader_struct.h"
#include <nlohmann/json.hpp>
using namespace nlohmann;

class TiTraderCallback
{
public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str) = 0;     //非交易逻辑的统一实现接口
    
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData) = 0;
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast) = 0;
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast) = 0;
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast) = 0;

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData) = 0;
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData) = 0;
public:
    TiTraderCallback(){};
    virtual ~TiTraderCallback(){};
};

#endif 