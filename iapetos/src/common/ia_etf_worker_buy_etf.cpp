#include "ia_etf_worker_buy_etf.h"
#include "ia_etf_price_tool.h"
#include "ti_quote_formater.h"
#include "datetime.h"

IaETFWorkerBuyEtf::IaETFWorkerBuyEtf(TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account)
    : IaETFTradingWorker(client, m_quote_cache, factor, account)
{
    m_status.symbol = factor->GetEtfInfo()->m_fundId;
    m_status.exchange = factor->GetEtfInfo()->m_exchange;
    m_status.volume = factor->GetEtfInfo()->m_minUnit;
    m_status.real_cost = 0;
    m_status.finish_volume = 0;
    m_status.wrong_number = 0;
    m_check_time = datetime::get_now_timestamp_ms();

    TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
        m_status.symbol.c_str(), 
        m_status.exchange.c_str());
    memcpy(&m_open_snap, snap, sizeof(TiQuoteSnapshotStockField));
    updateExpectCost(snap);
}

void IaETFWorkerBuyEtf::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    
};

void IaETFWorkerBuyEtf::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    if(isOver())
    {
        return;
    }
    auto iter = m_req_id_set.find(pData->nReqId);
    if (iter == m_req_id_set.end())
    {
        return;
    }
    if (pData->nStatus == TI_OrderStatusType_fail)
    {
        m_status.wrong_number++;
    }
    m_order_map[pData->nOrderId] = *pData;
    updateStatus();
};

void IaETFWorkerBuyEtf::OnTimer()
{
    if(isOver())
    {
        return;
    }
    int64_t now = datetime::get_now_timestamp_ms();
    if((now - m_check_time) >= 3000) //1s 检查一次
    {
        m_check_time = now;
        for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
        {
            if (iter->second.nStatus > 0 && iter->second.nDealtVol == 0)
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

void IaETFWorkerBuyEtf::updateExpectCost(TiQuoteSnapshotStockField* pData)
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

void IaETFWorkerBuyEtf::updateStatus()
{
    m_status.real_cost = 0;
    m_status.finish_volume = 0;

    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        m_status.real_cost += (double)iter->second.nDealtVol * iter->second.nDealtPrice;
        m_status.finish_volume += iter->second.nDealtVol;
    }
};


bool IaETFWorkerBuyEtf::hasQueueOrder()
{
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        if (iter->second.nStatus >= 0)
        {
            return true;
        }
    }
    return false;
};


int64_t IaETFWorkerBuyEtf::open()
{
    std::shared_ptr<IaEtfInfo> m_etf_info = m_etf_factor->GetEtfInfo();
    TiQuoteSnapshotStockField* etf_snap = m_quote_cache->GetStockSnapshot(m_etf_info->m_fundId.c_str(), m_etf_info->m_exchange.c_str());

    if (etf_snap == nullptr)
    {
        return -1;
    }
    
    updateExpectCost(etf_snap);
    double price = IaEtfPriceTool::get_order_price(
            m_status.volume, etf_snap->ask_price, etf_snap->ask_volume, TI_STOCK_ARRAY_LEN);
    double vol = m_status.volume - m_status.finish_volume;


    std::vector<int32_t> order_vol_vec;
    
    getSplitOrderVol(vol, order_vol_vec);

 //   /*
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
        req.nReqTimestamp = datetime::get_now_timestamp_ms();
        strcpy(req.szUseStr, "jager");

        m_client->orderInsert(&req);

        m_req_id_set.insert(req.nReqId);
    }
    return 0;
};

bool IaETFWorkerBuyEtf::isOver()
{
    if (m_status.finish_volume == m_status.volume)
    {
        return true;
    }
    if (m_status.wrong_number > 3)
    {
        return true;
    }
    return false;
};