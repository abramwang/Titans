#ifndef __OC_QUOTE_IPC_SERVER_H__
#define __OC_QUOTE_IPC_SERVER_H__
#include <string.h>
#include <mutex>
#include "ti_quote_cache.h"
#include "ti_quote_callback.h"
#include "ti_ipc_api.h"
#include "datetime.h"

class OcQuoteIpcServer: public TiQuoteCallback
{
    class Locker
    {
    private:
        std::mutex* m_mutex;
    public:
        Locker(std::mutex* mutex)
        {
            m_mutex = mutex;
            m_mutex->lock();
        };
        ~Locker()
        {
            m_mutex->unlock();
        };
    };
public:
    OcQuoteIpcServer();
    virtual ~OcQuoteIpcServer();

protected:
    char m_topic[64];
    TI_QuoteCacheBufferType m_buffer;
    TiQuoteCache* m_cache;
    int64_t m_last_pub_time;
    
    std::mutex m_mutex;
    
    void try_ipc_pub();
    void ipc_pub();
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};
};


#endif // __OC_QUOTE_IPC_SERVER_H__