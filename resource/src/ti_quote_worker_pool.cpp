#include "ti_quote_worker_pool.h"
#include "ti_quote_tools.h"

TiQuoteWorkerPool::TiQuoteWorkerPool(std::vector<TiQuoteCallback*> cb_vec)
{
    m_worker_num = 0;
    for (auto cb : cb_vec)
    {
        TiQuoteWorkerPtr worker = std::make_shared<TiQuoteWorker>(cb);
        m_worker_vec.push_back(worker);
    }
}

TiQuoteWorkerPtr TiQuoteWorkerPool::get_worker(const TI_ExchangeType exchange, const TI_SymbolType symbol)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    if (m_symbol_worker_map.find(id) != m_symbol_worker_map.end())
    {
        return m_symbol_worker_map[id];
    }else{
        m_symbol_worker_map[id] = m_worker_vec[m_worker_num];
        m_worker_num = (m_worker_num + 1) % m_worker_vec.size();
        return m_worker_vec[m_worker_num];
    }
}

void TiQuoteWorkerPool::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    TiQuoteWorkerPtr worker = get_worker(pData->exchange, pData->symbol);
    worker->OnL2FutureSnapshotRtn(pData);
};

void TiQuoteWorkerPool::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    TiQuoteWorkerPtr worker = get_worker(pData->exchange, pData->symbol);
    worker->OnL2StockSnapshotRtn(pData);
};

void TiQuoteWorkerPool::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData)
{
    TiQuoteWorkerPtr worker = get_worker(pData->exchange, pData->symbol);
    worker->OnL2StockMatchesRtn(pData);
};

void TiQuoteWorkerPool::OnL2StockOrderRtn(const TiQuoteOrderField* pData)
{
    TiQuoteWorkerPtr worker = get_worker(pData->exchange, pData->symbol);
    worker->OnL2StockOrderRtn(pData);
};

void TiQuoteWorkerPool::start()
{
    for (auto worker : m_worker_vec)
    {
        worker->start();
    }
}