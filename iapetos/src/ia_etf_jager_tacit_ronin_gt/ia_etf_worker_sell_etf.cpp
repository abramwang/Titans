#include "ia_etf_worker_sell_etf.h"

#include "ti_quote_formater.h"


IaETFWorkerSellEtf::IaETFWorkerSellEtf(TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account)
    : IaETFTradingWorker(client, m_quote_cache, factor, account)
{
    m_status.symbol = factor->GetEtfInfo()->m_fundId;
    m_status.exchange = factor->GetEtfInfo()->m_exchange;
    m_status.volume = factor->GetEtfInfo()->m_minUnit;

}

void IaETFWorkerSellEtf::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    
};

void IaETFWorkerSellEtf::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
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

void IaETFWorkerSellEtf::OnTimer()
{

};

void IaETFWorkerSellEtf::updateExpectCost(TiQuoteSnapshotStockField* pData)
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

void IaETFWorkerSellEtf::updateStatus()
{
    m_status.real_cost = 0;
    m_status.finish_volume = 0;

    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        m_status.real_cost += iter->second.nOrderPrice * iter->second.nOrderVol;
        m_status.finish_volume += iter->second.nOrderVol;
    }
};


bool IaETFWorkerSellEtf::hasQueueOrder()
{
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        if (iter->second.nStatus > 0)
        {
            return true;
        }
    }
    return false;
};

double IaETFWorkerSellEtf::getOrderPrice(TiQuoteSnapshotStockField* pData)
{
    double price = pData->ask_price[2];
    
    if (price == 0)
    {
        price = pData->ask_price[0];
    }
    
    if (price == 0)
    {
        price = pData->last;
    }

    return price;
};



int64_t IaETFWorkerSellEtf::open()
{
    std::shared_ptr<IaEtfInfo> m_etf_info = m_etf_factor->GetEtfInfo();
    TiQuoteSnapshotStockField* etf_snap = m_quote_cache->GetStockSnapshot(m_etf_info->m_fundId.c_str(), m_etf_info->m_exchange.c_str());

    if (etf_snap == nullptr)
    {
        return -1;
    }
    
    memcpy(&m_open_snap, etf_snap, sizeof(TiQuoteSnapshotStockField));
    updateExpectCost(etf_snap);
    double price = getOrderPrice(etf_snap);


    std::vector<int32_t> order_vol_vec;
    
    getSplitOrderVol(m_status.volume, order_vol_vec);

    /*
    for (size_t i = 0; i < order_vol_vec.size(); i++)
    {
        TiReqOrderInsert req;
        memset(&req, 0, sizeof(TiReqOrderInsert));
        strcpy(req.szSymbol, m_etf_info->m_fundId.c_str());
        strcpy(req.szExchange, m_etf_info->m_exchange.c_str());
        strcpy(req.szAccount, m_account.c_str());
        req.nTradeSideType = TI_TradeSideType_Buy;
        req.nBusinessType = TI_BusinessType_Stock;
        req.nOffsetType = TI_OffsetType_Open;
        req.nOrderPrice = price;
        req.nOrderVol = order_vol_vec[i];
        strcpy(req.szUseStr, "jager");

        m_client->orderInsert(&req);

        m_req_id_set.insert(req.nReqId);
    }
    */

//    /*
    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, m_etf_info->m_fundId.c_str());
    strcpy(req.szExchange, m_etf_info->m_exchange.c_str());
    strcpy(req.szAccount, m_account.c_str());
    req.nTradeSideType = TI_TradeSideType_Sell;
    req.nBusinessType = TI_BusinessType_Stock;
    req.nOffsetType = TI_OffsetType_Open;
    req.nOrderPrice = price;
    req.nOrderVol = m_etf_info->m_minUnit;
    strcpy(req.szUseStr, "jager");

    m_client->orderInsert(&req);
    m_req_id_set.insert(req.nReqId);
    return req.nReqId;
//    */
    return 0;
};

bool IaETFWorkerSellEtf::isOver()
{
    if (m_status.finish_volume == m_status.volume && !hasQueueOrder())
    {
        return true;
    }
    return false;
};