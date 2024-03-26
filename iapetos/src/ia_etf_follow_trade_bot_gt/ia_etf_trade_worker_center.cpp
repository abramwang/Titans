#include "ia_etf_trade_worker_center.h"

IaEtfTradeWorkerCenter::IaEtfTradeWorkerCenter(TiGtTraderClient* trade_client, IaEtfQuoteDataCache* quote_cache)
{
    m_trade_client = trade_client;
    m_quote_cache = quote_cache;
}

IaEtfTradeWorkerCenter::~IaEtfTradeWorkerCenter()
{
}