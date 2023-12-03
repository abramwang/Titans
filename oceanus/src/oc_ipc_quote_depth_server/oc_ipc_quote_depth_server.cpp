#include "oc_ipc_quote_depth_server.h"

OcIpcQuoteDepthServer::OcIpcQuoteDepthServer(){
    m_cout_time = 0;
    m_quote_ipc_publisher = new TiQuoteIpcPublisher("quote_depth_data");
    for(int32_t i = 0; i < 6; i++){
        //TiQuoteIpcPublisher* quote_ipc_publisher = new TiQuoteIpcPublisher("quote_depth_data");
        OcBookWorker* worker = new OcBookWorker(i, m_quote_ipc_publisher, m_quote_ipc_publisher);
        m_worker_vec.push_back(worker);
    }
    m_quote_ipc_publisher->start();
    m_quote_worker_pool = new TiQuoteWorkerPool(m_worker_vec);
    m_quote_worker_pool->start();
};

void OcIpcQuoteDepthServer::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    if ((pData->time - m_cout_time) > 5000)
    {
        printf("[OnL2FutureSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
            pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
        m_cout_time = pData->time;
    }
    m_quote_worker_pool->OnL2FutureSnapshotRtn(pData);
};
void OcIpcQuoteDepthServer::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    if ((pData->time - m_cout_time) > 5000)
    {
        printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
            pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
        m_cout_time = pData->time;
    }
    m_quote_worker_pool->OnL2StockSnapshotRtn(pData);
};
void OcIpcQuoteDepthServer::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    m_quote_worker_pool->OnL2StockMatchesRtn(pData);
};
void OcIpcQuoteDepthServer::OnL2StockOrderRtn(const TiQuoteOrderField* pData){
    m_quote_worker_pool->OnL2StockOrderRtn(pData);
};