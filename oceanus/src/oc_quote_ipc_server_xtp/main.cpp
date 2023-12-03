#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>
#include <thread>
#include <mutex>

#include "ti_zt_quote_client.h"
#include "ti_quote_cache.h"
#include "ti_quote_callback.h"
#include "ti_ipc_api.h"
#include "datetime.h"

class Callback: public TiQuoteCallback
{
protected:
    char m_topic[64];
    TI_QuoteCacheBufferType m_buffer;
    std::mutex m_mutex;
    TiQuoteCache* m_cache;
    int64_t m_last_pub_time;
    int64_t m_cout_time;
    
    void try_ipc_pub(){
        int64_t time = datetime::get_now_timestamp_ms();
        if ((time - m_last_pub_time) > 20)
        {
            if (m_cache->m_used)
            {
                int flag = ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
                //std::cout << "try_ipc_pub:" << flag << std::endl;
                m_cache->reset();
            }
            m_last_pub_time = time;
        }
    };
    void ipc_pub(){
        int64_t time = datetime::get_now_timestamp_ms();
        int flag = ti_ipc_pub(m_topic, m_buffer, TI_QUOTE_CACHE_MAX_LEN);
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
        m_cout_time = 0;
    };
    virtual ~Callback(){};
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
        m_mutex.lock();
        if ((pData->time - m_cout_time) > 5000)
        {
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %ld\n", 
                pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_time = pData->time;
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


int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    Callback* cb = new Callback();
    TiZtQuoteClient client("./config.ini", cb);

	client.connect();

    char* shCodeList[3] = {"600000", "600004", "600008"};
    char* szCodeList[3] = {"000001", "000008", "002001"};
    client.subData(NULL, shCodeList, 0);
    //client.subData("SH", shCodeList, 3);
    //client.subData("SZ", szCodeList, 3);

    while (1)
    {
        usleep(1000);
    }
    
    return 0;
}
