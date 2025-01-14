#ifndef __TI_QUOTE_WORKER_H__
#define __TI_QUOTE_WORKER_H__

#include <unordered_set>
#include <uv.h>
#include "concurrentqueue.h"
#include "ti_quote_wrap.h"
#include "ti_quote_cache.h"
#include "ti_thread.h"
#include "ti_quote_callback.h"
#include "datetime.h"

class TiQuoteWorker 
    : public TiThread, public TiQuoteCallback
{
public:
    TiQuoteWorker(TiQuoteCallback* callback);
    TiQuoteWorker(TiQuoteCallback* callback, int32_t core_id);
    virtual ~TiQuoteWorker();
public:
    int32_t                 m_core_id;
    TiQuoteCallback*        m_callback;
    uv_async_t*             m_recv_signal;
    TI_QuoteCacheBufferType m_buffer;
    TiQuoteCache*           m_save_cache;
    TiQuoteCache*           m_read_cache;
    moodycamel::ConcurrentQueue<void*> m_queue;
    int64_t                 m_send_time;

    static void recv_data_cb(uv_async_t* handle);

    void send_data();
    void send_data_freq();
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData);

};

#endif