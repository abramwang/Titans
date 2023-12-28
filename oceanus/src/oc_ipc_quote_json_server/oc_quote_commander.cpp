#include <iostream>
#include <unistd.h>
#include "oc_quote_commander.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include "ti_trader_formater.h"

OcQuoteCommander::OcQuoteCommander(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_quote_client = new TiQuoteIpcClient(configPath, loop, this, this);
    m_config = NULL;
    loadConfig(configPath);

    ///*
    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[OcQuoteCommander] flag: " << flag;
        resetStreamKey();
    }

}
OcQuoteCommander::~OcQuoteCommander(){
    if(m_quote_client){
        delete m_quote_client;
        m_quote_client = NULL;
    }
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////
void OcQuoteCommander::OnCommandRtn(const char* type, const char* command)
{
    std::cout << "OnCommandRtn: " << type << " " << command << std::endl;
    if (!strcmp(type, "enterOrder"))
    {
        json j = json::parse(command);
        //enterOrder(j);
        return;
    }

    if (!strcmp(type, "QueryAsset"))
    {
        //m_client->QueryAsset();
        return;
    }
    
};


void OcQuoteCommander::OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
void OcQuoteCommander::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
void OcQuoteCommander::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};
void OcQuoteCommander::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);

};
void OcQuoteCommander::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
void OcQuoteCommander::OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void OcQuoteCommander::onAuth(int err, const char* errStr){
    if(m_config){
        subStream(m_config->szCommandStreamGroup.c_str(),
            m_config->szCommandStreamKey.c_str(),
            m_config->szCommandConsumerId.c_str(), 
            m_config->nBlock);
    }
    std::cout << "onAuth:" << err << " " << errStr << std::endl;

    if(m_config->szQuoteTopic.empty()){
        m_quote_client->run(NULL);
    }else{
        m_quote_client->run(m_config->szQuoteTopic.c_str());
    }
}

void OcQuoteCommander::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void OcQuoteCommander::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    OnCommandRtn(type, msg);
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int OcQuoteCommander::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["oc_trader_commander_anxin"]["ip"]);
    m_config->nPort       = _iniFile["oc_trader_commander_anxin"]["port"];
    m_config->szAuth      = string(_iniFile["oc_trader_commander_anxin"]["auth"]);

    m_config->nBlock          = _iniFile["oc_trader_commander_anxin"]["block"];
    m_config->szCommandStreamKey     = string(_iniFile["oc_trader_commander_anxin"]["command_stream_key"]);
    m_config->szCommandStreamGroup   = string(_iniFile["oc_trader_commander_anxin"]["command_stream_group"]);
    m_config->szCommandConsumerId   = string(_iniFile["oc_trader_commander_anxin"]["command_consumer_id"]);
    
    m_config->szOrderKey         = string(_iniFile["oc_trader_commander_anxin"]["order_key"]);
    m_config->szMatchKey         = string(_iniFile["oc_trader_commander_anxin"]["match_key"]);
    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szCommandStreamGroup.empty() |
        m_config->szCommandStreamKey.empty() | 
        m_config->szCommandStreamGroup.empty() |
        m_config->szCommandConsumerId.empty() )
    {
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void OcQuoteCommander::resetStreamKey()
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
    if(!m_config->szOrderKey.empty())
    {
        m_redis.del(m_config->szOrderKey.c_str());
    }
    if(!m_config->szMatchKey.empty())
    {
        m_redis.del(m_config->szMatchKey.c_str());
    }
};
