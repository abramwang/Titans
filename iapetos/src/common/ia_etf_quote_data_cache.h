#ifndef __IA_ETF_QUOTE_DATA_CACHE_H__
#define __IA_ETF_QUOTE_DATA_CACHE_H__

#include <memory>
#include <string>
#include "unordered_map"

#include "ti_quote_tools.h"
#include "ti_quote_callback.h"

#include "ia_etf_quote_bar_cache.h"

class IaEtfQuoteDataCache 
    : public TiQuoteCallback
{
private:
    std::unordered_map<int64_t, std::shared_ptr<TiQuoteSnapshotStockField>> m_snapshot_map;
    std::unordered_map<int64_t, std::shared_ptr<TiQuoteSnapshotIndexField>> m_index_map;
    std::unordered_map<int64_t, std::shared_ptr<TiQuoteSnapshotFutureField>> m_future_map;

    IaEtfQuoteBarCache m_bar_cache;

public:     /*   行情回调   */
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

public:
    IaEtfQuoteDataCache();
    ~IaEtfQuoteDataCache();

public:
    TiQuoteSnapshotStockField* GetStockSnapshot(const char* symbol, const char* exchange);
    TiQuoteSnapshotIndexField* GetIndexSnapshot(const char* symbol, const char* exchange);
    TiQuoteSnapshotFutureField* GetFutureSnapshot(const char* symbol, const char* exchange);
    
    IaEtfQuoteBarCache* getBarCache() { return &m_bar_cache; };

};





#endif // __IA_ETF_QUOTE_DATA_CACHE_H__