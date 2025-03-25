#include "oc_ipc_quote_depth_server.h"

#define _ENABLE_WORKER_POOL 0

OcIpcQuoteDepthServer::OcIpcQuoteDepthServer(uv_loop_s* loop, int threadpool_size, int core_id){
    m_cout_time = 0;

    if (_ENABLE_WORKER_POOL) {
        m_quote_ipc_publisher = new TiQuoteIpcPublisher("quote_depth_data");
        for(int32_t i = 0; i < threadpool_size; i++){
            //TiQuoteIpcPublisher* quote_ipc_publisher = new TiQuoteIpcPublisher("quote_depth_data");
            OcBookWorker* worker = new OcBookWorker(i, m_quote_ipc_publisher);
            m_worker_vec.push_back(worker);
        }
        m_quote_ipc_publisher->start();

        if (core_id > 0)
        {
            m_quote_worker_pool = new TiQuoteWorkerPool(m_worker_vec, core_id);
        }else{
            m_quote_worker_pool = new TiQuoteWorkerPool(m_worker_vec);
        }
        m_quote_worker_pool->start();
    }else{
        m_quote_ipc_publisher = new TiQuoteIpcPublisher("quote_depth_data");
        m_worker = new OcBookWorker(0, m_quote_ipc_publisher);
        m_quote_ipc_publisher->start();
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);
};

void OcIpcQuoteDepthServer::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    if ((pData->time - m_cout_time) > 5000)
    {
        printf("[OnL2FutureSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
            pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
        m_cout_time = pData->time;
    }
    if (_ENABLE_WORKER_POOL)
    {
        m_quote_worker_pool->OnL2FutureSnapshotRtn(pData);
    }else{
        m_worker->OnL2FutureSnapshotRtn(pData);
    }
    
};
void OcIpcQuoteDepthServer::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    if ((pData->time - m_cout_time) > 5000)
    {
        printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
            pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
        m_cout_time = pData->time;
    }
    if (_ENABLE_WORKER_POOL)
    {
        m_quote_worker_pool->OnL2StockSnapshotRtn(pData);
    }else{
        m_worker->OnL2StockSnapshotRtn(pData);
    }
};
void OcIpcQuoteDepthServer::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    if (_ENABLE_WORKER_POOL)
    {
        m_quote_worker_pool->OnL2StockMatchesRtn(pData);
    }else{
        m_worker->OnL2StockMatchesRtn(pData);
    }
};
void OcIpcQuoteDepthServer::OnL2StockOrderRtn(const TiQuoteOrderField* pData){
    if (_ENABLE_WORKER_POOL)
    {
        m_quote_worker_pool->OnL2StockOrderRtn(pData);
    }else{
        m_worker->OnL2StockOrderRtn(pData);
    }
};


void OcIpcQuoteDepthServer::onTimer(uv_timer_t* handle)
{
    OcIpcQuoteDepthServer* pThis = (OcIpcQuoteDepthServer*)handle->data;
    pThis->OnTimer();
};

void OcIpcQuoteDepthServer::OnTimer()
{
    return;
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    /*
    std::cout << "当前时间: "
            << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
            << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec
            << std::endl;
    */
    if (localTime->tm_hour >= 15)
    {
        if (localTime->tm_hour == 15 && localTime->tm_min < 30)
        {
            return;
        }
        printf("terminate\n");
        std::terminate();
    }
};
