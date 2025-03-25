#include "oc_book_worker.h"
#include "ti_quote_tools.h"
#include <string.h>

OcBookWorker::OcBookWorker(int32_t id, TiQuoteIpcPublisher* quote_ipc_publisher)
{
    m_book_engine = new TiBookEngine(this, 0, 10);
    memset(&m_book_snap_cash, 0, sizeof(TiQuoteSnapshotStockField));
    m_cout_time = 0;
    m_id = id;
    m_quote_ipc_publisher = quote_ipc_publisher;
}

OcBookWorker::~OcBookWorker()
{
    delete m_book_engine;
}

OcQuoteDepthSnapRepairmanPtr OcBookWorker::GetRepairman(TiQuoteSnapshotStockField* pData)
{
    int64_t symbol_id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);

    auto it = m_repairman_map.find(symbol_id);
    if (it == m_repairman_map.end())
    {
        OcQuoteDepthSnapRepairmanPtr repairman = std::make_shared<OcQuoteDepthSnapRepairman>(pData->exchange, pData->symbol);
        m_repairman_map[symbol_id] = repairman;
        return repairman;
    }
    return it->second;
};

void OcBookWorker::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    if (!m_id)
    {
        if ((pData->time - m_cout_time) > 1000)
        {
            printf("[OcBookWorker::OnL2FutureSnapshotRtn] %s, %s, %d, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, m_cout_time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_time = pData->time;
        }
    }
    m_quote_ipc_publisher->OnL2FutureSnapshotRtn(pData);
};

void OcBookWorker::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    printf("[OcBookWorker::OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
        pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
        
    if(m_book_engine){
        m_book_engine->RecvStockSnap(pData);
    }

    OcQuoteDepthSnapRepairmanPtr repairman = GetRepairman((TiQuoteSnapshotStockField*)pData);
    repairman->OnL2OriginalStockSnapshotRtn(pData);

    m_quote_ipc_publisher->OnL2StockSnapshotRtn(pData);
};
void OcBookWorker::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    m_book_engine->RecvMatch(pData);
};
void OcBookWorker::OnL2StockOrderRtn(const TiQuoteOrderField* pData){
    m_book_engine->RecvOrder(pData);
};

void OcBookWorker::OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
    const std::vector<TiQuoteDepthLevel*> &asks, 
    const std::vector<TiQuoteDepthLevel*> &bids)
{
    if (asks.size() < 10 || bids.size() < 10)
    {
        return;
    }
    strcpy(m_book_snap_cash.exchange, pBase->exchange);
    strcpy(m_book_snap_cash.symbol, pBase->symbol);
    m_book_snap_cash.date = pBase->date;
    m_book_snap_cash.time = pBase->time;
    m_book_snap_cash.timestamp = pBase->timestamp;
    m_book_snap_cash.last = pBase->last;
    m_book_snap_cash.pre_close = pBase->pre_close;
    m_book_snap_cash.open = pBase->open;
    m_book_snap_cash.high = pBase->high;
    m_book_snap_cash.low = pBase->low;
    m_book_snap_cash.high_limit = pBase->high_limit;
    m_book_snap_cash.low_limit = pBase->low_limit;
    m_book_snap_cash.acc_volume = pBase->acc_volume;
    m_book_snap_cash.acc_turnover = pBase->acc_turnover;
    m_book_snap_cash.match_items = pBase->match_items;

    
    for (size_t i = 0; i < 10; i++)
    {
        m_book_snap_cash.ask_price[i] = asks[i]->price;
        m_book_snap_cash.ask_volume[i] = asks[i]->volume;
        m_book_snap_cash.ask_order_num[i] = asks[i]->order_count;
        m_book_snap_cash.bid_price[i] = bids[i]->price;
        m_book_snap_cash.bid_volume[i] = bids[i]->volume;
        m_book_snap_cash.bid_order_num[i] = bids[i]->order_count;
    }

    OcQuoteDepthSnapRepairmanPtr repairman = GetRepairman(&m_book_snap_cash);
    bool fitted = repairman->OnL2FittedStockSnapshotRtn(&m_book_snap_cash, &m_book_fitted_snap_cash);

    if (fitted)
    {
        OnDepthSnapshotRtn(&m_book_fitted_snap_cash);
        m_quote_ipc_publisher->OnL2StockSnapshotRtn(&m_book_fitted_snap_cash);
    }else{
        OnDepthSnapshotRtn(&m_book_snap_cash);
        m_quote_ipc_publisher->OnL2StockSnapshotRtn(&m_book_snap_cash);
    }
};

void OcBookWorker::OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData)
{
    m_quote_ipc_publisher->OnDepthOrderBookLevelRtn(pData);
};

void OcBookWorker::OnDepthSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    printf("[OcBookWorker::OnDepthSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
        pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
    //OcQuoteDepthSnapRepairmanPtr repairman = GetRepairman((TiQuoteSnapshotStockField* )pData);
    //repairman->OnL2OriginalStockSnapshotRtn(pData);
    //m_callback->OnL2StockSnapshotRtn(pData);
};