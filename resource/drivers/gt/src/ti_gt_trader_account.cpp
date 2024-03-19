#include "ti_gt_trader_account.h"
#include "datetime.h"
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

void TiGtTraderAccount::enterOrder(std::shared_ptr<TiRtnOrderStatus> order_ptr)
{
    m_order_map[order_ptr->nOrderId] = order_ptr;
    m_order_sys_map[order_ptr->szOrderStreamId] = order_ptr;
};

void TiGtTraderAccount::enterBatchOrder(std::shared_ptr<TiRtnOrderStatus> order_ptr)
{
    m_order_batch_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderStatus>>(order_ptr->nOrderId, order_ptr));
};

void TiGtTraderAccount::enterMatch(std::shared_ptr<TiRtnOrderMatch> match_ptr)
{
    m_matches_map.insert(std::pair<std::string, std::shared_ptr<TiRtnOrderMatch>>(match_ptr->szStreamId, match_ptr));
};

TiRtnOrderStatus* TiGtTraderAccount::getOrderStatus(int64_t order_id)
{
    auto iter = m_order_map.find(order_id);
    if (iter != m_order_map.end())
    {
        return iter->second.get();
    }
    return NULL;
};


TiRtnOrderStatus* TiGtTraderAccount::getOrderStatus(int64_t order_id, std::string symbol)
{
    auto range = m_order_batch_map.equal_range(order_id);
    for (auto it = range.first; it != range.second; ++it) {
        if (it->second->szSymbol == symbol)
        {
            return it->second.get();
        }
    }
    return NULL;
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
    order_ptr->nInsertTimestamp = datetime::get_now_timestamp_ms();
    m_order_sys_map[order_ptr->szOrderStreamId] = order_ptr;
    return order_ptr.get();
};