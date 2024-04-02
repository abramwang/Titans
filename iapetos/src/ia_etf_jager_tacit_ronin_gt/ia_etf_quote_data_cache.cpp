#include "ia_etf_quote_data_cache.h"

IaEtfQuoteDataCache::IaEtfQuoteDataCache()
{
}

IaEtfQuoteDataCache::~IaEtfQuoteDataCache()
{
}

void IaEtfQuoteDataCache::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);
    
    auto it = m_snapshot_map.find(id);
    if (it == m_snapshot_map.end())
    {
        m_snapshot_map[id] = std::make_shared<TiQuoteSnapshotStockField>();
    }

    memcpy(m_snapshot_map[id].get(), pData, sizeof(TiQuoteSnapshotStockField));        
}

TiQuoteSnapshotStockField* IaEtfQuoteDataCache::GetSnapshot(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_snapshot_map.find(id);
    if (it != m_snapshot_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};