#include "ia_etf_redeeming_worker.h"
#include "datetime.h"
#include <iostream>
#include "ia_etf_trading_signal_filter.h"
#include "ti_zt_trader_client.h"
#include "ti_trader_formater.h"

IaETFRedeemingWorker::IaETFRedeemingWorker(int64_t id, IaETFFactorOperator* factor, IaETFSemiTraderXtp* trader)
    : IaETFTradingWorker(trader->GetTraderClient())
{
    m_factor_operator = factor;
    m_trader = trader;
    m_fund_worker = NULL;
    m_id = id;
    m_start_stock_trading = false;
    m_is_over = false;

    init_worker_task(id);
    update_worker_status();
}

void IaETFRedeemingWorker::OnRspOrderDelete(const TiRspOrderDelete* pData){
    if (m_fund_worker)
    {
        m_fund_worker->OnRspOrderDelete(pData);
    }
    for (auto iter = m_stock_workers_map.begin(); iter != m_stock_workers_map.end(); iter++)
    {
        iter->second->OnRspOrderDelete(pData);
    }
};
void IaETFRedeemingWorker::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){
    if (m_is_over)
    {
        return;
    }
    
    if (!m_fund_worker)
    {
        return;
    }

    if (!strcmp(pData->szSymbol, m_factor_operator->m_etf_last_snap.symbol))
    {
        m_fund_worker->OnRtnOrderStatusEvent(pData);
    
        if (m_fund_worker->IsOver())
        {
            if(!m_start_stock_trading){
                init_stock_work();
                return;
            }
        }
    }

    if (!m_start_stock_trading)
    {
        return;
    }

    auto iter = m_stock_workers_map.find(pData->szSymbol);
    IaETFSingleStockTradingWorkerPtr worker = NULL;
    if (iter != m_stock_workers_map.end())
    {
        iter->second->OnRtnOrderStatusEvent(pData);
        worker = iter->second;
        if (worker->isOver())
        {
            m_finished_stock_workers_map[pData->szSymbol] = worker;
            m_stock_workers_map.erase(iter);
        }
        std::cout << "IaETFFundTradingRedeemingWorker: " << worker->isOver()  << ":" <<  worker->getStatusJson() << std::endl;
    }
    
    update_worker_status();
    std::cout << "IaETFFundTradingRedeemingWorker: " << m_finished_stock_workers_map.size()  << ":" <<  m_stock_workers_map.size() << "/" << m_finished_stock_workers_map.size() + m_stock_workers_map.size() << std::endl;

    if(m_finished_stock_workers_map.size() && m_stock_workers_map.empty()){
        std::cout << "IaETFFundTradingRedeemingWorker work done !" << std::endl;
        m_is_over = true;
        ((TiZtTraderClient*)m_client)->QueryAsset();
    }
};

void IaETFRedeemingWorker::OnTimer()
{
    if (m_fund_worker)
    {
        m_fund_worker->OnTimer();
    }
    for (auto iter = m_stock_workers_map.begin(); iter != m_stock_workers_map.end(); iter++)
    {
        iter->second->OnTimer();
    }
};


////////////////////////////////////////////////////////////////////////
// 私有方法
////////////////////////////////////////////////////////////////////////

void IaETFRedeemingWorker::init_worker_task(int64_t id)
{
    IaETFTradingSignalFilter::calc_redeeming_profit(m_factor_operator, m_json_signal);
    m_json_signal["id"] = id;
    m_trader->PublicWorkerSignal(m_json_signal);
    init_fund_work();
};    // 初始化任务

void IaETFRedeemingWorker::init_fund_work()
{
    m_fund_worker = new IaETFFundTradingRedeemingWorker(m_client, m_factor_operator);
    m_fund_worker->BuyEtf();
};              // 初始化基金交易任务

void IaETFRedeemingWorker::init_stock_work()
{
    std::cout << "IaETFRedeemingWorker::init_stock_work" << std::endl;
    m_start_stock_trading = true;
    for (auto iter = m_factor_operator->m_cashMap.begin(); iter != m_factor_operator->m_cashMap.end(); iter++)
    {   
        //同一个交易所的股票自己交易
        if (strcmp(m_factor_operator->m_etf_last_snap.exchange, iter->second->m_last_snap.exchange) == 0)
        {
            if (iter->second->m_info.m_disclosure_vol && iter->second->m_last_snap.last && !iter->second->bReplace)    //过滤停牌的股票，过滤现金替代部分
            {
                IaETFSingleStockTradingWorkerPtr worker = std::make_shared<IaETFSingleStockTradingWorker>(iter->second, TI_TradeSideType_Sell, m_client);
                m_stock_workers_map[iter->second->m_last_snap.symbol] = worker;
            }
        }
    }
};             // 初始化股票交易任务

void IaETFRedeemingWorker::update_worker_status()
{
    //m_status.sell_fund_cost = 0;
    //m_status.buy_stock_cost = 0;
    //m_status.replace_stock_cost = 0;
    public_worker_status();
};  // 保存任务状态

void IaETFRedeemingWorker::public_worker_status()
{
    m_json_status = {
        { "id", m_id },
        { "update_timestamp", datetime::get_format_now_time_ms() },
        { "stock_trade_status", json::array() },
        { "stock_finished_trade_status", json::array() },
        { "stock_public_fund_trade_status", json::array() },
    };
    //stock_public_fund_trade_status
    //stock_trade_status
    for (auto iter = m_stock_workers_map.begin(); iter != m_stock_workers_map.end(); iter++)
    {
        m_json_status["stock_trade_status"].push_back(iter->second->getStatusJson());
    }
    for (auto iter = m_finished_stock_workers_map.begin(); iter != m_finished_stock_workers_map.end(); iter++)
    {
        m_json_status["stock_finished_trade_status"].push_back(iter->second->getStatusJson());
    }

    m_trader->PublicWorkerStatus(m_json_status);
};