#ifndef __IA_ETF_WORKER_REDEMPTION_ETF_H__
#define __IA_ETF_WORKER_REDEMPTION_ETF_H__

#include "ia_etf_trading_worker.h"

#include <set>
#include <unordered_map>
#include <memory.h>

class IaETFWorkerRedemptionEtf :
    public IaETFTradingWorker
{
public:
    int64_t open();
public:
    IaETFWorkerRedemptionEtf(TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFWorkerRedemptionEtf(){};
};

typedef std::shared_ptr<IaETFWorkerRedemptionEtf> IaETFWorkerRedemptionEtfPtr;

#endif