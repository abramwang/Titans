#ifndef TI_TRADER_CLIENT_H
#define TI_TRADER_CLIENT_H

#include "ti_trader_struct.h"
#include <vector>
#include <string>

class TiTraderClient
{
public:
    virtual void connect() = 0;
    virtual int orderInsertBatch(std::vector<TiReqOrderInsert> &req_vec, std::string account_id) = 0;
    virtual int orderInsert(TiReqOrderInsert* req) = 0;
    virtual int orderDelete(TiReqOrderDelete* req) = 0;

public:
    virtual TiRtnOrderStatus* getOrderStatus(int64_t req_id, int64_t order_id) = 0;

    virtual int QueryAsset() = 0;
    virtual int QueryOrders() = 0;
    virtual int QueryMatches() = 0;
    virtual int QueryPositions() = 0;
public:
    TiTraderClient(){};
    virtual ~TiTraderClient(){};
};

#endif 