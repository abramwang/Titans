#include <iostream>
#include <unistd.h>
#include "ia_etf_follow_trade_bot_gf.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include "ti_trader_formater.h"

IaEtfFollowTradeBotGf::IaEtfFollowTradeBotGf(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_redis = new RedisSyncHandle();
    m_quote_client = new TiQuoteIpcClient(configPath, loop, this);
    m_trade_client = new TiGfTraderClient(configPath, this);
    m_config = NULL;
    m_mysql = NULL;
    m_user_setting = NULL;
    m_quote_cache = NULL;
    m_signal_center = NULL;
    m_trade_center = NULL;

    m_query_time = datetime::get_time_num();
    m_query_interval = 60 * 5; //5分钟查询一次

    loadConfig(configPath);

    ///*
    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis->connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[IaEtfFollowTradeBotGf] m_redis->connect flag: " << flag;
        resetStreamKey();

        m_mysql = new IaEtfInfoMysql(m_config->szSqlIp.c_str(), m_config->nSqlPort, m_config->szSqlUser.c_str(), m_config->szSqlPassword.c_str());
        m_user_setting = new IaEtfUserSetting(m_redis, m_mysql);
        m_quote_cache = new IaEtfQuoteDataCache();
        m_signal_center = new IaEtfSignalCenter(m_user_setting, m_quote_cache);
        m_trade_center = new IaEtfTradeWorkerCenter(m_trade_client, m_quote_cache, m_user_setting, m_signal_center);
        m_influxdb_client = new IaEtfFactorToInflux(m_config->szInfluxUrl.c_str(), m_config->szInfluxToken.c_str());
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);

    m_quote_client->run(NULL);
}
IaEtfFollowTradeBotGf::~IaEtfFollowTradeBotGf(){
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
void IaEtfFollowTradeBotGf::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData)
{
    Locker locker(&m_mutex);
    m_quote_cache->OnL2IndexSnapshotRtn(pData);
};

void IaEtfFollowTradeBotGf::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    Locker locker(&m_mutex);
    m_quote_cache->OnL2FutureSnapshotRtn(pData);
};

void IaEtfFollowTradeBotGf::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    Locker locker(&m_mutex);
    m_quote_cache->OnL2StockSnapshotRtn(pData);
    m_signal_center->OnL2StockSnapshotRtn(pData);
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////
void IaEtfFollowTradeBotGf::OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str)
{
    LOG(INFO) << "[IaEtfFollowTradeBotGf::OnCommonJsonRespones] " << *rspData;
    Locker locker(&m_mutex);
    if (err != 0)
    {
        LOG(ERROR) << "OnCommonJsonRespones: " << err_str << std::endl;
        return;
    }
    if (rspData == NULL)
    {
        LOG(WARNING) << "OnCommonJsonRespones: rspData json is null, " << req_id << std::endl;
        return;
    }

    /*
    if (isLast)
    {
        std::cout << "OnCommonJsonRespones: " << isLast << " data:" <<  *rspData << std::endl;
    }
    */

    if ((*rspData)["type"] == "onReqAccountDetail")
    {
        
    }
};   

void IaEtfFollowTradeBotGf::OnRspAccountInfo(const TiRspAccountInfo* pData)
{
    Locker locker(&m_mutex);
    TiRspAccountInfo data = {0};
    memcpy(&data, pData, sizeof(TiRspAccountInfo));

    std::shared_ptr<IaAccountDBInfo> account_info_ptr;
    if (m_user_setting->GetAccountDBInfo(data.szAccount, account_info_ptr))
    {
        strcpy(data.szName, account_info_ptr->product_name.c_str());
    }

    json j;
    TiTraderFormater::FormatAccountInfo(&data, j);

    if (m_config)
    {
        if(!m_config->szAccountKey.empty())
        {
            std::string key = m_config->szAccountKey;
            m_redis->hmset(key.c_str(), data.szAccount, j.dump().c_str());
        }
    }

    LOG(INFO) << "OnRspAccountInfo: " << data.szAccount << " " << j << std::endl;

    m_trade_center->OnRspAccountInfo(&data);
};

void IaEtfFollowTradeBotGf::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    Locker locker(&m_mutex);
};

void IaEtfFollowTradeBotGf::OnRspQryOrder(const TiRspQryOrder* pData, bool isLast)
{
    Locker locker(&m_mutex);
    m_trade_center->OnRspQryOrder(pData, isLast);

    if (m_config)
    {
        if(!m_config->szOrderKey.empty())
        {
            std::string key = m_config->szOrderKey;
            key += ".";
            key += pData->szAccount;

            json j;
            TiTraderFormater::FormatOrderStatus(pData, j);

            LOG(INFO) << "OnRspQryOrder: " << isLast << " " << key.c_str() << " " << pData->szOrderStreamId << " " << j << std::endl;

            m_redis->hmset(key.c_str(), pData->szOrderStreamId, j.dump().c_str());
        }
    }
};
void IaEtfFollowTradeBotGf::OnRspQryMatch(const TiRspQryMatch* pData, bool isLast)
{
    Locker locker(&m_mutex);
    m_trade_center->OnRspQryMatch(pData, isLast);

    if (m_config)
    {
        if(!m_config->szMatchKey.empty())
        {
            std::string key = m_config->szMatchKey;
            key += ".";
            key += pData->szAccount;

            json j;
            TiTraderFormater::FormatOrderMatchEvent(pData, j);

            LOG(INFO) << "OnRspQryMatch: " << isLast << " " << key.c_str() << " " << pData->szStreamId << " " << j << std::endl;

            m_redis->hmset(key.c_str(), pData->szStreamId, j.dump().c_str());
        }
    }
};
void IaEtfFollowTradeBotGf::OnRspQryPosition(const TiRspQryPosition* pData, bool isLast)
{
    Locker locker(&m_mutex);
    m_trade_center->OnRspQryPosition(pData, isLast);

    if (m_config)
    {
        if(!m_config->szPositionKey.empty())
        {
            std::string key = m_config->szPositionKey;
            key += ".";
            key += pData->szAccount;

            json j;
            TiTraderFormater::FormatPosition(pData, j);
            
            //LOG(INFO) << "OnRspQryPosition: " << isLast << " " << j << std::endl;

            m_redis->hmset(key.c_str(), pData->szSymbol, j.dump().c_str());
        }
    }
};
void IaEtfFollowTradeBotGf::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    json test_j;
    TiTraderFormater::FormatOrderStatus(pData, test_j);
    std::cout << "OnRtnOrderStatusEvent: " << test_j << std::endl;

    Locker locker(&m_mutex);
    m_trade_center->OnRtnOrderStatusEvent(pData);

    if (m_config)
    {
        try
        {
            json j;
            TiTraderFormater::FormatOrderStatus(pData, j);

            LOG(INFO) << "OnRtnOrderStatusEvent: "<< pData->szOrderStreamId << " " << j << std::endl;

            if(!m_config->szOrderKey.empty())
            {
                std::string key = m_config->szOrderKey;
                key += ".";
                key += pData->szAccount;

                m_redis->hmset(key.c_str(), pData->szOrderStreamId, j.dump().c_str());
            }
        }
        catch(const std::exception& e)
        {
            std::cout << "Order ID: " << pData->nOrderId << ", "
                << "Submit Volume: " << pData->nSubmitVol << ", "
                << "Dealt Price: " << pData->nDealtPrice << ", "
                << "Dealt Volume: " << pData->nDealtVol << ", "
                << "Total Withdrawn Volume: " << pData->nTotalWithDrawnVol << ", "
                << "Invalid: " << pData->nInValid << ", "
                << "Status: " << pData->nStatus << ", "
                << "Insert Timestamp: " << pData->nInsertTimestamp << ", "
                << "Last Update Timestamp: " << pData->nLastUpdateTimestamp << ", "
                << "Used Time: " << pData->nUsedTime << ", "
                << "Fee: " << pData->nFee << ", "
                << "Order Stream ID: " << pData->szOrderStreamId << ", "
                << "Shareholder ID: " << pData->szShareholderId << std::endl;
            std::cerr << e.what() << '\n';
        }
    }
};
void IaEtfFollowTradeBotGf::OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData)
{
    Locker locker(&m_mutex);
    m_trade_center->OnRtnOrderMatchEvent(pData);

    if (m_config)
    {
        json j;
        TiTraderFormater::FormatOrderMatchEvent(pData, j);

        LOG(INFO) << "OnRtnOrderMatchEvent: "<< pData->szStreamId << " " << j << std::endl;

        if(!m_config->szMatchKey.empty())
        {
            std::string key = m_config->szMatchKey;
            key += ".";
            key += pData->szAccount;

            m_redis->hmset(key.c_str(), pData->szStreamId, j.dump().c_str());
        }
    }
};


void IaEtfFollowTradeBotGf::OnTimer()
{
    Locker locker(&m_mutex);
    m_signal_center->OnTimer();
    m_trade_center->OnTimer();

    json signal_out;
    m_signal_center->GetJsonOut(signal_out);

    if(!signal_out.empty())
    {
        try{
            json signal_array = json::array();
            for (auto iter = signal_out.begin(); iter != signal_out.end(); iter++)
            {
                signal_array.push_back(iter.value());
                if (iter.key().empty())
                {
                    continue;
                }
                if (iter.value().empty())
                {
                    continue;
                }
                m_redis->hmset(m_config->szSignalMap.c_str(), iter.key().c_str(), iter.value().dump().c_str());
                m_influxdb_client->add_point("jager", iter.value()["influx"]);
            }
            m_influxdb_client->write(m_config->szInfluxBucket.c_str(), m_config->szInfluxOrg.c_str(), "ms");
            //m_redis->xadd(m_config->szSignalStream.c_str(), signal_array.dump().c_str(), 2000);
            std::cout << "[IaEtfFollowTradeBotGf::OnTimer] signal_size: " << signal_out.size() << std::endl;
        }catch(std::exception& e){
            std::cout << "[IaEtfFollowTradeBotGf::OnTimer] " << e.what() << std::endl;
        }catch(...){
            std::cout << "[IaEtfFollowTradeBotGf::OnTimer] " << "unknown exception" << std::endl;
        }
    }

    int64_t time_num = datetime::get_time_num();
    if ((time_num - m_query_time) > m_query_interval*1000)
    {
        m_trade_client->QueryAsset();
        m_trade_client->QueryPositions();
        m_query_time = time_num;
        std::cout << "[IaEtfFollowTradeBotGf::OnTimer] QueryAsset QueryPositions: " << time_num << std::endl;
    }
    
    return;
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
        if (localTime->tm_hour == 15 && localTime->tm_min < 10)
        {
            return;
        }
        std::cout << "terminate" << std::endl;
        std::terminate();
    }
};


void IaEtfFollowTradeBotGf::OnCommandRtn(const char* type, const char* command)
{
    LOG(INFO) << "[IaEtfFollowTradeBotGf::OnCommandRtn]" << type << ";" << command;

    if (!strcmp(type, "etfTradingSignal"))
    {
        json j = json::parse(command);
        m_trade_center->OnTradingSignal(j);
        return;
    }

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


void IaEtfFollowTradeBotGf::onTimer(uv_timer_t* handle)
{
    IaEtfFollowTradeBotGf* pThis = (IaEtfFollowTradeBotGf*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void IaEtfFollowTradeBotGf::onAuth(int err, const char* errStr){
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

void IaEtfFollowTradeBotGf::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void IaEtfFollowTradeBotGf::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    OnCommandRtn(type, msg);
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int IaEtfFollowTradeBotGf::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["ia_etf_follow_trade_bot_gf"]["ip"]);
    m_config->nPort       = _iniFile["ia_etf_follow_trade_bot_gf"]["port"];
    m_config->szAuth      = string(_iniFile["ia_etf_follow_trade_bot_gf"]["auth"]);

    m_config->nBlock          = _iniFile["ia_etf_follow_trade_bot_gf"]["block"];
    m_config->szCommandStreamKey        = string(_iniFile["ia_etf_follow_trade_bot_gf"]["command_stream_key"]);
    m_config->szCommandStreamGroup      = string(_iniFile["ia_etf_follow_trade_bot_gf"]["command_stream_group"]);
    m_config->szCommandConsumerId       = string(_iniFile["ia_etf_follow_trade_bot_gf"]["command_consumer_id"]);
    
    m_config->szPositionKey      = string(_iniFile["ia_etf_follow_trade_bot_gf"]["position_key"]);
    m_config->szOrderKey         = string(_iniFile["ia_etf_follow_trade_bot_gf"]["order_key"]);
    m_config->szMatchKey         = string(_iniFile["ia_etf_follow_trade_bot_gf"]["match_key"]);

    m_config->szAccountKey       = string(_iniFile["ia_etf_follow_trade_bot_gf"]["account_key"]);

    m_config->szSignalMap       = string(_iniFile["ia_etf_follow_trade_bot_gf"]["signal_map"]);
    
    m_config->szSqlIp       = string(_iniFile["ia_etf_follow_trade_bot_gf"]["sql_ip"]);
    m_config->nSqlPort      = _iniFile["ia_etf_follow_trade_bot_gf"]["sql_port"];
    m_config->szSqlUser     = string(_iniFile["ia_etf_follow_trade_bot_gf"]["sql_user"]);
    m_config->szSqlPassword = string(_iniFile["ia_etf_follow_trade_bot_gf"]["sql_password"]);

    m_config->szInfluxUrl   = string(_iniFile["ia_etf_follow_trade_bot_gf"]["influx_url"]);
    m_config->szInfluxToken = string(_iniFile["ia_etf_follow_trade_bot_gf"]["influx_token"]);
    m_config->szInfluxOrg   = string(_iniFile["ia_etf_follow_trade_bot_gf"]["influx_org"]);
    m_config->szInfluxBucket= string(_iniFile["ia_etf_follow_trade_bot_gf"]["influx_bucket"]);


    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szSignalMap.empty() | 
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

void IaEtfFollowTradeBotGf::resetStreamKey()
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
    if(!m_config->szSignalMap.empty())
    {
        m_redis->del(m_config->szSignalMap.c_str());
    }
    if(!m_config->szOrderKey.empty())
    {
        m_redis->del(m_config->szOrderKey.c_str());
    }
    if(!m_config->szMatchKey.empty())
    {
        m_redis->del(m_config->szMatchKey.c_str());
    }
};

void IaEtfFollowTradeBotGf::enterOrder(json &msg)
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

void IaEtfFollowTradeBotGf::enterOrders(json &msg)
{
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

void IaEtfFollowTradeBotGf::cancelOrder(json &msg)
{
    TiReqOrderDelete req;
    memset(&req, 0, sizeof(TiReqOrderDelete));
    req.nOrderId = msg["nOrderId"];
    strcpy(req.szOrderStreamId, std::string(msg["szOrderStreamId"]).c_str());
    strcpy(req.szAccount, std::string(msg["szAccount"]).c_str());

    m_trade_client->orderDelete(&req);

};

////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////
TiTraderClient* IaEtfFollowTradeBotGf::GetTraderClient()
{
    return m_trade_client;
};