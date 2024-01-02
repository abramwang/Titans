
#include "datetime.h"
#include <iostream>
#include <unistd.h>
#include <thread>

#include "ti_quote_cache.h"
#include "ti_quote_history_client_local.h"
#include "ti_ipc_api.h"

class Callback: public TiQuoteCallback
{
private:
    char m_topic[64];
    TI_QuoteCacheBufferType m_buffer;
    TiQuoteCache* m_cache;
    
    int64_t m_last_pub_time;
    int32_t m_cout_time;
    int32_t m_pre_time;
    int32_t m_sleep_speed;
public:
    Callback(int32_t sleep_speed = 0){
        sprintf(m_topic, "quote_data");
        ti_ipc_destroy(m_topic);
        memset(m_buffer, 0, TI_QUOTE_CACHE_MAX_LEN);
        m_cache = new TiQuoteCache(m_buffer, TI_QUOTE_CACHE_MAX_LEN);
        m_last_pub_time = 0;
        m_cout_time = 0;
        m_pre_time = 0;
        m_sleep_speed = sleep_speed;
    };
    virtual ~Callback(){};

    void ipc_pub(){
        int64_t time = datetime::get_now_timestamp_ms();
        ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
        m_cache->reset();
        m_last_pub_time = time;
    };
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
    void sleep(int32_t time){
        if (m_sleep_speed)
        {
            int32_t sleep_time = time - m_pre_time;
            m_pre_time = time;
            if (sleep_time > 0 && sleep_time < 10000)
            {
                
                if (sleep_time < 1000)
                {
                    usleep(sleep_time*1000/m_sleep_speed);
                }else{
                    usleep(1000*1000/m_sleep_speed);
                }
            }
        }
    };
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
        if ((pData->time - m_cout_time) > 5000)
        {
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
                pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_time = pData->time;
        }
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, (void *)pData, sizeof(TiQuoteSnapshotStockField));
        }else{
            try_ipc_pub();
        }
        sleep(pData->time);
    };
    
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_MATCH, (void *)pData, sizeof(TiQuoteMatchesField));
        }else{
            try_ipc_pub();
        }
    };
    
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){
        if(!m_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField)))
        {
            ipc_pub();
            m_cache->try_addData(TI_QUOTE_DATA_TYPE_ORDER, (void *)pData, sizeof(TiQuoteOrderField));
        }else{
            try_ipc_pub();
        }
    };
};

int set_thread_pool_num(){
    char value[32] = {0};
    int kernel_num = std::thread::hardware_concurrency();
    kernel_num *= 0.6;
    if(kernel_num > 8)
        kernel_num = 8;
    sprintf(value, "%d", kernel_num);
    //sprintf(value, "%d", kernel_num);
    setenv("UV_THREADPOOL_SIZE", value, 0);
    char *p; 
    if((p = getenv("UV_THREADPOOL_SIZE")))
        printf("UV_THREADPOOL_SIZE = %s\n",p); 
    return kernel_num;
}


int main(int argc, char** argv) {
    set_thread_pool_num();

    int sleep_speed = 0;
    if (argc > 1)
    {
        sleep_speed = atoi(argv[1]);
    }

    int trading_day = 20231201;
    if (argc > 2)
    {
        trading_day = atoi(argv[2]);
    }

    Callback cb(sleep_speed);
    TiQuoteHistoryClientLocal client("./config.ini", &cb);

    TI_QUOTE_DATA_TYPE dataTypeList[3] = {TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, TI_QUOTE_DATA_TYPE_MATCH, TI_QUOTE_DATA_TYPE_ORDER};

    client.replay(trading_day, dataTypeList, 3);
    cb.ipc_pub();   //发送缓存中最后的数据
}