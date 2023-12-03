#include "ia_etf_limit_redeeming_worker.h"
#include "datetime.h"
#include <iostream>
#include "ia_etf_trading_signal_filter.h"
#include "ti_zt_trader_client.h"
#include "ti_trader_formater.h"

IaETFLimitRedeemingWorker::IaETFLimitRedeemingWorker(int64_t id, IaETFFactorOperator* factor, IaETFSemiTraderXtp* trader)
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

void IaETFLimitRedeemingWorker::OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData)
{
    if (m_fund_worker)
    {
        m_fund_worker->OnDepthOrderBookLevelRtn(pData);
    }
};

void IaETFLimitRedeemingWorker::OnRspOrderDelete(const TiRspOrderDelete* pData){
    if (m_fund_worker)
    {
        m_fund_worker->OnRspOrderDelete(pData);
    }
    for (auto iter = m_stock_workers_map.begin(); iter != m_stock_workers_map.end(); iter++)
    {
        iter->second->OnRspOrderDelete(pData);
    }
};
void IaETFLimitRedeemingWorker::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){
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
        
        update_worker_status();
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

void IaETFLimitRedeemingWorker::OnTimer()
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

void IaETFLimitRedeemingWorker::init_worker_task(int64_t id)
{
    json j;
    IaETFTradingSignalFilter::calc_limit_redeeming_profit(m_factor_operator, j);
    j["id"] = id;
    m_trader->PublicWorkerSignal(j);
    init_fund_work();
};    // 初始化任务

void IaETFLimitRedeemingWorker::init_fund_work()
{
    m_fund_worker = new IaETFFundTradingLimitRedeemingWorker(m_client, m_factor_operator);
};              // 初始化基金交易任务

void IaETFLimitRedeemingWorker::init_stock_work()
{
    std::cout << "IaETFLimitRedeemingWorker::init_stock_work" << std::endl;
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

void IaETFLimitRedeemingWorker::update_worker_status()
{
    //m_status.sell_fund_cost = 0;
    //m_status.buy_stock_cost = 0;
    //m_status.replace_stock_cost = 0;
    public_worker_status();
};  // 保存任务状态

void IaETFLimitRedeemingWorker::public_worker_status()
{
    json j =  {
        { "id", m_id },
        { "update_timestamp", datetime::get_format_now_time_ms() },
        { "stock_orders", json::array() },
        { "etf_orders", json::array() },
    };

    for (auto iter = m_stock_workers_map.begin(); iter != m_stock_workers_map.end(); iter++)
    {
        j["stock_orders"].push_back(iter->second->getOrderArrayJson());
    }
    for (auto iter = m_finished_stock_workers_map.begin(); iter != m_finished_stock_workers_map.end(); iter++)
    {
        j["stock_orders"].push_back(iter->second->getOrderArrayJson());
    }

    j["etf_orders"] = m_fund_worker->GetOrderArrayJson();

    char id[1024] = { 0 };
    sprintf(id, "%s_%ld", m_factor_operator->m_etf_last_snap.symbol, m_id);

    m_trader->UpdateWorkerStatus(id, j);
};

void IaETFLimitRedeemingWorker::start()
{
    m_fund_worker->WaitForEtfOrderExecution();
};

bool IaETFLimitRedeemingWorker::is_over()
{
    if (m_fund_worker->IsCancel())
    {
        m_is_over = true;
    }
    return m_is_over;
};

int32_t IaETFLimitRedeemingWorker::get_redemption_failures()
{
    if (m_fund_worker)
    {
        return m_fund_worker->GetRedemptionFailures();
    }else{
        return 0;
    }
};