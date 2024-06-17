#include <iostream>
#include <unistd.h>
#include "oc_quote_ipc_server_ctp_redis_xt.h"
#include "iniFile.h"
#include "datetime.h"
#include "ti_quote_formater.h"
#include <glog/logging.h>

OcQuoteIpcServerCtpRedisXt::OcQuoteIpcServerCtpRedisXt(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_config = NULL;
    m_ctp_client = NULL;

    loadConfig(configPath);

    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[OcQuoteIpcServerCtpRedisXt] flag: " << flag;
        resetStreamKey();

        m_quote_info_mysql_client = new OcQuoteInfoMysql(m_config->szSqlIp, m_config->nSqlPort, m_config->szSqlUser, m_config->szSqlPassword);
        m_quote_info_mysql_client->QueryInstrumentInfoList(m_instrument_info_list);

        std::cout << "QueryInstrumentInfoList size: " << m_instrument_info_list.size() << std::endl;

        
        m_ctp_client = new TiCtpQuoteClient("./config.ini", &m_ipc_server);

        m_quote_cache.init_instrument(m_instrument_info_list);
        m_ctp_client->connect();
    }
    //*/

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);
}
OcQuoteIpcServerCtpRedisXt::~OcQuoteIpcServerCtpRedisXt(){
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

void OcQuoteIpcServerCtpRedisXt::OnTimer()
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    /*
    std::cout << "当前时间: "
            << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
            << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec
            << std::endl;
    */
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

void OcQuoteIpcServerCtpRedisXt::OnCommandRtn(const char* type, const char* command)
{
    json j = json::parse(command);

    for(auto iter = j.begin(); iter != j.end(); iter++)
    {
        TiQuoteSnapshotStockField* stock_ptr = nullptr;
        TiQuoteSnapshotIndexField* index_ptr = nullptr;

        bool flag = m_quote_cache.update_xt_snapshot_cache(iter.key(), iter.value(), stock_ptr, index_ptr);

        if (stock_ptr)
        {
            m_ipc_server.OnL2StockSnapshotRtn(stock_ptr);
        }
        
        if (index_ptr)
        {
            m_ipc_server.OnL2IndexSnapshotRtn(index_ptr);
        }
    }
};


void OcQuoteIpcServerCtpRedisXt::onTimer(uv_timer_t* handle)
{
    OcQuoteIpcServerCtpRedisXt* pThis = (OcQuoteIpcServerCtpRedisXt*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void OcQuoteIpcServerCtpRedisXt::onAuth(int err, const char* errStr){
    if(m_config){
        subStream(m_config->szQuoteStreamGroup.c_str(),
            m_config->szQuoteStreamKey.c_str(),
            m_config->szQuoteConsumerId.c_str(), 
            m_config->nBlock);
    }

    std::cout << "onAuth:" << err << " " << errStr << std::endl;
}

void OcQuoteIpcServerCtpRedisXt::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void OcQuoteIpcServerCtpRedisXt::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    OnCommandRtn(type, msg);
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int OcQuoteIpcServerCtpRedisXt::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["ip"]);
    m_config->nPort       = _iniFile["oc_quote_ipc_server_ctp_redis_xt"]["port"];
    m_config->szAuth      = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["auth"]);

    m_config->nBlock                = _iniFile["oc_quote_ipc_server_ctp_redis_xt"]["block"];
    m_config->szQuoteStreamKey      = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["quote_stream_key"]);
    m_config->szQuoteStreamGroup    = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["quote_stream_group"]);
    m_config->szQuoteConsumerId     = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["quote_consumer_id"]);
    
    m_config->szQuoteIpcTopic       = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["quote_ipc_topic"]);
    
    m_config->szSqlIp       = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["sql_ip"]);
    m_config->nSqlPort      = _iniFile["oc_quote_ipc_server_ctp_redis_xt"]["sql_port"];
    m_config->szSqlUser     = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["sql_user"]);
    m_config->szSqlPassword = string(_iniFile["oc_quote_ipc_server_ctp_redis_xt"]["sql_password"]);

    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szQuoteStreamGroup.empty() |
        m_config->szQuoteStreamKey.empty() |
        m_config->szQuoteConsumerId.empty() |
        m_config->szQuoteIpcTopic.empty() |
        m_config->szSqlIp.empty() |
        !m_config->nSqlPort |
        m_config->szSqlUser.empty() |
        m_config->szSqlPassword.empty() )
    {
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void OcQuoteIpcServerCtpRedisXt::resetStreamKey()
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
