#include "oc_quote_ipc_server.h"

OcQuoteIpcServer::OcQuoteIpcServer(){
    //sprintf(m_topic, "ctp_xt_quote_data");
    sprintf(m_topic, "quote_data");
    memset(m_buffer, 0, TI_QUOTE_CACHE_MAX_LEN);
    m_cache = new TiQuoteCache(m_buffer, TI_QUOTE_CACHE_MAX_LEN);
    m_last_pub_time = 0;
};

OcQuoteIpcServer::~OcQuoteIpcServer(){
    
};

void OcQuoteIpcServer::try_ipc_pub(){
    int64_t time = datetime::get_now_timestamp_ms();
    if ((time - m_last_pub_time) > 20)
    {
        if (m_cache->m_used)
        {
            ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
            m_cache->reset();
        }
        m_last_pub_time = time;
    }
};

void OcQuoteIpcServer::ipc_pub(){
    int64_t time = datetime::get_now_timestamp_ms();
    ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
    m_cache->reset();
    m_last_pub_time = time;
};


void OcQuoteIpcServer::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData)
{
    Locker locker(&m_mutex);
    if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_INDEX, (void *)pData, sizeof(TiQuoteSnapshotIndexField)))
    {
        ipc_pub();
        m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_INDEX, (void *)pData, sizeof(TiQuoteSnapshotIndexField));
    }else{
        try_ipc_pub();
    }
};

void OcQuoteIpcServer::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    Locker locker(&m_mutex);
    if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE, (void *)pData, sizeof(TiQuoteSnapshotFutureField)))
    {
        ipc_pub();
        m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE, (void *)pData, sizeof(TiQuoteSnapshotFutureField));
    }else{
        try_ipc_pub();
    }
};

void OcQuoteIpcServer::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    Locker locker(&m_mutex);
    if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField)))
    {
        ipc_pub();
        m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField));
    }else{
        try_ipc_pub();
    }
};
