#include "ia_etf_fund_trading_redeeming_worker.h"
#include "datetime.h"
#include "ti_trader_formater.h"
#include "ia_etf_price_tool.h"
#include <iostream>
#include <random>

IaETFFundTradingRedeemingWorker::IaETFFundTradingRedeemingWorker(TiTraderClient* client, IaETFFactorOperator* factor_operator)
    : IaETFTradingWorker(client)
{
    m_factor_operator = factor_operator;
    memset(&m_signal_snap, 0, sizeof(TiQuoteSnapshotStockField));
    memcpy(&m_signal_snap, &factor_operator->m_etf_last_snap, sizeof(TiQuoteSnapshotStockField));

    m_redeeming_order = NULL;
    m_is_over = false;
    m_start_buy_time = 0;
    m_check_time = 0;
}

void IaETFFundTradingRedeemingWorker::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
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
            std::cout << "IaETFFundTradingRedeemingWorker::OnRtnOrderStatusEvent: dealt_vol " << dealt_vol << std::endl;
            if(dealt_vol == m_factor_operator->m_minUnit)
            {
                std::cout << "IaETFFundTradingRedeemingWorker start redeeming! " << std::endl;
                m_is_over = true;
                redeemingEtf();
            }
        }
    }

    //赎回订单
    if (pData->nBusinessType == TI_BusinessType_ETF)
    {
        //赎回
        if (m_redeeming_order)
        {
            if (m_redeeming_order->nReqId == pData->nReqId)
            {
                memcpy(m_redeeming_order, pData, sizeof(TiRtnOrderStatus));
                if (m_redeeming_order->nDealtVol == m_redeeming_order->nOrderVol)
                {
                    std::cout << "IaETFFundTradingRedeemingWorker redeeming work done " << std::endl;
                }
            }
        }
    }
};

void IaETFFundTradingRedeemingWorker::OnTimer()
{
    if(isEtfBuyOver())
    {
        return;
    }
    int64_t now = datetime::get_now_timestamp_ms();
    if((now - m_check_time) >= 1000) //1s 检查一次
    {
        m_check_time = now;
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
    }
    if (!hasQueueOrder())
    {
        BuyEtf(); 
    }
};

void IaETFFundTradingRedeemingWorker::getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec)
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

bool IaETFFundTradingRedeemingWorker::isEtfBuyOver()
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

int32_t IaETFFundTradingRedeemingWorker::getEtfBuyVol()
{
    int32_t dealt_vol = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {
        dealt_vol += m_order_vec[i]->nDealtVol;
    }
    return m_factor_operator->m_minUnit - dealt_vol;
};

bool IaETFFundTradingRedeemingWorker::hasQueueOrder()
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

void IaETFFundTradingRedeemingWorker::cancelEtfOrder()
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


void IaETFFundTradingRedeemingWorker::redeemingEtf()
{
    m_redeeming_order = new TiRtnOrderStatus();
    memset(m_redeeming_order, 0, sizeof(TiRtnOrderStatus));
    TiReqOrderInsert* order_req = m_redeeming_order;
    memset(order_req, 0, sizeof(TiReqOrderInsert));
    order_req->nTradeSideType = TI_TradeSideType_Redemption;
    order_req->nBusinessType = TI_BusinessType_ETF;
    strcpy(order_req->szExchange, m_factor_operator->m_etf_last_snap.exchange);
    strcpy(order_req->szSymbol, m_factor_operator->m_etf_last_snap.symbol);
    order_req->nOrderPrice = m_factor_operator->m_etf_last_snap.ask_price[0];
    order_req->nOrderVol = m_factor_operator->m_minUnit;
    order_req->nReqTimestamp = datetime::get_now_timestamp_ms();

    m_client->orderInsert(order_req);
    std::cout << "IaETFFundTradingRedeemingWorker::redeemingEtf" << order_req->nBusinessType << " " << m_redeeming_order->nReqId << std::endl;
};

void IaETFFundTradingRedeemingWorker::BuyEtf()
{
    m_start_buy_time  = datetime::get_now_timestamp_ms();

    std::vector<int32_t> order_vol_vec;
    getSplitOrderVol(getEtfBuyVol(), order_vol_vec);
    for (size_t i = 0; i < order_vol_vec.size(); i++)
    {
        TiRtnOrderStatus* order_req = new TiRtnOrderStatus();
        order_req->nTradeSideType = TI_TradeSideType_Buy;
        order_req->nBusinessType = TI_BusinessType_Stock;
        strcpy(order_req->szExchange, m_factor_operator->m_etf_last_snap.exchange);
        strcpy(order_req->szSymbol, m_factor_operator->m_etf_last_snap.symbol);
        order_req->nOrderPrice = IaEtfPriceTool::get_order_price(order_vol_vec[i], 
            m_signal_snap.ask_price, m_signal_snap.ask_volume, TI_STOCK_ARRAY_LEN);
        
        order_req->nOrderVol = order_vol_vec[i];
        order_req->nReqTimestamp = datetime::get_now_timestamp_ms();

        m_client->orderInsert(order_req);

        m_order_vec.push_back(order_req);
        m_order_req_map[order_req->nReqId] = order_req;
    }
};

bool IaETFFundTradingRedeemingWorker::IsOver()
{
    if (m_is_over)
    {
        if (m_redeeming_order)
        {
            if (m_redeeming_order->nDealtVol == m_factor_operator->m_minUnit)
            {
                return true;
            }
        }
    }
    return false;
};

json IaETFFundTradingRedeemingWorker::GetOrderArrayJson()
{
    json j = json::array();
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        j.push_back(iter->second->nOrderId);
    }
    if (m_redeeming_order)
    {
        j.push_back(m_redeeming_order->nOrderId);
    }
    return j;
};

