#include "ia_etf_trading_worker.h"
#include "datetime.h"

IaETFTradingWorker::IaETFTradingWorker(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache,IaEtfSignalFactorPtr factor, std::string account){
    m_quote_cache = quote_cache;
    m_client = client;
    m_etf_factor = factor;
    m_account = account;
    m_pre_worker = nullptr;


    m_status.exchange = "";
    m_status.symbol = "";
    m_status.volume = 0;
    m_status.real_cost = 0;
    m_status.finish_volume = 0;
    m_status.wrong_number = 0;
    m_status.is_limit = false;
    m_check_time = datetime::get_now_timestamp_ms();
    memset(&m_canceling_order_info, 0, sizeof(DeleteOrderReqInfo));
};


void IaETFTradingWorker::getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec)
{
    int32_t min_vol = 100;
    int32_t max_vol = 300000;

    int32_t left_vol = order_vol;

    while (left_vol > max_vol)
    {
        int32_t random = rand() % max_vol + 1;
        int32_t vol = (random /min_vol )*  min_vol;
        if (vol > left_vol)
        {
            break;
        }else{
            order_vol_vec.push_back(vol);
            left_vol -= vol;
        }
    }
    if (left_vol)
    {
        order_vol_vec.push_back(left_vol);
    }
};


void IaETFTradingWorker::updateExpectCost(TiQuoteSnapshotStockField* pData)
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

void IaETFTradingWorker::updateStatus()
{
    m_status.real_cost = 0;
    m_status.finish_volume = 0;

    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
    {
        m_status.real_cost += (double)iter->second.nDealtVol * iter->second.nDealtPrice;
        m_status.finish_volume += iter->second.nDealtVol;
    }
};

bool IaETFTradingWorker::hasQueueOrder()
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

void IaETFTradingWorker::cancelOrder(TiRtnOrderStatus* order)
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

//////////////////////////////////////////////////////////////////////////////////////////////////
/// 回调函数
//////////////////////////////////////////////////////////////////////////////////////////////////

void IaETFTradingWorker::OnRspOrderDelete(const TiRspOrderDelete* pData)
{

};

void IaETFTradingWorker::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
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
    ///*
    if (strcmp(m_status.symbol.c_str(), pData->szSymbol) != 0)
    {
        return;
    }
    //*/
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

void IaETFTradingWorker::OnTimer()
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

bool IaETFTradingWorker::isOver()
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
    return false;
};