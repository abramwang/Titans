#include <iostream>
#include <unistd.h>
#include "oc_trader_commander_huaxin.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include "ti_trader_formater.h"

OcTraderCommanderHuaxin::OcTraderCommanderHuaxin(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_client = new TiHxTraderClient(configPath, this);
    m_config = NULL;
    m_total_asset = 0;
    m_cash_asset = 0;
    loadConfig(configPath);

    ///*
    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[OcTraderCommanderHuaxin] flag: " << flag;
        resetStreamKey();
    }
    //*/

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);

    /*
    m_client->connect();
    sleep(3);
    m_client->QueryPositions();

    sleep(3);

    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, "000001");
    strcpy(req.szExchange, "SZ");
    req.nTradeSideType = 'B';
    req.nBusinessType = 'S';
    req.nOffsetType = 'C';
    req.nOrderPrice = 11.26;
    req.nOrderVol = 10000;

    m_client->orderInsert(&req);
    */

}
OcTraderCommanderHuaxin::~OcTraderCommanderHuaxin(){
    if(m_client){
        delete m_client;
        m_client = NULL;
    }
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
void OcTraderCommanderHuaxin::OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str)
{
    if (err != 0)
    {
        std::cout << "OnCommonJsonRespones: " << err_str << std::endl;
        return;
    }
    if (rspData == NULL)
    {
        std::cout << "OnCommonJsonRespones: rspData json is null, " << req_id << std::endl;
        return;
    }
    if ((*rspData)["type"] == "OnRspFundQueryResult")
    {
        std::cout << "OnRspFundQueryResult: " << *rspData << std::endl;
        m_total_asset = (*rspData)["data"]["total_asset"].get<double>();
        m_cash_asset = (*rspData)["data"]["buying_power"].get<double>();
        return;
    }
};   

void OcTraderCommanderHuaxin::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    Locker locker(&m_mutex);
};
void OcTraderCommanderHuaxin::OnRspQryOrder(const TiRspQryOrder* pData, bool isLast)
{
    /*
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspQryOrder(pData, isLast);
    }
    */
    json j;
    TiTraderFormater::FormatOrderStatus(pData, j);
    std::cout << "OnRspQryOrder: " << j << std::endl;
    return;
    if (m_config)
    {
        if(!m_config->szOrderKey.empty())
        {
            TiTraderFormater::FormatOrderStatus(pData, m_json_cash);
            m_redis.hmset(m_config->szOrderKey.c_str(), m_json_cash["szOrderId"].get<std::string>().c_str(), m_json_cash.dump().c_str());
        }
    }
};
void OcTraderCommanderHuaxin::OnRspQryMatch(const TiRspQryMatch* pData, bool isLast)
{
    /*
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspQryMatch(pData, isLast);
    }
    */
    json j;
    TiTraderFormater::FormatOrderMatchEvent(pData, j);
    std::cout << "OnRtnOrderMatchEvent: " << j << std::endl;
    return;
    if (m_config)
    {
        if(!m_config->szMatchKey.empty())
        {
            TiTraderFormater::FormatOrderMatchEvent(pData, m_json_cash);
            m_redis.hmset(m_config->szMatchKey.c_str(), m_json_cash["szStreamId"].get<std::string>().c_str(), m_json_cash.dump().c_str());
        }
    }
};
void OcTraderCommanderHuaxin::OnRspQryPosition(const TiRspQryPosition* pData, bool isLast)
{
    /*
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspQryPosition(pData, isLast);
    }
    */
};
void OcTraderCommanderHuaxin::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    Locker locker(&m_mutex);
    json j;
    TiTraderFormater::FormatOrderStatus(pData, j);
    std::cout << "OnRtnOrderStatusEvent: " << j << std::endl;
};
void OcTraderCommanderHuaxin::OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData)
{
    Locker locker(&m_mutex);
    json j;
    TiTraderFormater::FormatOrderMatchEvent(pData, j);
    std::cout << "OnRtnOrderMatchEvent: " << j << std::endl;
};


void OcTraderCommanderHuaxin::OnTimer()
{
    Locker locker(&m_mutex);
};


void OcTraderCommanderHuaxin::OnCommandRtn(const char* type, const char* command)
{
    std::cout << "OnCommandRtn: " << type << " " << command << std::endl;
    if (!strcmp(type, "enterOrder"))
    {
        json j = json::parse(command);
        enterOrder(j);
        return;
    }

    if (!strcmp(type, "cancelOrder"))
    {
        json j = json::parse(command);
        cancelOrder(j);
        return;
    }

    if (!strcmp(type, "QueryAsset"))
    {
        m_client->QueryAsset();
        return;
    }

    if (!strcmp(type, "QueryPositions"))
    {
        m_client->QueryPositions();
        return;
    }

    if (!strcmp(type, "QueryOrders"))
    {
        m_client->QueryOrders();
        return;
    }

    if (!strcmp(type, "QueryMatches"))
    {
        m_client->QueryMatches();
        return;
    }

     if (!strcmp(type, "QueryETFFile"))
    {
        m_client->QueryETFFile();
        return;
    }

    if (!strcmp(type, "QueryETFBasket"))
    {
        m_client->QueryETFBasket();
        return;
    }
    
};


void OcTraderCommanderHuaxin::onTimer(uv_timer_t* handle)
{
    OcTraderCommanderHuaxin* pThis = (OcTraderCommanderHuaxin*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void OcTraderCommanderHuaxin::onAuth(int err, const char* errStr){
    if(m_config){
        subStream(m_config->szCommandStreamGroup.c_str(),
            m_config->szCommandStreamKey.c_str(),
            m_config->szCommandConsumerId.c_str(), 
            m_config->nBlock);
    }
    if(m_client){
        m_client->connect();
    }
    std::cout << "onAuth:" << err << " " << errStr << std::endl;
}

void OcTraderCommanderHuaxin::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void OcTraderCommanderHuaxin::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    OnCommandRtn(type, msg);
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int OcTraderCommanderHuaxin::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["oc_trader_commander_huaxin"]["ip"]);
    m_config->nPort       = _iniFile["oc_trader_commander_huaxin"]["port"];
    m_config->szAuth      = string(_iniFile["oc_trader_commander_huaxin"]["auth"]);

    m_config->nBlock          = _iniFile["oc_trader_commander_huaxin"]["block"];
    m_config->szCommandStreamKey     = string(_iniFile["oc_trader_commander_huaxin"]["command_stream_key"]);
    m_config->szCommandStreamGroup   = string(_iniFile["oc_trader_commander_huaxin"]["command_stream_group"]);
    m_config->szCommandConsumerId   = string(_iniFile["oc_trader_commander_huaxin"]["command_consumer_id"]);
    
    m_config->szOrderKey         = string(_iniFile["oc_trader_commander_huaxin"]["order_key"]);
    m_config->szMatchKey         = string(_iniFile["oc_trader_commander_huaxin"]["match_key"]);
    
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

void OcTraderCommanderHuaxin::resetStreamKey()
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

void OcTraderCommanderHuaxin::enterOrder(json &msg)
{
    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, std::string(msg["szSymbol"]).c_str());
    strcpy(req.szExchange, std::string(msg["szExchange"]).c_str());
    req.nTradeSideType = std::string(msg["nTradeSideType"]).c_str()[0];
    req.nBusinessType = std::string(msg["nBusinessType"]).c_str()[0];
    req.nOffsetType = std::string(msg["nOffsetType"]).c_str()[0];
    req.nOrderPrice = msg["nOrderPrice"];
    req.nOrderVol = msg["nOrderVol"];

    m_client->orderInsert(&req);
};


void OcTraderCommanderHuaxin::cancelOrder(json &msg)
{
    TiReqOrderDelete req;
    memset(&req, 0, sizeof(TiReqOrderDelete));
    req.nOrderId = msg["nOrderId"];

    m_client->orderDelete(&req);

};

////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////
TiTraderClient* OcTraderCommanderHuaxin::GetTraderClient()
{
    return m_client;
};