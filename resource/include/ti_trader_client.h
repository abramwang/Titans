#ifndef TI_TRADER_CLIENT_H
#define TI_TRADER_CLIENT_H

#include "ti_trader_struct.h"

class TiTraderClient
{
public:
    virtual void connect() = 0;
    virtual int orderInsert(TiReqOrderInsert* req) = 0;
    virtual int orderDelete(TiReqOrderDelete* req) = 0;

public:
    virtual TiRtnOrderStatus* getOrderStatus(int64_t req_id, int64_t order_id) = 0;
public:
    TiTraderClient(){};
    virtual ~TiTraderClient(){};
};

#endif 