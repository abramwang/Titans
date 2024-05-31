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
    m_status.wrong_number = 0;
    m_status.is_limit = false;
    m_check_time = datetime::get_now_timestamp_ms();

    TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
        m_status.symbol.c_str(), 
        m_status.exchange.c_str());
    memcpy(&m_open_snap, snap, sizeof(TiQuoteSnapshotStockField));
    memset(&m_canceling_order_info, 0, sizeof(DeleteOrderReqInfo));
}

void IaETFWorkerSingleStock::OnRspOrderDelete(const TiRspOrderDelete* pData)
{

};

void IaETFWorkerSingleStock::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
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
    /*
    if (m_status.symbol != pData->szSymbol || m_status.exchange != pData->szExchange)
    {
        return;
    }
    */
    if (m_canceling_order_info.nOrderId)
    {
        if (m_canceling_order_info.nOrderId == pData->nOrderId)
        {
            memset(&m_canceling_order_info, 0, sizeof(DeleteOrderReqInfo));
        }
        if (strcmp(m_canceling_order_info.szOrderStreamId, pData->szOrderStreamId) == 0)
        {
            memset(&m_canceling_order_info, 0, sizeof(DeleteOrderReqInfo));
        }
    }
    if (pData->nStatus == TI_OrderStatusType_fail)
    {
        m_status.wrong_number++;
    }
    m_order_map[pData->nOrderId] = *pData;
    updateStatus();
};

void IaETFWorkerSingleStock::OnTimer()
{
    if(isOver())
    {
        return;
    }
    if (m_canceling_order_info.nOrderId)
    {
        return;
    }
    int64_t now = datetime::get_now_timestamp_ms();
    if((now - m_check_time) >= 5000) //5s 检查一次
    {
        m_check_time = now;
        
        if (!hasQueueOrder())
        {
            open(); 
        }else{
            for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
            {
                if (iter->second.nStatus > 0 && iter->second.nDealtVol == 0)
                {
                    cancelOrder(&iter->second);
                    break;
                }
            }
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
    m_status.real_cost = 0;
    m_status.finish_volume = 0;

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
            /*
            std::cout << "[IaETFWorkerSingleStock::hasQueueOrder]: " << m_status.symbol 
                << ", order_id " << iter->second.nOrderId
                << ", status " << iter->second.nStatus
                << std::endl;
            */
            return true;
        }
    }
    return false;
};

void IaETFWorkerSingleStock::cancelOrder(TiRtnOrderStatus* order)
{
    TiReqOrderDelete req;
    memset(&req, 0, sizeof(TiReqOrderDelete));
    req.nOrderId = order->nOrderId;
    strcpy(req.szOrderStreamId, order->szOrderStreamId);
    strcpy(req.szAccount,order->szAccount);


    memset(&m_canceling_order_info, 0, sizeof(DeleteOrderReqInfo));
    m_canceling_order_info.nOrderId = order->nOrderId;
    strcpy(m_canceling_order_info.szOrderStreamId, order->szOrderStreamId);
    strcpy(m_canceling_order_info.szAccount,order->szAccount);

    m_client->orderDelete(&req);
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
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, m_status.symbol.c_str());
    strcpy(req.szExchange, m_status.exchange.c_str());
    strcpy(req.szAccount, m_account.c_str());
    req.nTradeSideType = m_side;
    req.nBusinessType = TI_BusinessType_Stock;
    req.nOffsetType = TI_OffsetType_Open;
    if (m_side == TI_TradeSideType_Buy)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(
            m_status.volume, snap->ask_price, snap->ask_volume, TI_STOCK_ARRAY_LEN, snap->high_limit);
        m_status.is_limit = (req.nOrderPrice == snap->high_limit);
    }
    if (m_side == TI_TradeSideType_Sell)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(
            m_status.volume, snap->bid_price, snap->bid_volume, TI_STOCK_ARRAY_LEN, snap->low_limit);
        m_status.is_limit = (req.nOrderPrice == snap->low_limit);
    }
    req.nOrderVol = vol;
    req.nReqTimestamp = datetime::get_now_timestamp_ms();
    strcpy(req.szUseStr, "jager");

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
    if (m_status.is_limit)
    {
        return true;
    }
    if (m_status.finish_volume == m_status.volume)
    {
        return true;
    }
    if (m_status.wrong_number > 3)
    {
        return true;
    }
    /*
    std::cout << "[IaETFWorkerSingleStock::isOver]: " << m_status.symbol 
        << ", finish_volume " << m_status.finish_volume
        << ", volume " << m_status.volume
        << ", hasQueueOrder " << hasQueueOrder()
        << ", wrong_number " << m_status.wrong_number
        << std::endl;
    //*/
    return false;
};
