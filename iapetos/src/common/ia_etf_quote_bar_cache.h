#ifndef __IA_ETF_QUOTE_BAR_CACHE_H__
#define __IA_ETF_QUOTE_BAR_CACHE_H__

#include <memory>
#include <string>

#include "ti_quote_tools.h"
#include "ti_quote_callback.h"
#include "ti_min_bar.h"

#include "unordered_map"

class IaEtfQuoteBarCache 
    : public TiQuoteCallback
{
private:

    typedef std::unordered_map<int64_t, std::shared_ptr<TiMinBar>> TiBarMap;

    //std::unordered_map<int64_t, std::shared_ptr<TiMinBar>> m_min_bar_map;

    std::unordered_map<TI_BarCycType, TiBarMap> m_min_bar_map;

public:     /*   行情回调   */
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

public:
    IaEtfQuoteBarCache();
    ~IaEtfQuoteBarCache();

    std::vector<TiBarDataPtr> getBarData(const char* symbol, const char* exchange, TI_BarCycType cyc);

};

#endif // __IA_ETF_QUOTE_BAR_CACHE_H__