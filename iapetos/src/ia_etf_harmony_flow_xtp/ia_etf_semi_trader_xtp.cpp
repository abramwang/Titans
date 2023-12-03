#include <iostream>
#include <unistd.h>
#include "ia_etf_semi_trader_xtp.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include "ti_trader_formater.h"
#include "ia_etf_semi_pilot.h"

IaETFSemiTraderXtp::IaETFSemiTraderXtp(uv_loop_s* loop, std::string configPath, IaCommandInterface* commander)
    : RedisCommander(loop)
{
    m_client = new TiZtTraderClient(configPath, this);
    m_config = NULL;
    m_commander = commander;
    m_total_asset = 0;
    m_cash_asset = 0;
    m_frozen_cash = 0;
    m_signal_count = 0;
    m_query_time = 0;
    m_heartbeat_time = 0;
    loadConfig(configPath);
    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[IaETFFactorCenter] flag: " << flag;
        resetStreamKey();
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);
}
IaETFSemiTraderXtp::~IaETFSemiTraderXtp(){
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
void IaETFSemiTraderXtp::OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str)
{
    Locker locker(&m_mutex);
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
    if ((*rspData)["type"] == "XtpOnQueryAsset")
    {
        m_query_time = datetime::get_now_timestamp_ms();
        m_total_asset = (*rspData)["data"]["total_asset"].get<double>();
        m_cash_asset = (*rspData)["data"]["buying_power"].get<double>();
        std::cout << "XtpOnQueryAsset: " << *rspData <<  " m_frozen_cash: " << m_frozen_cash << std::endl;
        return;
    }
};   

void IaETFSemiTraderXtp::OnRspOrderDelete(const TiRspOrderDelete* pData)
{
    Locker locker(&m_mutex);
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspOrderDelete(pData);
    }
};
void IaETFSemiTraderXtp::OnRspQryOrder(const TiRspQryOrder* pData, bool isLast)
{
    /*
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspQryOrder(pData, isLast);
    }
    */
    if (m_config)
    {
        if(!m_config->szOrderKey.empty())
        {
            TiTraderFormater::FormatOrderStatus(pData, m_json_cash);
            m_redis.hmset(m_config->szOrderKey.c_str(), m_json_cash["szOrderId"].get<std::string>().c_str(), m_json_cash.dump().c_str());
        }
    }
};
void IaETFSemiTraderXtp::OnRspQryMatch(const TiRspQryMatch* pData, bool isLast)
{
    /*
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspQryMatch(pData, isLast);
    }
    */
    if (m_config)
    {
        if(!m_config->szMatchKey.empty())
        {
            TiTraderFormater::FormatOrderMatchEvent(pData, m_json_cash);
            m_redis.hmset(m_config->szMatchKey.c_str(), m_json_cash["szStreamId"].get<std::string>().c_str(), m_json_cash.dump().c_str());
        }
    }
};
void IaETFSemiTraderXtp::OnRspQryPosition(const TiRspQryPosition* pData, bool isLast)
{
    /*
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRspQryPosition(pData, isLast);
    }
    */
};
void IaETFSemiTraderXtp::OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData)
{
    Locker locker(&m_mutex);
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRtnOrderStatusEvent(pData);
    }
    if (m_config)
    {
        if(!m_config->szOrderKey.empty() && !m_config->szTradingStreamKey.empty())
        {
            TiTraderFormater::FormatOrderStatus(pData, m_json_cash);
            m_redis.hmset(m_config->szOrderKey.c_str(), m_json_cash["szOrderId"].get<std::string>().c_str(), m_json_cash.dump().c_str());
            /*
            m_json_msg = {
                {"msgType", "orderStatus"},
                {"data", m_json_cash}
            };
            m_redis.xadd(m_config->szTradingStreamKey.c_str(), m_json_msg.dump().c_str());
            */
        }
    }
};
void IaETFSemiTraderXtp::OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData)
{
    Locker locker(&m_mutex);
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnRtnOrderMatchEvent(pData);
    }
    if (m_config)
    {
        if(!m_config->szMatchKey.empty() && !m_config->szTradingStreamKey.empty())
        {
            TiTraderFormater::FormatOrderMatchEvent(pData, m_json_cash);
            m_redis.hmset(m_config->szMatchKey.c_str(), m_json_cash["szStreamId"].get<std::string>().c_str(), m_json_cash.dump().c_str());
            /*
            m_json_msg = {
                {"msgType", "orderMatch"},
                {"data", m_json_cash}
            };
            m_redis.xadd(m_config->szTradingStreamKey.c_str(), m_json_msg.dump().c_str());
            */
        }
    }
};


void IaETFSemiTraderXtp::OnTimer()
{
    Locker locker(&m_mutex);
    for (auto iter = m_workerList.begin(); iter != m_workerList.end(); iter++)
    {
        (*iter)->OnTimer();
    }

    int64_t now = datetime::get_now_timestamp_ms();
    if ((now - m_query_time) > 1000 * 60 * 2) //2分钟查询一次
    {
        m_client->QueryAsset();
        m_query_time = now;
    }
    if ((now - m_heartbeat_time) > 1000 * 30 ) //30s 一个心跳
    {
        m_heartbeat_time = now;
        m_redis.hmset(
            "task_worked_time", 
            "ia_etf_harmony_flow_xtp", 
            datetime::get_format_now_time_ms().c_str());
    }
};


void IaETFSemiTraderXtp::onTimer(uv_timer_t* handle)
{
    IaETFSemiTraderXtp* pThis = (IaETFSemiTraderXtp*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void IaETFSemiTraderXtp::onAuth(int err, const char* errStr){
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

void IaETFSemiTraderXtp::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void IaETFSemiTraderXtp::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    if (m_commander)
    {
        m_commander->OnCommandRtn(type, msg);
    }
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int IaETFSemiTraderXtp::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["ia_etf_semi_trader_xtp"]["ip"]);
    m_config->nPort       = _iniFile["ia_etf_semi_trader_xtp"]["port"];
    m_config->szAuth      = string(_iniFile["ia_etf_semi_trader_xtp"]["auth"]);

    m_config->nBlock          = _iniFile["ia_etf_semi_trader_xtp"]["block"];
    m_config->szCommandStreamKey     = string(_iniFile["ia_etf_semi_trader_xtp"]["command_stream_key"]);
    m_config->szCommandStreamGroup   = string(_iniFile["ia_etf_semi_trader_xtp"]["command_stream_group"]);
    m_config->szCommandConsumerId   = string(_iniFile["ia_etf_semi_trader_xtp"]["command_consummer_id"]);
    
    m_config->szTradingStreamKey = string(_iniFile["ia_etf_semi_trader_xtp"]["trading_stream_key"]);
    m_config->szOrderKey         = string(_iniFile["ia_etf_semi_trader_xtp"]["order_key"]);
    m_config->szMatchKey         = string(_iniFile["ia_etf_semi_trader_xtp"]["match_key"]);
    
    m_config->szWorkerSignalStreamKey         = string(_iniFile["ia_etf_semi_trader_xtp"]["worker_signal_key"]);
    m_config->szWorkerStatusKey         = string(_iniFile["ia_etf_semi_trader_xtp"]["worker_status_key"]);

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

void IaETFSemiTraderXtp::resetStreamKey()
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
    if(!m_config->szTradingStreamKey.empty())
    {
        m_redis.del(m_config->szTradingStreamKey.c_str());
    }
    if(!m_config->szOrderKey.empty())
    {
        m_redis.del(m_config->szOrderKey.c_str());
    }
    if(!m_config->szMatchKey.empty())
    {
        m_redis.del(m_config->szMatchKey.c_str());
    }
    if(!m_config->szWorkerSignalStreamKey.empty())
    {
        m_redis.del(m_config->szWorkerSignalStreamKey.c_str());
    }
    if(!m_config->szWorkerStatusKey.empty())
    {
        m_redis.del(m_config->szWorkerStatusKey.c_str());
    }
};

////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////
double IaETFSemiTraderXtp::GetAvailableCash()
{
    return m_cash_asset - m_frozen_cash;
};
TiTraderClient* IaETFSemiTraderXtp::GetTraderClient()
{
    return m_client;
};

void IaETFSemiTraderXtp::AddWorker(std::shared_ptr<IaETFTradingWorker> worker_ptr)
{
    m_workerList.push_back(worker_ptr);
};

void IaETFSemiTraderXtp::PublicWorkerSignal(json &signal)
{
    if (!m_config)
    {
        return;
    }
    if (m_config->szWorkerSignalStreamKey.empty())
    {
        return;
    }
    m_redis.xadd(m_config->szWorkerSignalStreamKey.c_str(), signal.dump().c_str());
};

void IaETFSemiTraderXtp::UpdateWorkerStatus(std::string id, json &status)
{
    if (!m_config)
    {
        return;
    }
    if (m_config->szWorkerStatusKey.empty())
    {
        return;
    }
    m_redis.hset(m_config->szWorkerStatusKey.c_str(), id.c_str(), status.dump().c_str());
};