#ifndef __IA_ETF_WORKER_BUY_ETF_H__
#define __IA_ETF_WORKER_BUY_ETF_H__

#include "ia_etf_trading_worker.h"

#include <set>
#include <unordered_map>
#include <memory.h>

class IaETFWorkerBuyEtf :
    public IaETFTradingWorker
{
public:
    int64_t open();
public:
    IaETFWorkerBuyEtf(int64_t id, TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFWorkerBuyEtf(){};
};

typedef std::shared_ptr<IaETFWorkerBuyEtf> IaETFWorkerBuyEtfPtr;

#endif