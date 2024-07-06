#ifndef __IA_ETF_QUOTE_BAR_CACHE_H__
#define __IA_ETF_QUOTE_BAR_CACHE_H__

#include <memory>
#include <string>

#include "ti_quote_tools.h"
#include "ti_quote_callback.h"
#include "ti_min_bar.h"

#include "unordered_map"

#include <nlohmann/json.hpp>
using namespace nlohmann;


class IaEtfQuoteBarCache 
    : public TiQuoteCallback
{
private:

    typedef std::unordered_map<int64_t, std::shared_ptr<TiMinBar>> TiBarMap;

    std::unordered_map<TI_BarCycType, TiBarMap> m_min_price_bar_map;

    std::unordered_map<TI_BarCycType, TiBarMap> m_min_change_percentage_bar_map;


public:     /*   行情回调   */
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

private:
    void updateMinBar(std::unordered_map<TI_BarCycType, TiBarMap> &barMap, int64_t symbol_id, int64_t timestamp, double value);

public:
    IaEtfQuoteBarCache();
    ~IaEtfQuoteBarCache();

    bool getMinBar(const char* symbol, const char* exchange, TI_BarCycType cyc, TiMinBarPtr& minBar);
    bool getMinChangePercentageBar(const char* symbol, const char* exchange, TI_BarCycType cyc, TiMinBarPtr& minBar);

};

#endif // __IA_ETF_QUOTE_BAR_CACHE_H__