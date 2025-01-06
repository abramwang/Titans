#include <cstddef>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <glog/logging.h>

#include "ti_df_quote_client.h"
#include "ti_df_trader_client.h"

#include "ti_quote_cache.h"
#include "ti_quote_callback.h"
#include "ti_ipc_api.h"
#include "ti_quote_formater.h"
#include "datetime.h"

class Callback: public TiQuoteCallback
{
protected:
    char m_topic[64];
    TI_QuoteCacheBufferType m_buffer;
    std::mutex m_mutex;
    TiQuoteCache* m_cache;
    int64_t m_last_pub_time;
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
                //std::cout << "try_ipc_pub:" << flag << std::endl;
                m_cache->reset();
            }
            m_last_pub_time = time;
        }
    };
    void ipc_pub(){
        int64_t time = datetime::get_now_timestamp_ms();
        ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
        //std::cout << "ipc_pub:" <<  flag <<std::endl;
        m_cache->reset();
        m_last_pub_time = time;
    };

public:
    Callback(){
        sprintf(m_topic, "quote_data");
        memset(m_buffer, 0, TI_QUOTE_CACHE_MAX_LEN);
        m_cache = new TiQuoteCache(m_buffer, TI_QUOTE_CACHE_MAX_LEN);
        m_last_pub_time = 0;
        m_cout_time_snap = 0;
        m_cout_time_trans = 0;
        m_cout_time_order = 0;
    };
    virtual ~Callback(){};
public:
    virtual void OnEventRtn(TI_QUOTE_EVENT_TYPE event){
        return;
    };

    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){
        

    };
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){
        m_mutex.lock();
        if ((pData->time - m_cout_time_snap) > 5000)
        {
            printf("[OnL2FutureSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
                pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
            /*
            json j;
            TiQuoteFormater::FormatSnapshot(pData, j);
            printf("[OnL2FutureSnapshotRtn] %s\n", j.dump().c_str());
            */
            m_cout_time_snap = pData->time;
        }
        //return;
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE, (void *)pData, sizeof(TiQuoteSnapshotFutureField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE, (void *)pData, sizeof(TiQuoteSnapshotFutureField));
        }else{
            try_ipc_pub();
        }
        m_mutex.unlock();
    };

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
        m_mutex.lock();
        if ((pData->time - m_cout_time_snap) > 5000)
        {
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
                pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
            json j;
            TiQuoteFormater::FormatSnapshot(pData, j);
            //printf("[OnMDSnapshot] %s\n", out);
            printf("[OnL2StockSnapshotRtn] %s\n", j.dump().c_str());
            m_cout_time_snap = pData->time;
        }
        //return;
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField));
        }else{
            try_ipc_pub();
        }
        m_mutex.unlock();
    };
    
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
        //return;
        m_mutex.lock();
        if ((pData->time - m_cout_time_trans) > 5000)
        {
            json j;
            TiQuoteFormater::FormatMatch(pData, j);
            //printf("[OnMDTickExecution] %s\n", out);
            printf("[OnL2StockMatchesRtn] %s\n", j.dump().c_str());
            m_cout_time_trans = pData->time;
        }
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField));
        }else{
            try_ipc_pub();
        }
        m_mutex.unlock();
    };
    
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){
        //return;
        m_mutex.lock();
        if ((pData->time - m_cout_time_order) > 5000)
        {
            json j;
            TiQuoteFormater::FormatOrder(pData, j);
            //printf("[OnMDTickOrder] %s\n", out);
            printf("[OnL2StockOrderRtn] %s\n", j.dump().c_str());
            m_cout_time_order = pData->time;
        }
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField));
        }else{
            try_ipc_pub();
        }
        m_mutex.unlock();
    };
};

int main(int argc, char *argv[]) {
    Callback cb;
    TiDfQuoteClient client("./config.ini", &cb);
    client.connect();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
