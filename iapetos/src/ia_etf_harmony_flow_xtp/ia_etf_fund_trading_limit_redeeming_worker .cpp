#include "ia_etf_fund_trading_limit_redeeming_worker.h"
#include "datetime.h"
#include "ti_trader_formater.h"
#include "ia_etf_price_tool.h"
#include <iostream>
#include <random>

IaETFFundTradingLimitRedeemingWorker::IaETFFundTradingLimitRedeemingWorker(TiTraderClient* client, IaETFFactorOperator* factor_operator)
    : IaETFTradingWorker(client)
{
    m_factor_operator = factor_operator;
    memset(&m_signal_snap, 0, sizeof(TiQuoteSnapshotStockField));
    memcpy(&m_signal_snap, &factor_operator->m_etf_last_snap, sizeof(TiQuoteSnapshotStockField));
    memset(&m_order_book, 0, sizeof(TiQuoteOrderBookField));
    m_etf_init_price = IaETFTradingSignalFilter::get_limit_redeeming_price(m_factor_operator);

    m_redeeming_order = NULL;
    m_is_redeeming = false;
    m_is_over = false;
    m_redemption_failures = 0;
    m_expected_redeeming_profit = 0;
    m_start_buy_time = 0;
    m_check_time = 0;
    m_queue_order_seq = 0;
    m_is_wait_model = true;
}

void IaETFFundTradingLimitRedeemingWorker::OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData)
{
    if (m_is_over)
    {
        return;
    }
    
    memcpy(&m_order_book, pData, sizeof(TiQuoteOrderBookField));
    
    periodicProfitCheck();
};

void IaETFFundTradingLimitRedeemingWorker::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    auto set_iter = m_cancel_order_set.find(pData->nOrderId);
    if (set_iter != m_cancel_order_set.end())
    {
        m_cancel_order_set.erase(set_iter);
    }
};

void IaETFFundTradingLimitRedeemingWorker::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
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
            std::cout << "IaETFFundTradingLimitRedeemingWorker::OnRtnOrderStatusEvent: dealt_vol " << dealt_vol << std::endl;
            
            if(dealt_vol == m_factor_operator->m_minUnit)
            {
                std::cout << "IaETFFundTradingLimitRedeemingWorker start watching redeeming! " << std::endl;
                //redeemingEtf();
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
                m_is_redeeming = false;
                if (m_redeeming_order->nDealtVol == m_redeeming_order->nOrderVol)
                {
                    m_is_over = true;
                    std::cout << "IaETFFundTradingLimitRedeemingWorker redeeming work done " << std::endl;
                }else{
                    if (m_redeeming_order->nStatus < 0)
                    {
                        m_redemption_failures ++;
                        if (m_redemption_failures < 5)
                        {
                            redeemingEtf();
                        }
                    }
                }
            }
        }
    }
};

void IaETFFundTradingLimitRedeemingWorker::OnTimer()
{   
    if (m_is_over)
    {
        return;
    }
    if(isEtfBuyOver())
    {
        if (m_is_redeeming)
        {
            return;
        }
        double etf_dealt_price = getEtfDealtPrice();
        double expected_purchasing_profit = 0;
        double expected_redeeming_profit = 0;
        m_factor_operator->calcProfitWithEtfDealtPrice(etf_dealt_price, expected_purchasing_profit, expected_redeeming_profit);
        if (!m_expected_redeeming_profit)
        {
            m_expected_redeeming_profit = expected_redeeming_profit;
        }

        if ( m_expected_redeeming_profit < -1 * m_factor_operator->nDiscountProfitStep){
            std::cout << "expected_redeeming stop loss: " << m_expected_redeeming_profit << std::endl;
            redeemingEtf();
        }
        if ( expected_redeeming_profit > 0){
            if ( expected_redeeming_profit < (m_expected_redeeming_profit - 0.1 * m_factor_operator->nDiscountProfitStep)){
                std::cout << "expected_redeeming stop profit: " << expected_redeeming_profit << std::endl;
                redeemingEtf();
            }
        }else{
            if ( expected_redeeming_profit < (m_expected_redeeming_profit - 0.2 * m_factor_operator->nDiscountProfitStep)){
                std::cout << "expected_redeeming stop profit: " << expected_redeeming_profit << std::endl;
                redeemingEtf();
            }
        }
        m_expected_redeeming_profit = expected_redeeming_profit > m_expected_redeeming_profit ? expected_redeeming_profit : m_expected_redeeming_profit;
        return;
    }
    periodicProfitCheck();
    /*
    if (datetime::get_time_num() > 144000000)
    {
        m_is_wait_model = false;
        return;
    }
    */
};

void IaETFFundTradingLimitRedeemingWorker::getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec)
{
    int32_t min_vol = 100;
    int32_t max_vol = 1000000;

    int32_t left_vol = order_vol;
    int32_t fractional_shares = left_vol % min_vol;
    if (fractional_shares)
    {
        left_vol = left_vol - fractional_shares + min_vol;
    }

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

bool IaETFFundTradingLimitRedeemingWorker::isEtfBuyOver()
{
    int32_t dealt_vol = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {
        dealt_vol += m_order_vec[i]->nDealtVol;
    }
    if(dealt_vol >= m_factor_operator->m_minUnit)
    {
        return true;
    }else{
        return false;
    }
};

double IaETFFundTradingLimitRedeemingWorker::calculateTheoreticalProfit(double order_price)
{
    double dealt_price = getEtfDealtPrice();
    int32_t dealt_vol = getEtfDealtVol();
    int32_t left_vol = m_factor_operator->m_minUnit - dealt_vol;
    double ave_price = (dealt_price * dealt_vol + order_price * left_vol) / m_factor_operator->m_minUnit;

    double tmp =0;
    double profit = 0;
    m_factor_operator->calcProfitWithEtfDealtPrice(ave_price, tmp, profit);
    return profit;
};

int32_t IaETFFundTradingLimitRedeemingWorker::getEtfBuyVol()
{
    int32_t dealt_vol = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {
        dealt_vol += m_order_vec[i]->nDealtVol;
    }
    return m_factor_operator->m_minUnit - dealt_vol;
};

int32_t IaETFFundTradingLimitRedeemingWorker::getEtfDealtVol()
{
    int32_t dealt_vol = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {
        dealt_vol += m_order_vec[i]->nDealtVol;
    }
    return dealt_vol;
}; 

double IaETFFundTradingLimitRedeemingWorker::getEtfDealtPrice()
{
    int32_t dealt_vol = 0;
    double dealt_turnover = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {
        dealt_vol += m_order_vec[i]->nDealtVol;
        dealt_turnover += m_order_vec[i]->nDealtVol * m_order_vec[i]->nDealtPrice;
    }
    if (dealt_vol)
    {
        return dealt_turnover / dealt_vol;
    }else{
        return 0;
    }
};
double IaETFFundTradingLimitRedeemingWorker::getEtfQueuePrice()
{
    int32_t vol = 0;
    double turnover = 0;
    for (size_t i = 0; i < m_order_vec.size(); i++)
    {   
        if (m_order_vec[i]->nStatus >= 0)
        {
            vol += m_order_vec[i]->nOrderVol;
            turnover += m_order_vec[i]->nOrderPrice * m_order_vec[i]->nOrderVol;
        }
    }
    if (vol)
    {
        return turnover / vol;
    }else{
        return 0;
    }
}; 

bool IaETFFundTradingLimitRedeemingWorker::hasQueueOrder()
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

void IaETFFundTradingLimitRedeemingWorker::cancelEtfOrder()
{
    //std::cout << "[cancelEtfOrder]" << std::endl;

    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); ++iter)
    {
        //std::cout << "[cancelEtfOrder] nStatus:" << iter->second->nStatus << std::endl;
        if (iter->second->nStatus > 0)
        {
            auto set_iter = m_cancel_order_set.find(iter->second->nOrderId);
            if (set_iter != m_cancel_order_set.end())
            {
                continue;
            }
            TiReqOrderDelete req;
            memset(&req, 0, sizeof(TiReqOrderDelete));
            req.nOrderId = iter->second->nOrderId;
            //std::cout << "[cancelEtfOrder] order_id:" << req.nOrderId << std::endl;
            m_client->orderDelete(&req);
            m_cancel_order_set.insert(req.nOrderId);
        }
    }
};

int32_t IaETFFundTradingLimitRedeemingWorker::getQueueOrderSeq()
{
    int32_t seq = 0;
    if(!strcmp(m_order_book.exchange, "SH"))
    {
        for (size_t i = 0; i < TI_ORDERBOOK_ARRAY_LEN; i++)
        {
            if (i >= m_order_book.bid_order_count)
            {
                break;
            }
            seq = m_order_book.bid_orders[i].order_orino;
        }
    }
    if(!strcmp(m_order_book.exchange, "SZ"))
    {
        for (size_t i = 0; i < TI_ORDERBOOK_ARRAY_LEN; i++)
        {
            if (i >= m_order_book.bid_order_count)
            {
                break;
            }
            seq = m_order_book.bid_orders[i].seq;
        }
    }
    return seq;
};

int32_t IaETFFundTradingLimitRedeemingWorker::getQueueOrderVol()
{
    int32_t vol = 0;
    if(!m_queue_order_seq){
        return 0;
    }
    if(!strcmp(m_order_book.exchange, "SH"))
    {
        for (size_t i = 0; i < TI_ORDERBOOK_ARRAY_LEN; i++)
        {
            if (i >= m_order_book.bid_order_count)
            {
                break;
            }
            if (m_order_book.bid_orders[i].order_orino >= m_queue_order_seq)
            {
                break;
            }
            vol += m_order_book.bid_orders[i].volume;
        }
    }
    if(!strcmp(m_order_book.exchange, "SZ"))
    {
        for (size_t i = 0; i < TI_ORDERBOOK_ARRAY_LEN; i++)
        {
            if (i >= m_order_book.bid_order_count)
            {
                break;
            }
            if (m_order_book.bid_orders[i].seq >= m_queue_order_seq)
            {
                break;
            }
            vol += m_order_book.bid_orders[i].volume;
        }
    }
    return vol;
};

void IaETFFundTradingLimitRedeemingWorker::processMatchEtfOrder()
{
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

void IaETFFundTradingLimitRedeemingWorker::processPendingEtfOrder()
{
    double _pendingPrice = m_etf_init_price;
    if(getEtfDealtVol()){   //如果有成交了，就只挂买1价
        _pendingPrice = m_factor_operator->m_etf_last_snap.bid_price[0];
    }

    m_queue_order_seq = getQueueOrderSeq();
    std::vector<int32_t> order_vol_vec;
    getSplitOrderVol(getEtfBuyVol(), order_vol_vec);
    for (size_t i = 0; i < order_vol_vec.size(); i++)
    {
        TiRtnOrderStatus* order_req = new TiRtnOrderStatus();
        order_req->nTradeSideType = TI_TradeSideType_Buy;
        order_req->nBusinessType = TI_BusinessType_Stock;
        strcpy(order_req->szExchange, m_factor_operator->m_etf_last_snap.exchange);
        strcpy(order_req->szSymbol, m_factor_operator->m_etf_last_snap.symbol);
        order_req->nOrderPrice = _pendingPrice;
        
        order_req->nOrderVol = order_vol_vec[i];
        order_req->nReqTimestamp = datetime::get_now_timestamp_ms();

        m_client->orderInsert(order_req);

        m_order_vec.push_back(order_req);
        m_order_req_map[order_req->nReqId] = order_req;
    }
};

void IaETFFundTradingLimitRedeemingWorker::redeemingEtf()
{
    if (m_is_redeeming)
    {
        return;
    }
    m_is_redeeming = true;
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
    std::cout << "IaETFFundTradingLimitRedeemingWorker::redeemingEtf" << order_req->nBusinessType << " " << m_redeeming_order->nReqId << std::endl;
};

void IaETFFundTradingLimitRedeemingWorker::periodicProfitCheck()
{
    if (m_is_wait_model)
    {
        int32_t dealt_vol = getEtfDealtVol();
        if (dealt_vol)  //有成交才考虑止盈止损的情况
        {
            double order_price = getEtfQueuePrice();
            double waite_profit = calculateTheoreticalProfit(order_price);
            double ask_avg_price = IaEtfPriceTool::get_order_dealt_price(
                m_factor_operator->m_minUnit - dealt_vol, 
                m_factor_operator->m_etf_last_snap.ask_price, m_factor_operator->m_etf_last_snap.ask_volume, TI_STOCK_ARRAY_LEN);
            double ask_profit = calculateTheoreticalProfit(ask_avg_price);

            if (waite_profit < 400 && ask_profit < -500)
            {
                std::cout << "IaETFFundTradingLimitRedeemingWorker stop loss: " << ask_profit << std::endl;
                m_is_wait_model = false;
            }

            if (ask_profit > 1500)
            {
                std::cout << "IaETFFundTradingLimitRedeemingWorker stop profit: " << ask_profit << std::endl;
                m_is_wait_model = false;
            }
            /* code */
        }
    }

    WaitForEtfOrderExecution();
};

void IaETFFundTradingLimitRedeemingWorker::WaitForEtfOrderExecution()
{
    if (m_is_over)
    {
        return;
    }
    if (hasQueueOrder())
    {   
        double order_price = getEtfQueuePrice();
        bool allow_order = IaETFTradingSignalFilter::allow_limit_redeeming(m_factor_operator, order_price);
        if (!allow_order)
        {
            if (!getEtfDealtVol())  //还没有成交的情况就撤单  
            {
                cancelEtfOrder();
            }else{                  //已经有成交了就等着成吧 
                if (!m_is_wait_model)
                {
                    cancelEtfOrder();
                }
            }
        }
    }else{
        double order_price = IaETFTradingSignalFilter::get_limit_redeeming_price(m_factor_operator);
        bool allow_order = IaETFTradingSignalFilter::start_limit_redeeming(m_factor_operator, order_price);
        if (!allow_order)
        {
            if (!getEtfDealtVol())  //没成交也没什么信号的情况下
            {
                m_is_over = true;
                return;
            }
        }

        if (m_is_wait_model)
        {
            m_etf_init_price = order_price;
            processPendingEtfOrder();
        }else{
            processMatchEtfOrder();
        }
    }
};

bool IaETFFundTradingLimitRedeemingWorker::IsOver()
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

bool IaETFFundTradingLimitRedeemingWorker::IsCancel()
{
    if (m_is_over)
    {
        if(!m_redeeming_order)
        {
            return true;
        }
    }
    return false;
};

json IaETFFundTradingLimitRedeemingWorker::GetOrderArrayJson()
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

int32_t IaETFFundTradingLimitRedeemingWorker::GetRedemptionFailures()
{
    return m_redemption_failures;
};