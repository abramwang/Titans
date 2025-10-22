#include <iostream>
#include <unistd.h>
#include "oc_quote_ipc_server_ctp_redis_rq.h"
#include "iniFile.h"
#include "datetime.h"
#include "ti_quote_formater.h"
#include <glog/logging.h>
#include <chrono>

OcQuoteIpcServerCtpRedisRq::OcQuoteIpcServerCtpRedisRq(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_config = NULL;
    m_ctp_client = NULL;

    loadConfig(configPath);

    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[OcQuoteIpcServerCtpRedisRq] flag: " << flag;
        resetStreamKey();

        m_ctp_client = new TiCtpQuoteClient("./config.ini", &m_ipc_server);

        m_ctp_client->connect();
    }
    //*/

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);
}
OcQuoteIpcServerCtpRedisRq::~OcQuoteIpcServerCtpRedisRq(){
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    uv_timer_stop(&m_timer);
    uv_close((uv_handle_t*)&m_timer, NULL);
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void OcQuoteIpcServerCtpRedisRq::OnTimer()
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    /*
    std::cout << "当前时间: "
            << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
            << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec
            << std::endl;
    */

    return;
    if (localTime->tm_hour >= 15 )
    {
        if (localTime->tm_hour == 15 && localTime->tm_min < 30)
        {
            return;
        }
        std::cout << "terminate" << std::endl;
        std::terminate();
    }
};

void OcQuoteIpcServerCtpRedisRq::OnCommandRtn(const char* type, const char* command)
{
    json j = json::parse(command);

    TiQuoteSnapshotStockField stock = {0};
    
    // Parse datetime
    int64_t datetime_int = j["datetime"];
    int year = datetime_int / 10000000000000LL;
    int month = (datetime_int / 100000000000LL) % 100;
    int day = (datetime_int / 1000000000LL) % 100;
    int hour = (datetime_int / 10000000LL) % 100;
    int minute = (datetime_int / 100000LL) % 100;
    int second = (datetime_int / 1000LL) % 100;
    int millisecond = datetime_int % 1000;

    // Calculate Unix timestamp in milliseconds
    std::tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = minute;
    tm.tm_sec = second;
    std::time_t time_t = std::mktime(&tm);
    auto tp = std::chrono::system_clock::from_time_t(time_t);
    int64_t ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()).count();
    stock.timestamp = ms_since_epoch + millisecond;

    strcpy(stock.exchange, j["exchange"].get<std::string>().c_str());
    strcpy(stock.symbol, j["symbol"].get<std::string>().c_str());
    stock.last = j["last"];
    stock.pre_close = j["prev_close"];
    stock.open = j["open"];
    stock.high = j["high"];
    stock.low = j["low"];
    stock.high_limit = j["limit_up"];
    stock.low_limit = j["limit_down"];
    stock.volume = j["volume"];
    stock.acc_turnover = j["total_turnover"];
    stock.match_items = j["num_trades"];
    stock.iopv = j["iopv"];
    stock.pre_close_iopv = j["prev_iopv"];

    for(int i = 0; i < 5; i++){
        stock.ask_price[i] = j["ask"][i];
        stock.ask_volume[i] = j["ask_vol"][i];
        stock.bid_price[i] = j["bid"][i];
        stock.bid_volume[i] = j["bid_vol"][i];
    }

    /*
    std::cout << j << std::endl;

    json out_j;
    TiQuoteFormater::FormatSnapshot(&stock, out_j);

    std::cout << out_j << std::endl;
    std::cout << "-------------" << std::endl;

    */

    // Call the callback
    m_ipc_server.OnL2StockSnapshotRtn(&stock);

    return;
};


void OcQuoteIpcServerCtpRedisRq::onTimer(uv_timer_t* handle)
{
    OcQuoteIpcServerCtpRedisRq* pThis = (OcQuoteIpcServerCtpRedisRq*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void OcQuoteIpcServerCtpRedisRq::onAuth(int err, const char* errStr){
    if(m_config){
        subStream(m_config->szQuoteStreamGroup.c_str(),
            m_config->szQuoteStreamKey.c_str(),
            m_config->szQuoteConsumerId.c_str(), 
            m_config->nBlock);
    }

    std::cout << "onAuth:" << err << " " << errStr << std::endl;
}

void OcQuoteIpcServerCtpRedisRq::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void OcQuoteIpcServerCtpRedisRq::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    OnCommandRtn(type, msg);
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int OcQuoteIpcServerCtpRedisRq::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["oc_quote_ipc_server_ctp_redis_rq"]["ip"]);
    m_config->nPort       = _iniFile["oc_quote_ipc_server_ctp_redis_rq"]["port"];
    m_config->szAuth      = string(_iniFile["oc_quote_ipc_server_ctp_redis_rq"]["auth"]);

    m_config->nBlock                = _iniFile["oc_quote_ipc_server_ctp_redis_rq"]["block"];
    m_config->szQuoteStreamKey      = string(_iniFile["oc_quote_ipc_server_ctp_redis_rq"]["quote_stream_key"]);
    m_config->szQuoteStreamGroup    = string(_iniFile["oc_quote_ipc_server_ctp_redis_rq"]["quote_stream_group"]);
    m_config->szQuoteConsumerId     = string(_iniFile["oc_quote_ipc_server_ctp_redis_rq"]["quote_consumer_id"]);
    
    m_config->szQuoteIpcTopic       = string(_iniFile["oc_quote_ipc_server_ctp_redis_rq"]["quote_ipc_topic"]);
    

    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szQuoteStreamGroup.empty() |
        m_config->szQuoteStreamKey.empty() |
        m_config->szQuoteConsumerId.empty() |
        m_config->szQuoteIpcTopic.empty())
    {
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void OcQuoteIpcServerCtpRedisRq::resetStreamKey()
{
    if (!m_config)
    {
        return;
    }
    int64_t time_num = datetime::get_time_num();
    if (time_num  > 95000000 && time_num < 155000000)   //交易时段不重置了
    {
        return;
    }
};
