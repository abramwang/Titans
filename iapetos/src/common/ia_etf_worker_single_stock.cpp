#include "ia_etf_worker_signle_stock.h"
#include "ia_etf_price_tool.h"
#include "ti_trader_formater.h"
#include "datetime.h"

IaETFWorkerSingleStock:: IaETFWorkerSingleStock(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache, IaEtfSignalFactorPtr factor, std::string account, 
        std::shared_ptr<IaEtfConstituentInfo> constituentInfoPtr, TI_TradeSideType side)
    : IaETFTradingWorker(client, quote_cache, factor, account)
{
    m_side = side;
    m_status.exchange = constituentInfoPtr->m_exchange;
    m_status.symbol = constituentInfoPtr->m_symbol;
    m_status.volume = constituentInfoPtr->m_disclosure_vol;

    TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
        m_status.symbol.c_str(), 
        m_status.exchange.c_str());
    memcpy(&m_open_snap, snap, sizeof(TiQuoteSnapshotStockField));
}

int64_t IaETFWorkerSingleStock::open()
{
    TiQuoteSnapshotStockField* snap = m_quote_cache->GetStockSnapshot(
        m_status.symbol.c_str(), 
        m_status.exchange.c_str());

    if (snap == nullptr)
    {
        return -1;
    }
    
    double vol = m_status.volume - m_status.finish_volume;

    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, m_status.symbol.c_str());
    strcpy(req.szExchange, m_status.exchange.c_str());
    strcpy(req.szAccount, m_account.c_str());
    req.nTradeSideType = m_side;
    req.nBusinessType = TI_BusinessType_Stock;
    req.nOffsetType = TI_OffsetType_Open;
    if (m_side == TI_TradeSideType_Buy)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(
            m_status.volume, snap->ask_price, snap->ask_volume, 5, snap->high_limit);
        m_status.is_limit = (req.nOrderPrice == snap->high_limit);
    }
    if (m_side == TI_TradeSideType_Sell)
    {
        req.nOrderPrice = IaEtfPriceTool::get_order_price(
            m_status.volume, snap->bid_price, snap->bid_volume, 5, snap->low_limit);
        m_status.is_limit = (req.nOrderPrice == snap->low_limit);
    }
    req.nOrderVol = vol;
    req.nReqTimestamp = datetime::get_now_timestamp_ms();
    strcpy(req.szUseStr, "jager");

    m_client->orderInsert(&req);

    m_req_id_set.insert(req.nReqId);
    return req.nReqId;
};

json IaETFWorkerSingleStock::getStatusJson()
{
    json j = {
        {"symbol", m_status.symbol},
        {"exchange", m_status.exchange},
        {"expect_cost", m_status.expect_cost},
        {"volume", m_status.volume},
        {"real_cost", json::number_float_t(m_status.real_cost)},
        {"finish_volume", json::number_integer_t(m_status.finish_volume)},
        {"orders", json::array()}
    };
    for (auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        j["orders"].push_back(iter->second.nOrderId);
    }
    return j;
};