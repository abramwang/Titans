#include "ia_etf_worker_buy_etf.h"
#include "ia_etf_price_tool.h"
#include "ti_quote_formater.h"
#include "datetime.h"

IaETFWorkerBuyEtf::IaETFWorkerBuyEtf(TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account)
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

int64_t IaETFWorkerBuyEtf::open()
{
    std::shared_ptr<IaEtfInfo> m_etf_info = m_etf_factor->GetEtfInfo();
    TiQuoteSnapshotStockField* etf_snap = m_quote_cache->GetStockSnapshot(m_etf_info->m_fundId.c_str(), m_etf_info->m_exchange.c_str());

    if (etf_snap == nullptr)
    {
        return -1;
    }

    updateExpectCost(etf_snap);
    double price = IaEtfPriceTool::get_order_price(
            m_status.volume, etf_snap->ask_price, etf_snap->ask_volume, 5);
    double vol = m_status.volume - m_status.finish_volume;

    std::vector<int32_t> order_vol_vec;
    
    getSplitOrderVol(vol, order_vol_vec);

    for (size_t i = 0; i < order_vol_vec.size(); i++)
    {
        TiReqOrderInsert req;
        memset(&req, 0, sizeof(TiReqOrderInsert));
        strcpy(req.szSymbol, m_etf_info->m_fundId.c_str());
        strcpy(req.szExchange, m_etf_info->m_exchange.c_str());
        strcpy(req.szAccount, m_account.c_str());
        req.nTradeSideType = TI_TradeSideType_Buy;
        req.nBusinessType = TI_BusinessType_Stock;
        req.nOffsetType = TI_OffsetType_Open;
        req.nOrderPrice = price;
        req.nOrderVol = order_vol_vec[i];
        req.nReqTimestamp = datetime::get_now_timestamp_ms();
        strcpy(req.szUseStr, "jager");

        m_client->orderInsert(&req);

        m_req_id_set.insert(req.nReqId);
    }
    return 0;
};
