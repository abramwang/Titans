#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>
#include <thread>
#include <mutex>

#include "ti_ax_quote_client.h"
#include "ti_quote_cache.h"
#include "ti_quote_callback.h"
#include "ti_ipc_api.h"
#include "datetime.h"

#include "redis_sync_handle.h"

#include "ti_quote_formater.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

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
    RedisSyncHandle m_redis;
    
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

        m_client = new TiAxQuoteClient("./config.ini", this);;
        m_redis.connect("47.103.74.35", 20184, "W_P!ViW+d7xAFept6");    
    };
    virtual ~Callback(){};
    
    TiAxQuoteClient* m_client;
public:
    virtual void OnEventRtn(TI_QUOTE_EVENT_TYPE event){
        return;
        //getCodeList();
        json etf_code;
        bool flag = m_client->getETFCodeList(etf_code);
        if (!flag)
        {
            printf("getETFCodeList error\n");
            return;
        }
        for (auto iter = etf_code.begin(); iter != etf_code.end(); iter++)
        {
            m_redis.hset("etf_factor.disclosure_info.anxin", (*iter)["symbol"].dump().c_str(), (*iter).dump().c_str());
        }        
        m_redis.hset("task_worked_time", "etf_factor.disclosure_info.anxin", datetime::get_format_now_time_ms().c_str());
    };

    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){
        

    };
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};

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


int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    Callback cb;
     
	cb.m_client->connect();

    while (1)
    {
        usleep(1000000);
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        std::cout << "当前时间: "
              << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
              << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec
              << std::endl;
        if (localTime->tm_hour >16 )
        {
            return 0;
        }
    }
    
    return 0;
}
