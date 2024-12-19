#include "redis_uv.h"
#include <cstring>
#include <unistd.h>
#include <mutex>

#include "ti_hx_quote_client.h"
#include "ti_quote_cache.h"
#include "ti_quote_callback.h"
#include "ti_ipc_api.h"
#include "datetime.h"

#include "ti_quote_formater.h"

#define LOG_ENABLE True

class Callback: public TiQuoteCallback
{
protected:
    char m_topic[64];
    TI_QuoteCacheBufferType m_buffer;
    std::mutex m_mutex;
    TiQuoteCache* m_cache;
    int64_t m_last_pub_time;
    int64_t m_cout_time_index;
    int64_t m_cout_time_snap;
    int64_t m_cout_time_trans;
    int64_t m_cout_time_order;

    void try_ipc_pub(){
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
    void ipc_pub(){
        int64_t time = datetime::get_now_timestamp_ms();
        ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
        m_cache->reset();
        m_last_pub_time = time;
    };
public:
    Callback(){
        sprintf(m_topic, "quote_data");
        memset(m_buffer, 0, TI_QUOTE_CACHE_MAX_LEN);
        m_cache = new TiQuoteCache(m_buffer, TI_QUOTE_CACHE_MAX_LEN);
        m_last_pub_time = 0;
        m_cout_time_index = 0;
        m_cout_time_snap = 0;
        m_cout_time_trans = 0;
        m_cout_time_order = 0;
    };
    virtual ~Callback(){
        client = NULL;
    };
public:
    TiHxQuoteClient* client;

public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){
        printf("[OnTradingDayRtn] %d, %s\n", day, exchangeName);

        /*
        char* shCodeList[3] = {"600000"};
        if(client)
        {
            client->subData("SH", shCodeList, 1);
        }
        return;
        */
        ///*
        {
            char* shCodeList[3] = {"600***", "688***", "51****"};
            if(client)
            {
                client->subData("SH", shCodeList, 3);
            }
        }
        //*/
        
        ///*
        {
            char* shCodeList[3] = {"000***", "300***", "15****"};
            if(client)
            {
                client->subData("SZ", shCodeList, 3);
            }
        }
        //*/
    };
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
        //m_mutex.lock();
        if ((pData->time - m_cout_time_snap) > 5000)
        {
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
                pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
            /*
            json j;
            TiQuoteFormater::FormatSnapshot(pData, j);
            //printf("[OnMDSnapshot] %s\n", out);
            printf("[OnL2StockSnapshotRtn] %s\n", j.dump().c_str());
            */
            m_cout_time_snap = pData->time;
        }
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField));
        }else{
            try_ipc_pub();
        }
        //m_mutex.unlock();
    };
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
        //return;
        //m_mutex.lock();
        if ((pData->time - m_cout_time_trans) > 5000)
        {
            json j;
            TiQuoteFormater::FormatMatch(pData, j);
            //printf("[OnMDTickExecution] %s\n", out);
            printf("[OnL2StockMatchesRtn] %s\n", j.dump().c_str());
            m_cout_time_trans = pData->time;
        }
        //return;
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField));
        }else{
            try_ipc_pub();
        }
        //m_mutex.unlock();
    };
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){
        //m_mutex.lock();
        if ((pData->time - m_cout_time_order) > 5000)
        {
            json j;
            TiQuoteFormater::FormatOrder(pData, j);
            //printf("[OnMDTickOrder] %s\n", out);
            printf("[OnL2StockOrderRtn] %s\n", j.dump().c_str());
            m_cout_time_order = pData->time;
        }
        //return;
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField));
        }else{
            try_ipc_pub();
        }
        //m_mutex.unlock();
    };

};

int main()
{
    Callback* cb = new Callback();

    TiHxQuoteClient api("./config.ini", cb);
    cb->client = &api;

    api.connect();

    while (1)
    {
        usleep(1000000);
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        if (localTime->tm_hour > 16 )
        {
            return 0;
        }
    }
    
    return 0;
}
