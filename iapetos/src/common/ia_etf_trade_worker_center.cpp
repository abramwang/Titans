#include "ia_etf_trade_worker_center.h"
#include "ia_etf_signal_factor.h"

#include "ia_etf_worker_buy_etf.h"
#include "ia_etf_worker_sell_etf.h"
#include "ia_etf_worker_basket_stock.h"
#include "ia_etf_worker_purchase_etf.h"
#include "ia_etf_worker_redemption_etf.h"

#include "datetime.h"

IaEtfTradeWorkerCenter::IaEtfTradeWorkerCenter(TiTraderClient* trade_client, 
    IaEtfQuoteDataCache* quote_cache, 
    IaEtfUserSetting* user_setting,
    IaEtfSignalCenter* signal_center)
{
    m_work_id = datetime::get_time_sec_num()*10;

    m_trade_client = trade_client;
    m_quote_cache = quote_cache;
    m_user_setting = user_setting;
    m_signal_center = signal_center;
}

IaEtfTradeWorkerCenter::~IaEtfTradeWorkerCenter()
{
}

void IaEtfTradeWorkerCenter::OnRspAccountInfo(const TiRspAccountInfo* pData)
{
    auto iter = m_account_info_map.find(pData->szAccount);
    if (iter == m_account_info_map.end())
    {
        m_account_info_map.insert(std::make_pair(pData->szAccount, *pData));
    }
    else
    {
        iter->second = *pData;
    }
};

void IaEtfTradeWorkerCenter::OnRspQryPosition(const TiRspQryPosition* pData, bool isLast)
{
    auto map_iter = m_account_position_map.find(pData->szAccount);
    if (map_iter == m_account_position_map.end())
    {
        std::map<std::string, TiRspQryPosition> position_map;
        position_map.insert(std::make_pair(pData->szSymbol, *pData));
        m_account_position_map.insert(std::make_pair(pData->szAccount, position_map));
        
        position_map[pData->szSymbol] = *pData;
    }
    else
    {
        auto iter = map_iter->second.find(pData->szSymbol);
        if (iter == map_iter->second.end())
        {
            map_iter->second.insert(std::make_pair(pData->szSymbol, *pData));
        }
        else
        {
            iter->second = *pData;
        }
    }
};

void IaEtfTradeWorkerCenter::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    auto iter = m_trading_worker_list.begin();
    for (; iter != m_trading_worker_list.end(); )
    {
        if ((*iter)->getAccount() == pData->szAccount)
        {
            (*iter)->OnRtnOrderStatusEvent(pData);
        }

        if ((*iter)->isOver())
        {
            std::cout << "IaEtfTradeWorkerCenter::OnRtnOrderStatusEvent: is over" << std::endl; 
            m_over_trading_worker_list.push_back(*iter);
            iter = m_trading_worker_list.erase(iter);
        }else{
            iter++;
        }
    }
};

void IaEtfTradeWorkerCenter::OnTimer()
{
    auto iter = m_trading_worker_list.begin();
    for (; iter != m_trading_worker_list.end(); iter++)
    {
        (*iter)->OnTimer();
    }

    iter = m_trading_waiting_worker_list.begin();
    for (; iter != m_trading_waiting_worker_list.end(); )
    {
        if ((*iter)->isReady())
        {
            m_trading_worker_list.push_back(*iter);
            (*iter)->open();
            iter = m_trading_waiting_worker_list.erase(iter);
        }else{
            iter++;
        }
    }
};


bool IaEtfTradeWorkerCenter::get_position(const std::string &account, const std::string &symbol, TiRspQryPosition &position)
{
    auto map_iter = m_account_position_map.find(account);
    if (map_iter == m_account_position_map.end())
    {
        return false;
    }

    auto iter = map_iter->second.find(symbol);
    if (iter == map_iter->second.end())
    {
        return false;
    }

    position = iter->second;
    return true;
};

void IaEtfTradeWorkerCenter::create_etf_smart_worker(const std::string &symbol, const std::string &account, TI_TradeSideType side, std::shared_ptr<IaEtfSignalFactor> etf_factor)
{
    std::cout << "IaEtfTradeWorkerCenter::create_etf_smart_worker" << symbol << " " << account << " " << side<< std::endl;
    std::cout << etf_factor->m_info.diff << " creation_profit:" << etf_factor->m_info.creation_profit << " redemption_profit:"  << etf_factor->m_info.redemption_profit << std::endl;

    return;
    IaETFTradingWorkerPtr worker;
    /*
    if (side == TI_TradeSideType_Buy)
    {
        worker = std::make_shared<IaETFWorkerBuyEtf>(m_trade_client, m_quote_cache, etf_factor, account);
    }
    else if (side == TI_TradeSideType_Sell)
    {
        worker = std::make_shared<IaETFWorkerSellEtf>(m_trade_client, m_quote_cache, etf_factor, account);
    }
    else
    {
        return;
    }
//  */
    worker = std::make_shared<IaETFWorkerBasketStock>(m_work_id ,m_trade_client, m_quote_cache, etf_factor, account, side);

    if ( etf_factor->m_info.creation_profit  >  etf_factor->m_info.redemption_profit)
    {
        //申购方向

    } else {
        //赎回方向
    }

    m_trading_worker_list.push_back(worker);
    worker->open();
};

void IaEtfTradeWorkerCenter::create_etf_trading_worker(bool etf, const std::string &symbol, const std::string &account, TI_TradeSideType side, std::shared_ptr<IaEtfSignalFactor> etf_factor)
{
    std::cout << "IaEtfTradeWorkerCenter::create_etf_trading_worker" << symbol << " " << account << " " << side<< std::endl;
    std::cout << etf_factor->m_info.diff << " creation_profit:" << etf_factor->m_info.creation_profit << " redemption_profit:"  << etf_factor->m_info.redemption_profit << std::endl;

    m_work_id++;

    IaETFTradingWorkerPtr worker;
    
    if (etf)
    {
        if (side == TI_TradeSideType_Buy)
        {
            worker = std::make_shared<IaETFWorkerBuyEtf>(m_work_id, m_trade_client, m_quote_cache, etf_factor, account);
            IaETFWorkerRedemptionEtfPtr next_worker = std::make_shared<IaETFWorkerRedemptionEtf>(m_work_id, m_trade_client, m_quote_cache, etf_factor, account);
            next_worker->setPreWorker(worker);
            m_trading_waiting_worker_list.push_back(next_worker);
        }
        else if (side == TI_TradeSideType_Sell)
        {
            worker = std::make_shared<IaETFWorkerSellEtf>(m_work_id, m_trade_client, m_quote_cache, etf_factor, account);
        }
        else
        {
            return;
        }
    }
    else
    {
        if (side == TI_TradeSideType_Buy)
        {
            worker = std::make_shared<IaETFWorkerBasketStock>(m_work_id, m_trade_client, m_quote_cache, etf_factor, account, side);
            IaETFTradingWorkerPtr next_worker = std::make_shared<IaETFWorkerPurchaseEtf>(m_work_id, m_trade_client, m_quote_cache, etf_factor, account);
            next_worker->setPreWorker(worker);
            m_trading_waiting_worker_list.push_back(next_worker);
        }
        else if (side == TI_TradeSideType_Sell)
        {
            worker = std::make_shared<IaETFWorkerBasketStock>(m_work_id, m_trade_client, m_quote_cache, etf_factor, account, side);
        }
        else
        {
            return;
        }

    }
    
    m_trading_worker_list.push_back(worker);
    worker->open();
};


void IaEtfTradeWorkerCenter::OnTradingSignal(json &msg)
{
    std::cout << "IaEtfTradeWorkerCenter::OnTradingSignal" << msg << std::endl;

    if(!msg.contains("side"))
    {
        return;
    }

    if (!msg.contains("symbol_list"))
    {
        return;
    }
    
    if (!msg["symbol_list"].is_array())
    {
        return;
    }

    if (!msg.contains("account_list"))
    {
        return;
    }

    if (!msg["account_list"].is_array())
    {
        return;
    }

    bool enable_smart = true;
    bool etf = false;

    if (msg.contains("security_type"))
    {
        enable_smart = false;
        std::string security_type = msg["security_type"].get<std::string>();
        if (security_type == "etf")
        {
            etf = true;
        }else{
            etf = false;
        }
    }


    TI_TradeSideType side = TI_TradeSideType_Default;

    if (msg["side"].get<std::string>() == "buy")
    {
        side = TI_TradeSideType_Buy;
    }
    else if (msg["side"].get<std::string>() == "sell")
    {
        side = TI_TradeSideType_Sell;
    }
    

    std::shared_ptr<IaEtfSignalFactor> etf_factor;

    for (auto symbol_iter = msg["symbol_list"].begin(); symbol_iter !=  msg["symbol_list"].end(); symbol_iter++)
    {
        std::string symbol = symbol_iter.value();

        if (!m_signal_center->GetEtfFactor(symbol, etf_factor))
        {
            continue;
        }

        for (auto account_iter = msg["account_list"].begin(); account_iter !=  msg["account_list"].end(); account_iter++)
        {
            std::string account = account_iter.value();
            if (enable_smart)
            {
                create_etf_smart_worker(symbol, account, side, etf_factor);
            }
            else
            {
                create_etf_trading_worker(etf, symbol, account, side, etf_factor);
            }
        }
    }
    

};