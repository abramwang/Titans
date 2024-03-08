#include "ti_gt_trader_account.h"

#include <iostream>

TiGtTraderAccount::TiGtTraderAccount(TI_BrokerType broker_type, TI_AccountType account, TiTraderCallback* userCb)
{
    m_cb = userCb;

};

TiGtTraderAccount::~TiGtTraderAccount()
{

};


void TiGtTraderAccount::OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str)
{
    std::cout << *rspData << std::endl;
};


int TiGtTraderAccount::enterOrder(const TiReqOrderInsert* req)
{
    std::shared_ptr<TiRtnOrderStatus> order = std::make_shared<TiRtnOrderStatus>();
    memcpy(order.get(), req, sizeof(TiReqOrderInsert));

    m_order_req_map[req->nReqId] = order;
    return 0;
};

TiRtnOrderStatus* TiGtTraderAccount::getOrderStatus(int64_t req_id, int64_t order_id)
{
    if (order_id != 0)
    {
        auto iter = m_order_map.find(order_id);
        if (iter != m_order_map.end())
        {
            return iter->second.get();
        }
    }
    auto iter = m_order_req_map.find(req_id);
    if (iter != m_order_req_map.end())
    {
        return iter->second.get();
    }
    return NULL;
};