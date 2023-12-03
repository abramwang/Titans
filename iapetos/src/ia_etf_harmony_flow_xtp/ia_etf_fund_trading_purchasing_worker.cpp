#include "ia_etf_fund_trading_purchasing_worker.h"
#include "datetime.h"
#include "ti_trader_formater.h"
#include "ia_etf_price_tool.h"
#include <iostream>
#include <random>

IaETFFundTradingPurchasingWorker::IaETFFundTradingPurchasingWorker(TiTraderClient* client, IaETFFactorOperator* factor_operator, TiQuoteSnapshotStockField *signal_snap)
    : IaETFTradingWorker(client)
{
    m_factor_operator = factor_operator;
    memset(&m_signal_snap, 0, sizeof(TiQuoteSnapshotStockField));
    memcpy(&m_signal_snap, signal_snap, sizeof(TiQuoteSnapshotStockField));

    m_purchasing_order = NULL;
    m_is_over = false;
    m_start_sell_time = 0;
}

void IaETFFundTradingPurchasingWorker::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    if (isEtfSellOver())
    {
        return;
    }
    //申赎订单
    if (pData->nBusinessType == TI_BusinessType_ETF)
    {
        //申购
        if (m_purchasing_order)
        {
            if (m_purchasing_order->nReqId == pData->nReqId && m_purchasing_order->nStatus >= 0)
            {
                memcpy(m_purchasing_order, pData, sizeof(TiRtnOrderStatus));
                json j;
                TiTraderFormater::FormatOrderStatus(pData, j);
                std::cout << "IaETFFundTradingPurchasingWorker::OnRtnOrderStatusEvent: " << j << std::endl;

                if (m_purchasing_order->nDealtVol == m_purchasing_order->nOrderVol)
                {
                    std::cout << "IaETFFundTradingPurchasingWorker:: start sell etf! " << std::endl;
                    sellEtf();
                }
            }
        }
    }
    //普通订单
    if (pData->nBusinessType == TI_BusinessType_Stock)
    {
        bool is_order_exist = false;
        auto iter = m_order_map.find(pData->nOrderId);
        if (iter != m_order_map.end())
        {
            memcpy(iter->second, pData, sizeof(TiRtnOrderStatus));
            is_order_exist = true;
        }else{
            iter = m_order_req_map.find(pData->nReqId);
            if (iter != m_order_req_map.end())
            {
                memcpy(iter->second, pData, sizeof(TiRtnOrderStatus));
                m_order_map[pData->nOrderId] = iter->second;
                is_order_exist = true;
            }
        }

        if (is_order_exist)
        {
            int32_t dealt_vol = 0;
            for (size_t i = 0; i < m_order_vec.size(); i++)
            {
                dealt_vol += m_order_vec[i]->nDealtVol;
            }
            std::cout << "IaETFFundTradingPurchasingWorker::OnRtnOrderStatusEvent: dealt_vol " << dealt_vol << std::endl;
            if(dealt_vol == m_factor_operator->m_minUnit)
            {
                std::cout << "IaETFFundTradingPurchasingWorker work done " << std::endl;
                m_is_over = true;
            }
        }
    }
};

void IaETFFundTradingPurchasingWorker::OnTimer()
{
    if (isEtfSellOver())
    {
        return;
    }
    int64_t now = datetime::get_now_timestamp_ms();
    //回撤过大强平
    double signal_cost_price = m_signal_snap.bid_price[0];
    int32_t left_vol = getEtfSellVol();
    double current_price = IaEtfPriceTool::get_order_price(left_vol, 
            m_factor_operator->m_etf_last_snap.bid_price, m_factor_operator->m_etf_last_snap.bid_volume, TI_STOCK_ARRAY_LEN);
    
    bool close_flag = false;
    if (current_price < signal_cost_price * 0.996)  //千4 强平
    {
        close_flag = true;
    }
    //超时强平
    if ((now - m_start_sell_time) > 1000 * 60 * 5) //5分钟
    {
        close_flag = true;
    }
    if (close_flag)
    {
        cancelEtfOrder();
    }
    if (!hasQueueOrder())
    {
        if (close_flag)
        {
            sellEtfClose(); 
        }else{
            sellEtf();
        }
    }
};

void IaETFFundTradingPurchasingWorker::getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec)
{
    int32_t min_vol = 100;
    int32_t max_vol = 1000000;

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

bool IaETFFundTradingPurchasingWorker::isEtfSellOver()
{   
    if (m_is_over)
    {
        return true;
    }
    int32_t dealt_vol = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {
        dealt_vol += m_order_vec[i]->nDealtVol;
    }
    if(dealt_vol == m_factor_operator->m_minUnit)
    {
        m_is_over = true;
        return true;
    }else{
        return false;
    }
};

int32_t IaETFFundTradingPurchasingWorker::getEtfSellVol()
{
    int32_t dealt_vol = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {
        dealt_vol += m_order_vec[i]->nDealtVol;
    }
    return m_factor_operator->m_minUnit - dealt_vol;
};

bool IaETFFundTradingPurchasingWorker::hasQueueOrder()
{
    auto iter = m_order_vec.begin();
    for (; iter != m_order_vec.end(); ++iter)
    {
        if ((*iter)->nStatus >= 0)
        {
            return true;
        }
    }
    return false;
};

void IaETFFundTradingPurchasingWorker::cancelEtfOrder()
{
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
    {
        if (iter->second->nStatus > 0)
        {
            TiReqOrderDelete req;
            memset(&req, 0, sizeof(TiReqOrderDelete));
            req.nOrderId = iter->second->nOrderId;
            m_client->orderDelete(&req);
        }
    }
};

void IaETFFundTradingPurchasingWorker::sellEtf()
{
    std::vector<int32_t> order_vol_vec;
    getSplitOrderVol(m_factor_operator->m_minUnit, order_vol_vec);
    for (size_t i = 0; i < order_vol_vec.size(); i++)
    {
        TiRtnOrderStatus* order_req = new TiRtnOrderStatus();
        order_req->nTradeSideType = TI_TradeSideType_Sell;
        order_req->nBusinessType = TI_BusinessType_Stock;
        strcpy(order_req->szExchange, m_factor_operator->m_etf_last_snap.exchange);
        strcpy(order_req->szSymbol, m_factor_operator->m_etf_last_snap.symbol);
        order_req->nOrderPrice = m_signal_snap.bid_price[0];
        order_req->nOrderVol = order_vol_vec[i];
        order_req->nReqTimestamp = datetime::get_now_timestamp_ms();

        m_client->orderInsert(order_req);

        m_order_vec.push_back(order_req);
        m_order_req_map[order_req->nReqId] = order_req;
    }
};

void IaETFFundTradingPurchasingWorker::sellEtfClose()
{
    std::vector<int32_t> order_vol_vec;
    getSplitOrderVol(getEtfSellVol(), order_vol_vec);
    for (size_t i = 0; i < order_vol_vec.size(); i++)
    {
        TiRtnOrderStatus* order_req = new TiRtnOrderStatus();
        order_req->nTradeSideType = TI_TradeSideType_Sell;
        order_req->nBusinessType = TI_BusinessType_Stock;
        strcpy(order_req->szExchange, m_factor_operator->m_etf_last_snap.exchange);
        strcpy(order_req->szSymbol, m_factor_operator->m_etf_last_snap.symbol);
        order_req->nOrderPrice = IaEtfPriceTool::get_order_price(order_vol_vec[i], 
            m_factor_operator->m_etf_last_snap.bid_price, m_factor_operator->m_etf_last_snap.bid_volume, TI_STOCK_ARRAY_LEN);
        order_req->nOrderVol = order_vol_vec[i];
        order_req->nReqTimestamp = datetime::get_now_timestamp_ms();

        m_client->orderInsert(order_req);

        m_order_vec.push_back(order_req);
        m_order_req_map[order_req->nReqId] = order_req;
    }
};

void IaETFFundTradingPurchasingWorker::PurchasingEtf()
{
    m_purchasing_order = new TiRtnOrderStatus();
    memset(m_purchasing_order, 0, sizeof(TiRtnOrderStatus));
    TiReqOrderInsert* order_req = m_purchasing_order;
    order_req->nTradeSideType = TI_TradeSideType_Purchase;
    order_req->nBusinessType = TI_BusinessType_ETF;
    strcpy(order_req->szExchange, m_factor_operator->m_etf_last_snap.exchange);
    strcpy(order_req->szSymbol, m_factor_operator->m_etf_last_snap.symbol);
    order_req->nOrderPrice = IaEtfPriceTool::get_order_price(m_factor_operator->m_minUnit, 
        m_factor_operator->m_etf_last_snap.ask_price, m_factor_operator->m_etf_last_snap.ask_volume, TI_STOCK_ARRAY_LEN);
   
    order_req->nOrderVol = m_factor_operator->m_minUnit;
    order_req->nReqTimestamp = datetime::get_now_timestamp_ms();

    m_client->orderInsert(order_req);
};

json IaETFFundTradingPurchasingWorker::GetOrderArrayJson()
{
    json j = json::array();
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        j.push_back(iter->second->nOrderId);
    }
    if (m_purchasing_order)
    {
        j.push_back(m_purchasing_order->nOrderId);
    }
    return j;
};