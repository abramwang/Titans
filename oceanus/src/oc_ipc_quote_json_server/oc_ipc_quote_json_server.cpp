#include <iostream>
#include <unistd.h>
#include "oc_ipc_quote_json_server.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include "ti_quote_formater.h"

OcIpcQuoteJsonServer::OcIpcQuoteJsonServer(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_quote_client = new TiQuoteIpcClient(configPath, loop, this, this);
    m_config = NULL;
    m_cout_time_snap = 0;
    loadConfig(configPath);

    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[OcIpcQuoteJsonServer] flag: " << flag;

        resetStreamKey();
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 100);
}
OcIpcQuoteJsonServer::~OcIpcQuoteJsonServer(){
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
void OcIpcQuoteJsonServer::OnCommandRtn(const char* type, const char* command)
{
    std::cout << "OnCommandRtn: " << type << " " << command << std::endl;
    
    if (!strcmp(type, "update_sub_info"))
    {
        //updateQuoteInfo(command);
        //initQuoteInfo(command);
        return;
    }
};

void OcIpcQuoteJsonServer::OnTimer()
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    /*
    std::cout << "当前时间: "
            << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
            << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec
            << std::endl;
    */
    ///*
    if (localTime->tm_hour > 16 )
    {
        std::terminate();
    }
    //*/

    
    for (auto iter = m_snapshot_map.begin(); iter!= m_snapshot_map.end(); iter++)
    {
        json j;
        TiQuoteFormater::FormatSnapshot(iter->second.get(), j);
        j["type"] = "snapshot";
        m_json_cash.push_back(j);

        if (!m_config->szQuoteQueryKey.empty())
        {
            m_redis.hset(m_config->szQuoteQueryKey.c_str(), iter->second->symbol, j.dump().c_str());
        }
    }
    
    if (m_json_cash.empty())
    {
        return;
    }

    std::string msg = m_json_cash.dump();
    m_json_cash = json::array();

    m_redis.xadd(m_config->szQuoteStreamKey.c_str(), msg.c_str(), 500);
};


void OcIpcQuoteJsonServer::OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
void OcIpcQuoteJsonServer::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData)
{
    json j;
    TiQuoteFormater::FormatSnapshot(pData, j);
    j["type"] = "snapshot";

    m_json_cash.push_back(j);
};

void OcIpcQuoteJsonServer::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{    
    json j;
    TiQuoteFormater::FormatSnapshot(pData, j);
    j["type"] = "snapshot";

    m_json_cash.push_back(j);
};

void OcIpcQuoteJsonServer::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    //return;
    if ((pData->time - m_cout_time_snap) > 5000)
    {
        printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f\n", 
            pData->symbol, pData->time_str, pData->time, pData->last, pData->acc_volume, pData->acc_turnover);
        m_cout_time_snap = pData->time;
    }

    TiQuoteSnapshotStockField* pSnap = GetStockSnapshot(pData->symbol, pData->exchange);
    if(pSnap){
        memcpy(pSnap, pData, sizeof(TiQuoteSnapshotStockField));
    }else{
        m_snapshot_map[TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol)] = std::make_unique<TiQuoteSnapshotStockField>(*pData);
    }
};

void OcIpcQuoteJsonServer::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    return;
    /*
    int64_t symbol_id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);
    if (m_subscribed_snapshot_symbol_ids.find(symbol_id) == m_subscribed_snapshot_symbol_ids.end())
    {
        return;
    }
    */

    json j;
    TiQuoteFormater::FormatMatch(pData, j);
    j["type"] = "matches";

    //m_json_cash.push_back(j);
};

void OcIpcQuoteJsonServer::OnL2StockOrderRtn(const TiQuoteOrderField* pData){
    return;
    /*
    int64_t symbol_id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);
    if (m_subscribed_snapshot_symbol_ids.find(symbol_id) == m_subscribed_snapshot_symbol_ids.end())
    {
        return;
    }
    */

    json j;
    TiQuoteFormater::FormatOrder(pData, j);
    j["type"] = "order";

    //m_json_cash.push_back(j);
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void OcIpcQuoteJsonServer::onAuth(int err, const char* errStr){
    if(!m_config){
        return;
    }
    std::cout << "onAuth:" << err << " " << errStr << std::endl;

    if(m_config->szQuoteTopic.empty()){
        m_quote_client->run(NULL);
    }else{
        m_quote_client->run(m_config->szQuoteTopic.c_str());
    }
}

void OcIpcQuoteJsonServer::onTimer(uv_timer_t* handle)
{
    OcIpcQuoteJsonServer* pThis = (OcIpcQuoteJsonServer*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int OcIpcQuoteJsonServer::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["oc_ipc_quote_json_server"]["ip"]);
    m_config->nPort       = _iniFile["oc_ipc_quote_json_server"]["port"];
    m_config->szAuth      = string(_iniFile["oc_ipc_quote_json_server"]["auth"]);

    m_config->szQuoteTopic      = string(_iniFile["oc_ipc_quote_json_server"]["ipc_quote_topic"]);

    m_config->szQuoteStreamKey  = string(_iniFile["oc_ipc_quote_json_server"]["quote_notify_stream_key"]);
    m_config->szQuoteQueryKey   = string(_iniFile["oc_ipc_quote_json_server"]["quote_query_key"]);

    //m_config->szQuoteQueryKey = "oc_ipc_quote_json_server.query";

    std::cout << "szQuoteStreamKey: " << m_config->szQuoteStreamKey << std::endl;
    std::cout << "szQuoteQueryKey: " << m_config->szQuoteQueryKey << std::endl;
    
    if( m_config->szIp.empty() |
        !m_config->nPort )
    {
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void OcIpcQuoteJsonServer::resetStreamKey()
{
    if (!m_config)
    {
        return;
    }

    int64_t time_num = datetime::get_time_num();
    if (time_num  > 95000000 && time_num < 205000000)   //交易时段不重置了
    {
        return;
    }

    return;

    if(!m_config->szQuoteStreamKey.empty())
    {
        m_redis.del(m_config->szQuoteStreamKey.c_str());
    }
};

TiQuoteSnapshotStockField* OcIpcQuoteJsonServer::GetStockSnapshot(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_snapshot_map.find(id);
    if (it != m_snapshot_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};
    