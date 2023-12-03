#include "ia_etf_single_stock_trading_worker.h"
#include "ia_etf_price_tool.h"
#include "ti_trader_formater.h"
#include "datetime.h"

IaETFSingleStockTradingWorker::IaETFSingleStockTradingWorker(IaETFConstituentCache* cache, TI_TradeSideType side, TiTraderClient* client)
    : IaETFTradingWorker(client)
{
    m_cache = cache;
    m_side = side;
    m_req_id = 0;
    m_status.exchange = m_cache->szExchange;
    m_status.symbol = m_cache->m_last_snap.symbol;
    m_status.expect_cost = (m_side == TI_TradeSideType_Buy) ? m_cache->nBuyCost : m_cache->nSellCost;
    m_status.volume = m_cache->m_info.m_disclosure_vol;
    m_status.real_cost = 0;
    m_status.finish_volume = 0;
    m_check_time = datetime::get_now_timestamp_ms();

    memcpy(&m_open_snap, &m_cache->m_last_snap, sizeof(TiQuoteSnapshotStockField));
    open();
}

void IaETFSingleStockTradingWorker::OnRspOrderDelete(const TiRspOrderDelete* pData)
{

};

void IaETFSingleStockTradingWorker::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    if (pData->nReqId == m_req_id)
    {
        m_order_map[pData->nOrderId] = *pData;
        m_req_id = 0;
        updateStatus();
        return;
    }
    auto iter = m_order_map.find(pData->nOrderId);
    if (iter != m_order_map.end())
    {
        iter->second = *pData;
        updateStatus();
        return;
    }
};


void IaETFSingleStockTradingWorker::OnTimer()
{
    if(isOver())
    {
        return;
    }
    int64_t now = datetime::get_now_timestamp_ms();
    updateStatus();
    if((now - m_check_time) >= 1000) //1s 检查一次
    {
        m_check_time = now;
        for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
        {
            if (iter->second.nStatus > 0)
            {
                TiReqOrderDelete req;
                memset(&req, 0, sizeof(TiReqOrderDelete));
                req.nOrderId = iter->second.nOrderId;
                m_client->orderDelete(&req);
            }
        }
    }
    if (!hasQueueOrder())
    {
        open(); 
    }
};

void IaETFSingleStockTradingWorker::updateStatus()
{
    m_status.finish_volume = 0;
    m_status.real_cost = 0;
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
    {
        m_status.finish_volume += iter->second.nDealtVol;
        m_status.real_cost += (double)iter->second.nDealtVol * iter->second.nDealtPrice;
    }
};

bool IaETFSingleStockTradingWorker::hasQueueOrder()
{
    if(m_req_id)
    {
        return true;
    }
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
    {
        if(iter->second.nStatus >= 0)
        {
            return true;
        }
    }
    return false;
};

int64_t IaETFSingleStockTradingWorker::open()
{
    TiReqOrderInsert req;
    req.nTradeSideType = m_side;
    req.nBusinessType = TI_BusinessType_Stock;
    strcpy(req.szExchange, m_cache->szExchange);
    strcpy(req.szSymbol, m_cache->m_last_snap.symbol);
    if (m_side == TI_TradeSideType_Buy)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(m_cache->m_info.m_disclosure_vol, m_cache->m_last_snap.ask_price, m_cache->m_last_snap.ask_volume, TI_STOCK_ARRAY_LEN);
    }
    if (m_side == TI_TradeSideType_Sell)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(m_cache->m_info.m_disclosure_vol, m_cache->m_last_snap.bid_price, m_cache->m_last_snap.bid_volume, TI_STOCK_ARRAY_LEN);
    }
    req.nOrderVol = m_cache->m_info.m_disclosure_vol - m_status.finish_volume;
    req.nReqTimestamp = datetime::get_now_timestamp_ms();

    m_client->orderInsert(&req);

    m_req_id = req.nReqId;
    return req.nReqId;
};

json IaETFSingleStockTradingWorker::getStatusJson()
{
    json j = {
        {"symbol", m_status.symbol},
        {"exchange", m_status.exchange},
        {"expect_cost", m_status.expect_cost},
        {"volume", m_status.volume},
        {"real_cost", json::number_float_t(m_status.real_cost)},
        {"finish_volume", json::number_integer_t(m_status.finish_volume)},
        {"orders", json::array()}
    };
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        j["orders"].push_back(iter->second.nOrderId);
    }
    return j;
};

bool IaETFSingleStockTradingWorker::isOver()
{
    return m_status.finish_volume == m_status.volume;
};
