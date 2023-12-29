#include <iostream>
#include <unistd.h>
#include "oc_quote_commander.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include "ti_quote_formater.h"

OcQuoteCommander::OcQuoteCommander(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_quote_client = new TiQuoteIpcClient(configPath, loop, this, this);
    m_config = NULL;
    loadConfig(configPath);

    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[OcQuoteCommander] flag: " << flag;

        resetStreamKey();
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);
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

void OcQuoteCommander::OnTimer()
{
    if (m_json_cash.empty())
    {
        return;
    }
    
    std::string msg = m_json_cash.dump();
    m_json_cash = json::array();

    m_redis.xadd(m_config->szQuoteStreamKey.c_str(), msg.c_str());
};


void OcQuoteCommander::OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
void OcQuoteCommander::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
void OcQuoteCommander::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};
void OcQuoteCommander::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    int64_t symbol_id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);
    if (m_subscribed_snapshot_symbol_ids.find(symbol_id) == m_subscribed_snapshot_symbol_ids.end())
    {
        return;
    }
    printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);

    json j;
    TiQuoteFormater::FormatSnapshot(pData, j);

    m_json_cash.push_back(j);    
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

    initQuoteInfo(m_config->szSubscribedInfoKey);

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

void OcQuoteCommander::onTimer(uv_timer_t* handle)
{
    OcQuoteCommander* pThis = (OcQuoteCommander*)handle->data;
    pThis->OnTimer();
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

    m_config->szIp        = string(_iniFile["oc_quote_commander"]["ip"]);
    m_config->nPort       = _iniFile["oc_quote_commander"]["port"];
    m_config->szAuth      = string(_iniFile["oc_quote_commander"]["auth"]);

    m_config->szQuoteTopic      = string(_iniFile["oc_quote_commander"]["ipc_quote_topic"]);

    m_config->nBlock                    = _iniFile["oc_quote_commander"]["block"];
    m_config->szCommandStreamKey        = string(_iniFile["oc_quote_commander"]["command_stream_key"]);
    m_config->szCommandStreamGroup      = string(_iniFile["oc_quote_commander"]["command_stream_group"]);
    m_config->szCommandConsumerId       = string(_iniFile["oc_quote_commander"]["command_consumer_id"]);
    
    m_config->szSubscribedInfoKey       = string(_iniFile["oc_quote_commander"]["subscribed_info_key"]);
    m_config->szQuoteStreamKey          = string(_iniFile["oc_quote_commander"]["quote_notify_stream_key"]);
    
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

void OcQuoteCommander::initQuoteInfo(std::string quoteInfoKey)
{
    std::string quoteInfo;
    bool flag = m_redis.get(quoteInfoKey.c_str(), quoteInfo);

    json j;
    if(flag){
        m_subscribed_snapshot_symbol_ids.clear();
        m_subscribed_matches_symbol_ids.clear();
        m_subscribed_orders_symbol_ids.clear();

        j = json::parse(quoteInfo);

        if(j["SH.snapshot"].is_array()){
            for(auto it = j["SH.snapshot"].begin(); it != j["SH.snapshot"].end(); it++){
                std::string symbol = it->get<std::string>();
                m_subscribed_snapshot_symbol_ids.insert(TiQuoteTools::GetSymbolID("SH", symbol.c_str()));
            }
        }

        if(j["SZ.snapshot"].is_array()){
            for(auto it = j["SZ.snapshot"].begin(); it != j["SZ.snapshot"].end(); it++){
                std::string symbol = it->get<std::string>();
                m_subscribed_snapshot_symbol_ids.insert(TiQuoteTools::GetSymbolID("SZ", symbol.c_str()));
            }
        }
    }

    std::cout << "m_subscribed_snapshot_symbol_ids: " << m_subscribed_snapshot_symbol_ids.size() << std::endl;

};

void OcQuoteCommander::resetStreamKey()
{
    if (!m_config)
    {
        return;
    }
    m_redis.xtrim(m_config->szQuoteStreamKey.c_str(), 1000);

    int64_t time_num = datetime::get_time_num();
    if (time_num  > 95000000 && time_num < 155000000)   //交易时段不重置了
    {
        return;
    }
    if(!m_config->szSubscribedInfoKey.empty())
    {
        m_redis.del(m_config->szSubscribedInfoKey.c_str());
    }
    if(!m_config->szQuoteStreamKey.empty())
    {
        m_redis.del(m_config->szQuoteStreamKey.c_str());
    }
};
