#include "ia_etf_worker_purchase_etf.h"
#include "ia_etf_price_tool.h"
#include "ti_quote_formater.h"
#include "datetime.h"

IaETFWorkerPurchaseEtf::IaETFWorkerPurchaseEtf(TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account)
    : IaETFTradingWorker(client, m_quote_cache, factor, account)
{
    m_status.symbol = factor->GetEtfInfo()->m_fundId;
    m_status.exchange = factor->GetEtfInfo()->m_exchange;
    m_status.volume = factor->GetEtfInfo()->m_minUnit;
    m_status.real_cost = 0;
    m_status.finish_volume = 0;
    m_status.wrong_number = 0;
    m_check_time = datetime::get_now_timestamp_ms();

    TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
        m_status.symbol.c_str(), 
        m_status.exchange.c_str());
    memcpy(&m_open_snap, snap, sizeof(TiQuoteSnapshotStockField));
    updateExpectCost(snap);
}

int64_t IaETFWorkerPurchaseEtf::open()
{
    std::shared_ptr<IaEtfInfo> m_etf_info = m_etf_factor->GetEtfInfo();
    
    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, m_etf_info->m_fundId.c_str());
    strcpy(req.szExchange, m_etf_info->m_exchange.c_str());
    strcpy(req.szAccount, m_account.c_str());
    req.nTradeSideType = TI_TradeSideType_Purchase;
    req.nBusinessType = TI_BusinessType_ETF;
    req.nOffsetType = TI_OffsetType_Open;
    req.nOrderPrice = 1;
    req.nOrderVol = m_status.volume;
    req.nReqTimestamp = datetime::get_now_timestamp_ms();
    strcpy(req.szUseStr, "jager");

    m_client->orderInsert(&req);

    m_req_id_set.insert(req.nReqId);
    return 0;
};