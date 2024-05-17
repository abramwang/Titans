#include "ia_etf_worker_buy_etf.h"

#include "ti_quote_formater.h"


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


int64_t IaETFWorkerBuyEtf::open()
{
    std::shared_ptr<IaEtfInfo> m_etf_info = m_etf_factor->GetEtfInfo();
    TiQuoteSnapshotStockField* etf_snap = m_quote_cache->GetStockSnapshot(m_etf_info->m_fundId.c_str(), m_etf_info->m_exchange.c_str());

    if (etf_snap == nullptr)
    {
        return -1;
    }
    
    memcpy(&m_open_snap, etf_snap, sizeof(TiQuoteSnapshotStockField));

    json j;
    TiQuoteFormater::FormatSnapshot(&m_open_snap, j);

    std::cout << "[IaETFWorkerBuyEtf::open] " << j << std::endl;

    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, m_etf_info->m_fundId.c_str());
    strcpy(req.szExchange, m_etf_info->m_exchange.c_str());
    strcpy(req.szAccount, m_account.c_str());
    req.nTradeSideType = TI_TradeSideType_Buy;
    req.nBusinessType = TI_BusinessType_Stock;
    req.nOffsetType = TI_OffsetType_Open;
    req.nOrderPrice = etf_snap->ask_price[2];
    req.nOrderVol = m_etf_info->m_minUnit;
    strcpy(req.szUseStr, "jager");

    m_client->orderInsert(&req);

    return req.nReqId;
};