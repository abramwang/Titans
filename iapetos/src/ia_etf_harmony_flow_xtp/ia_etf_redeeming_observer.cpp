#include "ia_etf_redeeming_observer.h"
#include "ia_etf_trading_signal_filter.h"
#include "datetime.h"
#include <iostream>

IaETFRedeemingObserver::IaETFRedeemingObserver(int64_t id, IaETFFactorOperator* factor, IaETFSemiTraderXtp* trader)
    : IaETFTradingWorker(trader->GetTraderClient())
{
    m_factor_operator = factor;
    m_semi_trader = trader;
    m_worker = NULL;
    m_reservedCapital = 0;
    m_redemption_failures = 0;
}

void IaETFRedeemingObserver::OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData)
{
    if (m_worker)
    {
        m_worker->OnDepthOrderBookLevelRtn(pData);
    }
};

void IaETFRedeemingObserver::OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data)
{
    //return;
    if (m_redemption_failures > 5)
    {
        return;
    }
    
    /*
    if (m_semi_trader->m_signal_count >= 2)
    {
        return;
    }
    */

    if (!m_worker)
    {
        double need_cash = m_factor_operator->m_etf_last_snap.last * m_factor_operator->m_minUnit;
    
        double available_cash = m_semi_trader->GetAvailableCash();
        if (need_cash > available_cash * 0.9) // 资金不足
        {
            return;
        }

        double order_price = IaETFTradingSignalFilter::get_limit_redeeming_price(m_factor_operator);
        bool allow_order = IaETFTradingSignalFilter::allow_limit_redeeming(m_factor_operator, order_price);
        if (allow_order)
        {
            m_reservedCapital = m_factor_operator->m_etf_last_snap.last * m_factor_operator->m_minUnit;
            m_semi_trader->m_frozen_cash += m_reservedCapital;

            m_worker = new IaETFLimitRedeemingWorker(datetime::get_time_num(), m_factor_operator, m_semi_trader);
            m_semi_trader->AddWorker(std::shared_ptr<IaETFTradingWorker>(m_worker));
            m_semi_trader->m_signal_count += 1;
            m_worker->start();
        }
    }else{
        if(m_worker->is_over())
        {
            std::cout << "[ReleaseFrozenFunds]: " << m_factor_operator->m_etf_last_snap.symbol << ", m_reservedCapital:" << m_reservedCapital << ", m_frozen_cash:" << m_semi_trader->m_frozen_cash << std::endl;
            m_semi_trader->m_frozen_cash -= m_reservedCapital;
            m_reservedCapital = 0;
            std::cout << "[ReleaseFrozenFunds over]: " << m_factor_operator->m_etf_last_snap.symbol << ", m_reservedCapital:" << m_reservedCapital << ", m_frozen_cash:" << m_semi_trader->m_frozen_cash << std::endl;
            
            m_redemption_failures += m_worker->get_redemption_failures();
            m_ex_worker_vec.push_back(m_worker);
            m_worker = NULL;
        }
    }

};