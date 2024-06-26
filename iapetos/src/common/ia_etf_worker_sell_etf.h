#ifndef __IA_ETF_WORKER_SELL_ETF_H__
#define __IA_ETF_WORKER_SELL_ETF_H__

#include "ia_etf_trading_worker.h"

#include <set>
#include <unordered_map>
#include <memory.h>

class IaETFWorkerSellEtf :
    public IaETFTradingWorker
{
public:
    int64_t open();
public:
    IaETFWorkerSellEtf(int64_t id, TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFWorkerSellEtf(){};
};

typedef std::shared_ptr<IaETFWorkerSellEtf> IaETFWorkerSellEtfPtr;

#endif