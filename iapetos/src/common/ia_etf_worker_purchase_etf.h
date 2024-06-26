#ifndef __IA_ETF_WORKER_PURCHASE_ETF_H__
#define __IA_ETF_WORKER_PURCHASE_ETF_H__

#include "ia_etf_trading_worker.h"

#include <set>
#include <unordered_map>
#include <memory.h>

class IaETFWorkerPurchaseEtf :
    public IaETFTradingWorker
{
public:
    int64_t open();
public:
    IaETFWorkerPurchaseEtf(TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFWorkerPurchaseEtf(){};
};

typedef std::shared_ptr<IaETFWorkerPurchaseEtf> IaETFWorkerPurchaseEtfPtr;

#endif