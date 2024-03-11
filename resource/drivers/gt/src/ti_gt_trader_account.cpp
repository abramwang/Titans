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

void TiGtTraderAccount::enterOrder(std::shared_ptr<TiRtnOrderStatus> order)
{
   m_order_map[order->nOrderId] = order;
   m_order_sys_map[order->szOrderStreamId] = order;
};

TiRtnOrderStatus* TiGtTraderAccount::getOrderStatus(int64_t order_id)
{
    auto iter = m_order_map.find(order_id);
    if (iter != m_order_map.end())
    {
        return iter->second.get();
    }
    std::shared_ptr<TiRtnOrderStatus> order_ptr = std::make_shared<TiRtnOrderStatus>();
    memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
    order_ptr->nOrderId = order_id;
    m_order_map[order_ptr->nOrderId] = order_ptr;
    return order_ptr.get();
};

TiRtnOrderStatus* TiGtTraderAccount::getOrderStatus(std::string order_stream_id)
{
    auto iter = m_order_sys_map.find(order_stream_id);
    if (iter != m_order_sys_map.end())
    {
        return iter->second.get();
    }
    std::shared_ptr<TiRtnOrderStatus> order_ptr = std::make_shared<TiRtnOrderStatus>();
    memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
    strcpy(order_ptr->szOrderStreamId, order_stream_id.c_str());
    m_order_sys_map[order_ptr->szOrderStreamId] = order_ptr;
    return order_ptr.get();
};