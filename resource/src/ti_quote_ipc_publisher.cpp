#include "ti_quote_ipc_publisher.h"

TiQuoteIpcPublisher::TiQuoteIpcPublisher(const char* topic)
{
    strcpy(m_topic, topic);
    m_publisher = ti_ipc_get_publisher(topic);
    m_cache = new TiQuoteCache(m_buffer, TI_QUOTE_CACHE_MAX_LEN);
    m_last_pub_time = datetime::get_now_timestamp_ms();

    m_pub_signal = new uv_async_t;
    m_pub_signal->data = this;
    uv_async_init(m_loop, m_pub_signal, pub_data_cb);
};

TiQuoteIpcPublisher::~TiQuoteIpcPublisher()
{
    delete m_cache;

    uv_close((uv_handle_t*)m_pub_signal, NULL);
    delete m_pub_signal;
};

void TiQuoteIpcPublisher::pub_data_cb(uv_async_t* handle)
{
    TiQuoteIpcPublisher* p = (TiQuoteIpcPublisher*)handle->data;
    char* buff = NULL;
    while (p->m_queue.try_dequeue(buff))
    {
        if (buff)
        {   
            TI_QUOTE_DATA_TYPE type = TiQuoteWrap::GetDataType(buff);
            void* pData = (void*)(buff + 1);
            size_t len = 0;

            switch (type)
            {
            case TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK:
            {
                len = sizeof(TiQuoteSnapshotStockField);
                break;
            }
            case TI_QUOTE_DATA_TYPE_MATCH:
            {
                len = sizeof(TiQuoteMatchesField);
                break;
            }
            case TI_QUOTE_DATA_TYPE_ORDER:
            {
                len = sizeof(TiQuoteOrderField);
                break;
            }
            case TI_QUOTE_DATA_TYPE_ORDERBOOK_LEVEL:
            {
                len = sizeof(TiQuoteOrderBookField);
                break;
            }
            default:
                break;
            }

            if(!p->m_cache->try_addData(type, pData, len))
            {
                p->ipc_pub();
                p->m_cache->try_addData(type, pData, len);
            }else{
                p->try_ipc_pub();
            }
            delete[] buff;
            buff = NULL;
        }
    }
};


void TiQuoteIpcPublisher::try_ipc_pub(){
    int64_t time = datetime::get_now_timestamp_ms();
    if ((time - m_last_pub_time) > 20)
    {
        if (m_cache->m_used)
        {
            ipc_pub();
        }
        m_last_pub_time = time;
    }
};

void TiQuoteIpcPublisher::ipc_pub(){
//    ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
    ti_ipc_pub_by_publisher(m_publisher, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
    m_cache->reset();
};

void TiQuoteIpcPublisher::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    char* warp = TiQuoteWrap::CreatWrap(pData);
    m_queue.enqueue(warp);
    uv_async_send(m_pub_signal);
};

void TiQuoteIpcPublisher::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    char* warp = TiQuoteWrap::CreatWrap(pData);
    m_queue.enqueue(warp);
    uv_async_send(m_pub_signal);
};

void TiQuoteIpcPublisher::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData)
{
    char* warp = TiQuoteWrap::CreatWrap(pData);
    m_queue.enqueue(warp);
    uv_async_send(m_pub_signal);
};

void TiQuoteIpcPublisher::OnL2StockOrderRtn(const TiQuoteOrderField* pData)
{
    char* warp = TiQuoteWrap::CreatWrap(pData);
    m_queue.enqueue(warp);
    uv_async_send(m_pub_signal);
};


void TiQuoteIpcPublisher::OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData)
{
    char* warp = TiQuoteWrap::CreatWrap(pData);
    m_queue.enqueue(warp);
    uv_async_send(m_pub_signal);
};