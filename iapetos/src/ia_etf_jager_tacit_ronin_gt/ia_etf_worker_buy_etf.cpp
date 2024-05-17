#include "ia_etf_worker_buy_etf.h"

IaETFWorkerBuyEtf::IaETFWorkerBuyEtf(IaEtfQuoteDataCache* m_quote_cache, TiGtTraderClient* client, IaEtfSignalFactorPtr factor, std::string account)
    : IaETFTradingWorker(m_quote_cache, client, factor, account)
{

}



void IaETFWorkerBuyEtf::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    
};

void IaETFWorkerBuyEtf::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{

};

void IaETFWorkerBuyEtf::OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData)
{
    
};