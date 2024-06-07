#include <iostream>
#include <unistd.h>
#include "oc_quote_ipc_server_ctp_redis_xt.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>

OcQuoteIpcServerCtpRedisXt::OcQuoteIpcServerCtpRedisXt(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_config = NULL;
    loadConfig(configPath);

    ///*
    if(m_config){
        std::cout << "test" << std::endl;
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[OcQuoteIpcServerCtpRedisXt] flag: " << flag;
        resetStreamKey();
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
    Locker locker(&m_mutex);
};


void OcQuoteIpcServerCtpRedisXt::OnCommandRtn(const char* type, const char* command)
{
    json j = json::parse(command);

    for(auto iter = j.begin(); iter != j.end(); iter++){

        std::cout << iter.key() << ":" << iter.value() << std::endl;
        break;
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
    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szQuoteStreamGroup.empty() |
        m_config->szQuoteStreamKey.empty() |
        m_config->szQuoteConsumerId.empty()|
        m_config->szQuoteIpcTopic.empty() )
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
