#include "ia_etf_quote_bar_cache.h"

IaEtfQuoteBarCache::IaEtfQuoteBarCache()
{
    m_min_bar_map[TI_BarCycType_1m] = TiBarMap();
    m_min_bar_map[TI_BarCycType_5m] = TiBarMap();
    m_min_bar_map[TI_BarCycType_10m] = TiBarMap();
}

IaEtfQuoteBarCache::~IaEtfQuoteBarCache()
{
}


void IaEtfQuoteBarCache::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);

    auto map_iter = m_min_bar_map.begin();
    for(; map_iter != m_min_bar_map.end(); map_iter++)
    {
        auto it = map_iter->second.find(id);
        if (it == map_iter->second.end())
        {
            map_iter->second[id] = std::make_shared<TiMinBar>(map_iter->first);
        }

        map_iter->second[id]->processTrade(pData->timestamp, pData->last);
    }

};

void IaEtfQuoteBarCache::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);

    auto map_iter = m_min_bar_map.begin();
    for(; map_iter != m_min_bar_map.end(); map_iter++)
    {
        auto it = map_iter->second.find(id);
        if (it == map_iter->second.end())
        {
            map_iter->second[id] = std::make_shared<TiMinBar>(map_iter->first);
        }

        map_iter->second[id]->processTrade(pData->timestamp, pData->last);
    }
};

void IaEtfQuoteBarCache::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);

    auto map_iter = m_min_bar_map.begin();
    for(; map_iter != m_min_bar_map.end(); map_iter++)
    {
        auto it = map_iter->second.find(id);
        if (it == map_iter->second.end())
        {
            map_iter->second[id] = std::make_shared<TiMinBar>(map_iter->first);
        }

        map_iter->second[id]->processTrade(pData->timestamp, pData->last);
    }
};


std::vector<TiBarDataPtr> IaEtfQuoteBarCache::getBarData(const char* symbol, const char* exchange, TI_BarCycType cyc)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);

    auto map_iter = m_min_bar_map.find(cyc);
    if (map_iter != m_min_bar_map.end())
    {
        auto it = map_iter->second.find(id);
        if (it != map_iter->second.end())
        {
            return it->second->getBars();
        }
    }
    return std::vector<TiBarDataPtr>();
};