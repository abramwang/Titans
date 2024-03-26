#include <iostream>
#include <unistd.h>
#include "ia_etf_follow_trade_bot_gt.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include "ti_trader_formater.h"

IaEtfFollowTradeBotGt::IaEtfFollowTradeBotGt(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_quote_client = new TiQuoteIpcClient(configPath, loop, this);
    m_trade_client = new TiGtTraderClient(configPath, this);
    m_config = NULL;
    m_user_setting = NULL;
    m_mysql = NULL;
    m_quote_cache = NULL;
    m_trade_center = NULL;
    m_total_asset = 0;
    m_cash_asset = 0;
    loadConfig(configPath);

    ///*
    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[IaEtfFollowTradeBotGt] flag: " << flag;
        resetStreamKey();

        m_user_setting = new IaEtfUserSetting(&m_redis);
        m_mysql = new IaEtfInfoMysql(m_config->szSqlIp.c_str(), m_config->nSqlPort, m_config->szSqlUser.c_str(), m_config->szSqlPassword.c_str(), m_config->szSqlDb.c_str());
        m_quote_cache = new IaEtfQuoteDataCache();
        m_trade_center = new IaEtfTradeWorkerCenter(m_trade_client, m_quote_cache, m_mysql);
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);

    m_quote_client->run(NULL);
}
IaEtfFollowTradeBotGt::~IaEtfFollowTradeBotGt(){
    if(m_trade_client){
        delete m_trade_client;
        m_trade_client = NULL;
    }
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    uv_timer_stop(&m_timer);
    uv_close((uv_handle_t*)&m_timer, NULL);
};

////////////////////////////////////////////////////////////////////////
// 行情回调
////////////////////////////////////////////////////////////////////////
void IaEtfFollowTradeBotGt::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    m_quote_cache->OnL2StockSnapshotRtn(pData);
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////
void IaEtfFollowTradeBotGt::OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str)
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

void IaEtfFollowTradeBotGt::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    Locker locker(&m_mutex);
};
void IaEtfFollowTradeBotGt::OnRspQryOrder(const TiRspQryOrder* pData, bool isLast)
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
void IaEtfFollowTradeBotGt::OnRspQryMatch(const TiRspQryMatch* pData, bool isLast)
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
void IaEtfFollowTradeBotGt::OnRspQryPosition(const TiRspQryPosition* pData, bool isLast)
{

    json j;
    TiTraderFormater::FormatPosition(pData, j);
    std::cout << "OnRspQryPosition: " << j << std::endl;
    /*
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspQryPosition(pData, isLast);
    }
    */
};
void IaEtfFollowTradeBotGt::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    Locker locker(&m_mutex);
    json j;
    TiTraderFormater::FormatOrderStatus(pData, j);
    std::cout << "OnRtnOrderStatusEvent: " << j << std::endl;
};
void IaEtfFollowTradeBotGt::OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData)
{
    Locker locker(&m_mutex);
    json j;
    TiTraderFormater::FormatOrderMatchEvent(pData, j);
    std::cout << "OnRtnOrderMatchEvent: " << j << std::endl;
};


void IaEtfFollowTradeBotGt::OnTimer()
{
    Locker locker(&m_mutex);
    TiQuoteSnapshotStockField* pData = m_quote_cache->GetSnapshot("000001", "SZ");
    if (pData)
    {
        printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
    }
};


void IaEtfFollowTradeBotGt::OnCommandRtn(const char* type, const char* command)
{
    std::cout << "OnCommandRtn: " << type << " " << command << std::endl;
    if (!strcmp(type, "enterOrder"))
    {
        json j = json::parse(command);
        enterOrder(j);
        return;
    }

    if (!strcmp(type, "enterOrders"))
    {
        json j = json::parse(command);
        enterOrders(j);
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
        m_trade_client->QueryAsset();
        return;
    }

    if (!strcmp(type, "QueryPositions"))
    {
        m_trade_client->QueryPositions();
        return;
    }

    if (!strcmp(type, "QueryOrders"))
    {
        m_trade_client->QueryOrders();
        return;
    }

    if (!strcmp(type, "QueryMatches"))
    {
        m_trade_client->QueryMatches();
        return;
    }
    
};


void IaEtfFollowTradeBotGt::onTimer(uv_timer_t* handle)
{
    IaEtfFollowTradeBotGt* pThis = (IaEtfFollowTradeBotGt*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void IaEtfFollowTradeBotGt::onAuth(int err, const char* errStr){
    if(m_config){
        subStream(m_config->szCommandStreamGroup.c_str(),
            m_config->szCommandStreamKey.c_str(),
            m_config->szCommandConsumerId.c_str(), 
            m_config->nBlock);
    }
    ///* 
    if(m_trade_client){
        m_trade_client->connect();
    }
    //*/
    std::cout << "onAuth:" << err << " " << errStr << std::endl;
}

void IaEtfFollowTradeBotGt::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void IaEtfFollowTradeBotGt::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    OnCommandRtn(type, msg);
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int IaEtfFollowTradeBotGt::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["ia_etf_follow_trade_bot_gt"]["ip"]);
    m_config->nPort       = _iniFile["ia_etf_follow_trade_bot_gt"]["port"];
    m_config->szAuth      = string(_iniFile["ia_etf_follow_trade_bot_gt"]["auth"]);

    m_config->nBlock          = _iniFile["ia_etf_follow_trade_bot_gt"]["block"];
    m_config->szCommandStreamKey     = string(_iniFile["ia_etf_follow_trade_bot_gt"]["command_stream_key"]);
    m_config->szCommandStreamGroup   = string(_iniFile["ia_etf_follow_trade_bot_gt"]["command_stream_group"]);
    m_config->szCommandConsumerId   = string(_iniFile["ia_etf_follow_trade_bot_gt"]["command_consumer_id"]);
    
    m_config->szOrderKey         = string(_iniFile["ia_etf_follow_trade_bot_gt"]["order_key"]);
    m_config->szMatchKey         = string(_iniFile["ia_etf_follow_trade_bot_gt"]["match_key"]);
    
    m_config->szSqlIp       = string(_iniFile["ia_etf_follow_trade_bot_gt"]["sql_ip"]);
    m_config->nSqlPort      = _iniFile["ia_etf_follow_trade_bot_gt"]["sql_port"];
    m_config->szSqlUser     = string(_iniFile["ia_etf_follow_trade_bot_gt"]["sql_user"]);
    m_config->szSqlPassword = string(_iniFile["ia_etf_follow_trade_bot_gt"]["sql_password"]);
    m_config->szSqlDb       = string(_iniFile["ia_etf_follow_trade_bot_gt"]["sql_db"]);

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

void IaEtfFollowTradeBotGt::resetStreamKey()
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

void IaEtfFollowTradeBotGt::enterOrder(json &msg)
{
    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, std::string(msg["szSymbol"]).c_str());
    strcpy(req.szExchange, std::string(msg["szExchange"]).c_str());
    strcpy(req.szAccount, std::string(msg["szAccount"]).c_str());
    req.nTradeSideType = std::string(msg["nTradeSideType"]).c_str()[0];
    req.nBusinessType = std::string(msg["nBusinessType"]).c_str()[0];
    req.nOffsetType = std::string(msg["nOffsetType"]).c_str()[0];
    req.nOrderPrice = msg["nOrderPrice"];
    req.nOrderVol = msg["nOrderVol"];
    strcpy(req.szUseStr, "oc_trader_commander_gt");

    m_trade_client->orderInsert(&req);
};

void IaEtfFollowTradeBotGt::enterOrders(json &msg)
{
    std::cout << "enterOrders: " << std::endl;
    if(!msg.is_array())
    {
       return;
    }
    std::vector<TiReqOrderInsert> req_vec;
    std::string account_id;
    for (auto iter = msg.begin(); iter != msg.end(); iter++)
    {
        TiReqOrderInsert req;
        memset(&req, 0, sizeof(TiReqOrderInsert));
        strcpy(req.szSymbol, std::string((*iter)["szSymbol"]).c_str());
        strcpy(req.szExchange, std::string((*iter)["szExchange"]).c_str());
        strcpy(req.szAccount, std::string((*iter)["szAccount"]).c_str());
        req.nTradeSideType = std::string((*iter)["nTradeSideType"]).c_str()[0];
        req.nBusinessType = std::string((*iter)["nBusinessType"]).c_str()[0];
        req.nOffsetType = std::string((*iter)["nOffsetType"]).c_str()[0];
        req.nOrderPrice = (*iter)["nOrderPrice"];
        req.nOrderVol = (*iter)["nOrderVol"];
        strcpy(req.szUseStr, "oc_trader_commander_gt");
        
        req_vec.push_back(req);

        account_id = (*iter)["szAccount"];
    }
    m_trade_client->orderInsertBatch(req_vec, account_id);
};


void IaEtfFollowTradeBotGt::cancelOrder(json &msg)
{
    TiReqOrderDelete req;
    memset(&req, 0, sizeof(TiReqOrderDelete));
    req.nOrderId = msg["nOrderId"];

    m_trade_client->orderDelete(&req);

};

////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////
TiTraderClient* IaEtfFollowTradeBotGt::GetTraderClient()
{
    return m_trade_client;
};