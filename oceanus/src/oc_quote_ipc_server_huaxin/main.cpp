#include <fstream>
#include <string>
#include <iomanip> // for std::setprecision
#include <cstring>
#include <unistd.h>
#include <mutex>
#include <vector>
#include <glog/logging.h>

#include "redis_uv.h"
#include "ti_hx_quote_client.h"
#include "ti_quote_cache.h"
#include "ti_quote_callback.h"
#include "ti_ipc_api.h"
#include "datetime.h"
#include "redis_sync_handle.h"

#include "ti_quote_formater.h"
#include "iniFile.h"

#define Enable_QuoteDataOutput 0

class Callback: public TiQuoteCallback
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        std::string m_sub_symbol_key;               //订阅的股票代码存放的key
    } ConfigInfo;
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

    ConfigInfo* m_config;
    RedisSyncHandle m_redis;
    std::vector<std::string> m_sub_symbol_vec;     //订阅的股票代码集合

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
    Callback(std::string configPath = "./config.ini"){
        sprintf(m_topic, "quote_data");
        memset(m_buffer, 0, TI_QUOTE_CACHE_MAX_LEN);
        m_cache = new TiQuoteCache(m_buffer, TI_QUOTE_CACHE_MAX_LEN);
        m_last_pub_time = 0;
        m_cout_time_index = 0;
        m_cout_time_snap = 0;
        m_cout_time_trans = 0;
        m_cout_time_order = 0;
        loadConfig(configPath);
        loadSubSymbol();
    };
    virtual ~Callback(){
        client = NULL;
    };
public:
    TiHxQuoteClient* client;
    
    int loadConfig(std::string iniFileName){
        if(m_config){
            return -1;
        }

        IniFile _iniFile;
        _iniFile.load(iniFileName);

        m_config = new ConfigInfo();

        m_config->szIp        = string(_iniFile["oc_quote_ipc_server_huaxin"]["ip"]);
        m_config->nPort       = _iniFile["oc_quote_ipc_server_huaxin"]["port"];
        m_config->szAuth      = string(_iniFile["oc_quote_ipc_server_huaxin"]["auth"]);

        m_config->m_sub_symbol_key      = string(_iniFile["oc_quote_ipc_server_huaxin"]["sub_symbol_key"]);

        if( m_config->szIp.empty() |
            !m_config->nPort)
        {
            delete m_config;
            m_config = NULL;
            return -1;
        }
        return 0;
    };

    void loadSubSymbol(){
        if(m_config){
            bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
            std::cout << "[loadSubSymbol] flag: " << flag;

            std::string sub_symbol_key = m_config->m_sub_symbol_key;
            if (!sub_symbol_key.empty())
            {
                m_redis.smembers(sub_symbol_key.c_str(), m_sub_symbol_vec);
            }
        }
    };
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){
        printf("[OnTradingDayRtn] %d, %s\n", day, exchangeName);
        //return;
        if(m_sub_symbol_vec.size() > 0){
            char* codeList[m_sub_symbol_vec.size()];
            for (int i = 0; i < m_sub_symbol_vec.size(); i++)
            {
                codeList[i] = (char*)m_sub_symbol_vec[i].c_str();
            }
            if(client)
            {
                client->subData("SH", codeList, m_sub_symbol_vec.size());
                client->subData("SZ", codeList, m_sub_symbol_vec.size());
            }
        }else{
            {
                char* shCodeList[3] = {"600***", "688***", "51****"};
                if(client)
                {
                    client->subData("SH", shCodeList, 3);
                }
            }
            
            {
                char* shCodeList[3] = {"000***", "300***", "15****"};
                if(client)
                {
                    client->subData("SZ", shCodeList, 3);
                }
            }
        }
        /*
        char* shCodeList[3] = {"600000"};
        if(client)
        {
            client->subData("SH", shCodeList, 1);
        }
        return;
        */
    };
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
        //m_mutex.lock();
        if ((pData->time - m_cout_time_snap) > 5000)
        {
            printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f, %f, %f\n", 
                pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover, pData->high_limit, pData->low_limit);
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
    #if Enable_QuoteDataOutput
        try {
            WriteToCSV(*pData, "quote_matches_data.csv");
        } catch (const std::exception& e) {
            // 捕获异常并打印错误
            std::cerr << "Error writing to CSV: " << e.what() << std::endl;
        }
    #endif
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
    #if Enable_QuoteDataOutput
        try {
            WriteToCSV(*pData, "quote_order_data.csv");
        } catch (const std::exception& e) {
            // 捕获异常并打印错误
            std::cerr << "Error writing to CSV: " << e.what() << std::endl;
        }
    #endif
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

    // 定义一个函数，将结构体数据写入 CSV 文件
    void WriteToCSV(const TiQuoteOrderField& data, const std::string& fileName) {
        // 打开文件（如果文件不存在，会创建新文件）
        std::ofstream file;
        file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + fileName);
        }

        // 如果是新文件，写入标题行
        static bool isHeaderWritten = false;
        if (!isHeaderWritten) {
            file << "Exchange,Symbol,Date,Time,Timestamp,TimeStr,RecvTimeStr,RecvTimestamp,Channel,Seq,OrderOrino,Price,Volume,OrderType,FunctionCode" << std::endl;
            isHeaderWritten = true;
        }

        // 写入数据行
        file << data.exchange << ","
            << data.symbol << ","
            << data.date << ","
            << data.time << ","
            << data.timestamp << ","
            << data.time_str << ","
            << data.recv_time_str << ","
            << data.recv_timestamp << ","
            << data.channel << ","
            << data.seq << ","
            << data.order_orino << ","
            << std::fixed << std::setprecision(2) << data.price << ","
            << data.volume << ","
            << static_cast<char>(data.order_type) << ","
            << static_cast<char>(data.function_code) << std::endl;

        file.close();
    }

    void WriteToCSV(const TiQuoteMatchesField& data, const std::string& fileName) {
        // 打开文件（如果文件不存在，会创建新文件）
        std::ofstream file;
        file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + fileName);
        }

        // 如果是新文件，写入标题行
        static bool isHeaderWritten = false;
        if (!isHeaderWritten) {
            file << "Exchange,Symbol,Date,Time,Timestamp,TimeStr,RecvTimeStr,RecvTimestamp,Channel,Seq,Price,Volume,BsFlag,FunctionCode,AskOrderSeq,BidOrderSeq" << std::endl;
            isHeaderWritten = true;
        }

        // 写入数据行
        file << data.exchange << ","
            << data.symbol << ","
            << data.date << ","
            << data.time << ","
            << data.timestamp << ","
            << data.time_str << ","
            << data.recv_time_str << ","
            << data.recv_timestamp << ","
            << data.channel << ","
            << data.seq << ","
            << std::fixed << std::setprecision(2) << data.price << ","
            << data.volume << ","
            << static_cast<char>(data.bs_flag) << ","
            << static_cast<char>(data.function_code) << ","
            << data.ask_order_seq << ","
            << data.bid_order_seq << std::endl;

        file.close();
    }
};

int main(int argc, char* argv[])
{
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    Callback* cb = new Callback();

    TiHxQuoteClient api("./config.ini", cb);
    cb->client = &api;

    api.connect();

    while (1)
    {
        usleep(1000000);
        //continue;
        std::time_t currentTime = std::time(nullptr);
        std::tm* localTime = std::localtime(&currentTime);
        if (localTime->tm_hour > 16 )
        {
            return 0;
        }
    }
    
    return 0;
}
