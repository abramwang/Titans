#include "ia_etf_quote_bar_cache.h"

IaEtfQuoteBarCache::IaEtfQuoteBarCache()
{
    m_min_price_bar_map[TI_BarCycType_1m] = TiBarMap();
    m_min_price_bar_map[TI_BarCycType_5m] = TiBarMap();
    m_min_price_bar_map[TI_BarCycType_10m] = TiBarMap();


    m_min_change_percentage_bar_map[TI_BarCycType_1m] = TiBarMap();
    m_min_change_percentage_bar_map[TI_BarCycType_5m] = TiBarMap();
    m_min_change_percentage_bar_map[TI_BarCycType_10m] = TiBarMap();
}

IaEtfQuoteBarCache::~IaEtfQuoteBarCache()
{
}


void IaEtfQuoteBarCache::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);

    updateMinBar(m_min_price_bar_map, id, pData->timestamp, pData->last);

    double change_percentage = pData->pre_close == 0 ? 0 : (pData->last - pData->pre_close) / pData->pre_close;
    updateMinBar(m_min_change_percentage_bar_map, id, pData->timestamp, change_percentage);
};

void IaEtfQuoteBarCache::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);

    updateMinBar(m_min_price_bar_map, id, pData->timestamp, pData->last);

    double change_percentage = pData->pre_close == 0 ? 0 : (pData->last - pData->pre_close) / pData->pre_close;
    updateMinBar(m_min_change_percentage_bar_map, id, pData->timestamp, change_percentage);
};

void IaEtfQuoteBarCache::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);

    updateMinBar(m_min_price_bar_map, id, pData->timestamp, pData->last);

    double change_percentage = pData->pre_close == 0 ? 0 : (pData->last - pData->pre_close) / pData->pre_close;
    updateMinBar(m_min_change_percentage_bar_map, id, pData->timestamp, change_percentage);
};


void IaEtfQuoteBarCache::updateMinBar(std::unordered_map<TI_BarCycType, TiBarMap> &barMap, int64_t symbol_id, int64_t timestamp, double value)
{
    auto map_iter = barMap.begin();
    for(; map_iter != barMap.end(); map_iter++)
    {
        auto it = map_iter->second.find(symbol_id);
        if (it == map_iter->second.end())
        {
            map_iter->second[symbol_id] = std::make_shared<TiMinBar>(map_iter->first);
        }

        map_iter->second[symbol_id]->updateData(timestamp, value);
    }
};

bool IaEtfQuoteBarCache::getMinBar(const char* symbol, const char* exchange, TI_BarCycType cyc,TiMinBarPtr& minBar)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);

    auto map_iter = m_min_price_bar_map.find(cyc);
    if (map_iter != m_min_price_bar_map.end())
    {
        auto it = map_iter->second.find(id);
        if (it != map_iter->second.end())
        {
            minBar = it->second;
            return true;
        }
    }
    return false;
};

bool IaEtfQuoteBarCache::getMinChangePercentageBar(const char* symbol, const char* exchange, TI_BarCycType cyc, TiMinBarPtr& minBar)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);

    auto map_iter = m_min_change_percentage_bar_map.find(cyc);
    if (map_iter != m_min_change_percentage_bar_map.end())
    {
        auto it = map_iter->second.find(id);
        if (it != map_iter->second.end())
        {
            minBar = it->second;
            return true;
        }
    }
    return false;
};