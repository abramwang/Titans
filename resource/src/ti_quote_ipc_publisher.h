#ifndef TI_QUOTE_IPC_PUBLISHER_H
#define TI_QUOTE_IPC_PUBLISHER_H

#include <mutex>
#include <string.h>
#include "concurrentqueue.h"
#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_ipc_api.h"
#include "ti_thread.h"
#include "ti_quote_wrap.h"
#include "ti_quote_cache.h"
#include "datetime.h"

class TiQuoteIpcPublisher 
    : public TiThread, public TiQuoteCallback, public TiQuoteDepthCallback
{
public:
    TiQuoteIpcPublisher(const char* topic);
    virtual ~TiQuoteIpcPublisher();
private:    
    char m_topic[64];
    void* m_publisher;
    TI_QuoteCacheBufferType m_buffer;
    TiQuoteCache* m_cache;
    int64_t m_last_pub_time;

    moodycamel::ConcurrentQueue<char*>  m_queue;
    uv_async_t*                         m_pub_signal;

private:
    static void pub_data_cb(uv_async_t* handle);
    void try_ipc_pub();
    void ipc_pub();
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData);

    
    virtual void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids){};
    
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData);

};

#endif