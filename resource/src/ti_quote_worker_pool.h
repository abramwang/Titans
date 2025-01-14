#ifndef __TI_QUOTE_WORKER_POOL_H__
#define __TI_QUOTE_WORKER_POOL_H__

#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include "ti_quote_worker.h"
#include "ti_quote_callback.h"

typedef std::shared_ptr<TiQuoteWorker> TiQuoteWorkerPtr;

class TiQuoteWorkerPool
    : public TiQuoteCallback
{
public:
    TiQuoteWorkerPool(std::vector<TiQuoteCallback*> cb_vec);
    TiQuoteWorkerPool(std::vector<TiQuoteCallback*> cb_vec, int32_t core_id);
    virtual ~TiQuoteWorkerPool(){};
private:
    int m_core_id;
    int m_worker_num;
    std::vector<TiQuoteWorkerPtr> m_worker_vec;
    std::unordered_map<int64_t, TiQuoteWorkerPtr> m_symbol_worker_map; //ex_symbol, worker

    TiQuoteWorkerPtr get_worker(const TI_ExchangeType exchange,const TI_SymbolType symbol);
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData);

public:
    void start();
};

#endif