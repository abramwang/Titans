#include "ia_etf_worker_signle_stock.h"
#include "ia_etf_price_tool.h"
#include "ti_trader_formater.h"
#include "datetime.h"

IaETFWorkerSingleStock:: IaETFWorkerSingleStock(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache, IaEtfSignalFactorPtr factor, std::string account, 
        std::shared_ptr<IaEtfConstituentInfo> constituentInfoPtr, TI_TradeSideType side)
    : IaETFTradingWorker(client, quote_cache, factor, account)
{
    m_side = side;
    m_status.exchange = constituentInfoPtr->m_exchange;
    m_status.symbol = constituentInfoPtr->m_symbol;
    m_status.volume = constituentInfoPtr->m_disclosure_vol;
    m_status.real_cost = 0;
    m_status.finish_volume = 0;
    m_check_time = datetime::get_now_timestamp_ms();

    TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
        m_status.symbol.c_str(), 
        m_status.exchange.c_str());
    memcpy(&m_open_snap, snap, sizeof(TiQuoteSnapshotStockField));
}

void IaETFWorkerSingleStock::OnRspOrderDelete(const TiRspOrderDelete* pData)
{

};

void IaETFWorkerSingleStock::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    auto iter = m_order_map.find(pData->szOrderStreamId);
    if (iter != m_order_map.end())
    {
        m_order_map[pData->szOrderStreamId] = *pData;
    }
    if (m_req_id_set.find(pData->nReqId) != m_req_id_set.end())
    {
        m_order_map[pData->szOrderStreamId] = *pData;
    }
    updateStatus();
};

void IaETFWorkerSingleStock::OnTimer()
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
                strcpy(req.szOrderStreamId, iter->second.szOrderStreamId);
                strcpy(req.szAccount, iter->second.szAccount);
                m_client->orderDelete(&req);
            }
        }
        if (!hasQueueOrder())
        {
            open(); 
        }
    }
};

void IaETFWorkerSingleStock::updateExpectCost(TiQuoteSnapshotStockField* pData)
{
    double price = pData->ask_price[0];

    if (price == 0)
    {
        price = pData->bid_price[0];
    }

    if (price == 0)
    {
        price = pData->last;
    }

    m_status.expect_cost = m_status.volume * price;
};

void IaETFWorkerSingleStock::updateStatus()
{
    m_status.finish_volume = 0;
    m_status.real_cost = 0;
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
    {
        m_status.real_cost += (double)iter->second.nDealtVol * iter->second.nDealtPrice;
        m_status.finish_volume += iter->second.nDealtVol;
    }
};

bool IaETFWorkerSingleStock::hasQueueOrder()
{
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
    {
        if(iter->second.nStatus >= 0)
        {
            return true;
        }
    }
    return false;
};

int64_t IaETFWorkerSingleStock::open()
{
    TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
        m_status.symbol.c_str(), 
        m_status.exchange.c_str());

    if (snap == nullptr)
    {
        return -1;
    }

    
    double vol = m_status.volume - m_status.finish_volume;

    TiReqOrderInsert req;
    req.nTradeSideType = m_side;
    req.nBusinessType = TI_BusinessType_Stock;
    strcpy(req.szExchange, m_status.exchange.c_str());
    strcpy(req.szSymbol, m_status.symbol.c_str());
    strcpy(req.szAccount, m_account.c_str());
    if (m_side == TI_TradeSideType_Buy)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(
            m_status.volume, snap->ask_price, snap->ask_volume, TI_STOCK_ARRAY_LEN);
    }
    if (m_side == TI_TradeSideType_Sell)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(
            m_status.volume, snap->bid_price, snap->bid_volume, TI_STOCK_ARRAY_LEN);
    }
    req.nOrderVol = vol;
    strcpy(req.szUseStr, "jager");
    req.nReqTimestamp = datetime::get_now_timestamp_ms();

    m_client->orderInsert(&req);

    m_req_id_set.insert(req.nReqId);
    return req.nReqId;
};

json IaETFWorkerSingleStock::getStatusJson()
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

bool IaETFWorkerSingleStock::isOver()
{
    if (m_status.finish_volume == m_status.volume && !hasQueueOrder())
    {
        return true;
    }
    return false;
};
