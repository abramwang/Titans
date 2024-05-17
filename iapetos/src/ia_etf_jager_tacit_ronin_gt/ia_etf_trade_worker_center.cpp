#include "ia_etf_trade_worker_center.h"
#include "ia_etf_signal_factor.h"

#include "ia_etf_worker_buy_etf.h"

IaEtfTradeWorkerCenter::IaEtfTradeWorkerCenter(TiGtTraderClient* trade_client, 
    IaEtfQuoteDataCache* quote_cache, 
    IaEtfUserSetting* user_setting,
    IaEtfSignalCenter* signal_center)
{
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
        (*iter)->OnRtnOrderStatusEvent(pData);

        if ((*iter)->isOver())
        {
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

void IaEtfTradeWorkerCenter::create_trading_worker(const std::string &symbol, const std::string &account, TI_TradeSideType side, std::shared_ptr<IaEtfSignalFactor> etf_factor)
{
    std::cout << "IaEtfTradeWorkerCenter::create_trading_worker" << symbol << " " << account << " " << side<< std::endl;
    std::cout << etf_factor->m_info.diff << " creation_profit:" << etf_factor->m_info.creation_profit << " redemption_profit:"  << etf_factor->m_info.redemption_profit << std::endl;

    IaETFWorkerBuyEtfPtr worker = std::make_shared<IaETFWorkerBuyEtf>(m_quote_cache, m_trade_client, etf_factor, account);
    


    if ( etf_factor->m_info.creation_profit  >  etf_factor->m_info.redemption_profit)
    {
        //申购方向

    } else {
        //赎回方向
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

            create_trading_worker(symbol, account, side, etf_factor);
        }
    }
    

};