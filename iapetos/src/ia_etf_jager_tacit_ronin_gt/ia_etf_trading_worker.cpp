#include "ia_etf_trading_worker.h"

IaETFTradingWorker::IaETFTradingWorker(IaEtfQuoteDataCache* quote_cache, TiGtTraderClient* client, IaEtfSignalFactorPtr factor, std::string account){
    m_quote_cache = quote_cache;
    m_client = client;
    m_etf_factor = factor;
    m_account = account;
};