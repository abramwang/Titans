#include "ti_quote_worker.h"
#include <string.h>
#include <iostream>

TiQuoteWorker::TiQuoteWorker(TiQuoteCallback* callback)
{
    m_callback = callback;
    m_recv_signal = new uv_async_t;
    m_recv_signal->data = this;
    uv_async_init(m_loop, m_recv_signal, recv_data_cb);
    memset(m_buffer, 0, TI_QUOTE_CACHE_MAX_LEN);
    m_save_cache = new TiQuoteCache(m_buffer, TI_QUOTE_CACHE_MAX_LEN);
    m_read_cache = new TiQuoteCache();
    m_send_time = datetime::get_now_timestamp_ms();
}

TiQuoteWorker::~TiQuoteWorker()
{
    uv_close((uv_handle_t*)m_recv_signal, NULL);
    delete m_recv_signal;
}

void TiQuoteWorker::recv_data_cb(uv_async_t* handle)
{
    TiQuoteWorker* p = (TiQuoteWorker*)handle->data;
    void* buff = NULL;
    while (p->m_queue.try_dequeue(buff))
    {
        p->m_read_cache->reset_buffer(buff, TI_QUOTE_CACHE_MAX_LEN);
        TiQuoteCache::DataPtr* dataPtr = NULL;
        while (p->m_read_cache->try_getNext(dataPtr))
        {
            switch (dataPtr->dataType)
            {
            case TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK:
            {
                TiQuoteSnapshotStockField* pData = (TiQuoteSnapshotStockField*)dataPtr->data;
                p->m_callback->OnL2StockSnapshotRtn(pData);
                break;
            }
            case TI_QUOTE_DATA_TYPE_MATCH:
            {
                TiQuoteMatchesField* pData = (TiQuoteMatchesField*)dataPtr->data;
                p->m_callback->OnL2StockMatchesRtn(pData);
                break;
            }
            case TI_QUOTE_DATA_TYPE_ORDER:
            {
                TiQuoteOrderField* pData = (TiQuoteOrderField*)dataPtr->data;
                p->m_callback->OnL2StockOrderRtn(pData);
                break;
            }
            default:
                break;
            }
        }
        if (buff)
        {
            free(buff);
            buff = NULL;
        }
    }
};


void TiQuoteWorker::send_data()
{
    void* buffer = malloc(TI_QUOTE_CACHE_MAX_LEN);
    memcpy(buffer, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
    m_queue.enqueue(buffer);
    m_save_cache->reset();
    uv_async_send(m_recv_signal);
    m_send_time = datetime::get_now_timestamp_ms();
};

void TiQuoteWorker::send_data_freq()
{
    int64_t now = datetime::get_now_timestamp_ms();
    if (now - m_send_time > 10)
    {
        send_data();
    }
};

void TiQuoteWorker::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    if(!m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE, (void *)pData, sizeof(TiQuoteSnapshotFutureField)))
    {
        send_data();
        m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE, (void *)pData, sizeof(TiQuoteSnapshotFutureField));
    }else{
        send_data_freq();
    }
};

void TiQuoteWorker::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    if(!m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField)))
    {
        send_data();
        m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField));
    }else{
        send_data_freq();
    }
};

void TiQuoteWorker::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData)
{
    if(!m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField)))
    {
        send_data();
        m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField));
    }else{
        send_data_freq();
    }
};

void TiQuoteWorker::OnL2StockOrderRtn(const TiQuoteOrderField* pData)
{
    if(!m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField)))
    {
        send_data();
        m_save_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField));
    }else{
        send_data_freq();
    }
};