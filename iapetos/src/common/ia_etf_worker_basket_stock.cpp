#include "ia_etf_worker_basket_stock.h"
#include "ia_etf_price_tool.h"
#include "ti_trader_formater.h"
#include <glog/logging.h>
#include "datetime.h"

IaETFWorkerBasketStock:: IaETFWorkerBasketStock(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache, IaEtfSignalFactorPtr factor, std::string account, 
        TI_TradeSideType side)
    : IaETFTradingWorker(client, quote_cache, factor, account)
{
    m_side = side;
    m_basket_status.symbol = factor->GetEtfInfo()->m_fundId;
    m_basket_status.exchange = factor->GetEtfInfo()->m_exchange;
    m_basket_status.stock_num = 0;
    m_basket_status.over_num = 0;

    m_check_time = datetime::get_now_timestamp_ms();
}

void IaETFWorkerBasketStock::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    for (auto iter = m_trading_worker_map.begin(); iter != m_trading_worker_map.end(); ++iter)
    {
        iter->second->OnRspOrderDelete(pData);
    }
};

void IaETFWorkerBasketStock::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    if (isOver())
    {
        return;
    }
    for (auto iter = m_trading_worker_map.begin(); iter != m_trading_worker_map.end(); ++iter)
    {
        iter->second->OnRtnOrderStatusEvent(pData);
    }
};

void IaETFWorkerBasketStock::OnTimer()
{
    if (isOver())
    {
        return;
    }
    std::vector<std::string> working_symbol;
    m_basket_status.over_num = 0;
    for (auto iter = m_trading_worker_map.begin(); iter != m_trading_worker_map.end(); ++iter)
    {
        iter->second->OnTimer();
        if (iter->second->isOver())
        {
            m_basket_status.over_num += 1;
        }else{
            working_symbol.push_back(iter->first);
        }
    }
    
    int64_t now = datetime::get_now_timestamp_ms();
    if((now - m_check_time) >= 5000) //5s 检查一次
    {
        m_check_time = now;
        std::cout << "[OnTimer]: ETF " << m_basket_status.symbol 
            << " Basket Stock Over Num: " << m_basket_status.over_num 
            << "/ " << m_basket_status.stock_num
            << std::endl;
        for(auto iter = working_symbol.begin(); iter != working_symbol.end(); ++iter)
        {
            std::cout << "[OnTimer]: ETF " << m_basket_status.symbol 
                << " Working Stock: " << *iter
                << std::endl;
        }
    }
};

int64_t IaETFWorkerBasketStock::open()
{
    std::vector<std::shared_ptr<IaEtfConstituentInfo>>* constituent_info_vec = m_etf_factor->GetConstituentInfo();

    for (auto iter = constituent_info_vec->begin(); iter != constituent_info_vec->end(); ++iter)
    {
        std::shared_ptr<IaEtfConstituentInfo> constituent_info_ptr = *iter;

        //过滤掉现金替代品
        if (constituent_info_ptr->m_symbol == "159900")
        {
            LOG(INFO) << "Cash-Must:  ETF " << m_basket_status.symbol << "," << m_basket_status.exchange<< "," << constituent_info_ptr->m_symbol << std::endl;
            continue;
        }

        //过滤没有披露数量的股票
        if (constituent_info_ptr->m_disclosure_vol == 0)
        {
            continue;
        }

        //过滤不在同一市场的股票
        if (constituent_info_ptr->m_exchange != m_basket_status.exchange)
        {
            LOG(INFO) << "Inter-Market:  ETF " << m_basket_status.symbol << "," << m_basket_status.exchange<< "," << constituent_info_ptr->m_symbol << std::endl;
            continue;
        }

        //过滤必须现金替代的股票
        if ((constituent_info_ptr->m_replace_flag == IA_ERT_CASH_MUST) ||
            (constituent_info_ptr->m_replace_flag == IA_ERT_CASH_MUST_INTER_SZ) ||
            (constituent_info_ptr->m_replace_flag == IA_ERT_CASH_MUST_INTER_OTHER) ||
            (constituent_info_ptr->m_replace_flag == IA_ERT_CASH_MUST_INTER_HK))
        {
            LOG(INFO) << "Must_Replace:  ETF " << m_basket_status.symbol << "," << m_basket_status.exchange<< "," << constituent_info_ptr->m_symbol << std::endl;
            continue;
        }

        TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
            constituent_info_ptr->m_symbol.c_str(), 
            constituent_info_ptr->m_exchange.c_str());
        
        if (snap == nullptr)
        {
            LOG(INFO) << "No-Snapshot:  ETF " << m_basket_status.symbol << "," << m_basket_status.exchange<< "," << constituent_info_ptr->m_symbol << std::endl;
            continue;
        }
        
        //跌停不买
        if (m_side == TI_TradeSideType_Buy)
        {
            if (snap->last == snap->low_limit)
            {
                LOG(INFO) << "Low-Limit:  ETF " << m_basket_status.symbol << "," << m_basket_status.exchange<< "," << constituent_info_ptr->m_symbol << std::endl;
                continue;
            }
        }

        //涨停不卖
        if (m_side == TI_TradeSideType_Sell)
        {
            if (snap->last == snap->high_limit)
            {
                LOG(INFO) << "High-Limit:  ETF " << m_basket_status.symbol << "," << m_basket_status.exchange<< "," << constituent_info_ptr->m_symbol << std::endl;
                continue;
            }
        }
        
        LOG(INFO) << "Single Stock:  ETF " << m_basket_status.symbol << "," << m_basket_status.exchange<< "," << constituent_info_ptr->m_symbol << std::endl;

        IaETFWorkerSingleStockPtr worker = std::make_shared<IaETFWorkerSingleStock>(m_client, m_quote_cache, m_etf_factor, m_account, constituent_info_ptr, m_side);
        m_trading_worker_map[constituent_info_ptr->m_symbol] = worker;
    }

    m_basket_status.stock_num = m_trading_worker_map.size();

    for(auto iter = m_trading_worker_map.begin(); iter != m_trading_worker_map.end(); ++iter)
    {
        iter->second->open();
    }

    return 0;
};

json IaETFWorkerBasketStock::getStatusJson()
{
    json j = {
        {"symbol", m_basket_status.symbol},
        {"exchange", m_basket_status.exchange},
        {"stock_num", m_basket_status.stock_num},
        {"over_num", m_basket_status.over_num},
    };
    return j;
};

bool IaETFWorkerBasketStock::isOver()
{
    if (m_basket_status.stock_num == m_basket_status.over_num && m_basket_status.stock_num)
    {
        return true;
    }
    return false;
};
