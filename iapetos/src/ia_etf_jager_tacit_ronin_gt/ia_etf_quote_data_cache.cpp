#include "ia_etf_quote_data_cache.h"

IaEtfQuoteDataCache::IaEtfQuoteDataCache()
{
}

IaEtfQuoteDataCache::~IaEtfQuoteDataCache()
{
}

void IaEtfQuoteDataCache::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);
    
    auto it = m_index_map.find(id);
    if (it == m_index_map.end())
    {
        m_index_map[id] = std::make_shared<TiQuoteSnapshotIndexField>();
    }

    memcpy(m_index_map[id].get(), pData, sizeof(TiQuoteSnapshotIndexField));        
};

void IaEtfQuoteDataCache::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);
    
    auto it = m_future_map.find(id);
    if (it == m_future_map.end())
    {
        m_future_map[id] = std::make_shared<TiQuoteSnapshotFutureField>();
    }

    memcpy(m_future_map[id].get(), pData, sizeof(TiQuoteSnapshotFutureField));        
};


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

TiQuoteSnapshotStockField* IaEtfQuoteDataCache::GetStockSnapshot(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_snapshot_map.find(id);
    if (it != m_snapshot_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};

TiQuoteSnapshotIndexField* IaEtfQuoteDataCache::GetIndexSnapshot(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_index_map.find(id);
    if (it != m_index_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};

TiQuoteSnapshotFutureField* IaEtfQuoteDataCache::GetFutureSnapshot(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_future_map.find(id);
    if (it != m_future_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};