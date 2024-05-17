#include "ia_etf_trading_worker.h"

IaETFTradingWorker::IaETFTradingWorker(IaEtfQuoteDataCache* quote_cache, TiGtTraderClient* client, IaEtfSignalFactorPtr factor, std::string account){
    m_quote_cache = quote_cache;
    m_client = client;
    m_etf_factor = factor;
    m_account = account;
};


void IaETFTradingWorker::getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec)
{
    int32_t min_vol = 100;
    int32_t max_vol = 1000000;

    int32_t left_vol = order_vol;

    while (left_vol > max_vol)
    {
        int32_t random = rand() % max_vol + 1;
        int32_t vol = (random /min_vol )*  min_vol;
        if (vol > left_vol)
        {
            break;
        }else{
            order_vol_vec.push_back(vol);
            left_vol -= vol;
        }
    }
    if (left_vol)
    {
        order_vol_vec.push_back(left_vol);
    }
};