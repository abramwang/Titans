#include <iostream>
#include <unistd.h>
#include "ia_etf_jager.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>

IaEtfJager::IaEtfJager(uv_loop_s* loop, std::string configPath)
{
    m_loop = loop;

    m_redis = new RedisSyncHandle();
    m_quote_client = new TiQuoteIpcClient(configPath, loop, this);
    m_config = NULL;
    m_mysql = NULL;
    m_user_setting = NULL;
    m_quote_cache = NULL;
    m_signal_center = NULL;

    loadConfig(configPath);

    ///*
    if(m_config){
        bool flag = m_redis->connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[IaEtfFollowTradeBotGf] m_redis->connect flag: " << flag;

        resetStreamKey();

        m_mysql = new IaEtfInfoMysql(m_config->szSqlIp.c_str(), m_config->nSqlPort, m_config->szSqlUser.c_str(), m_config->szSqlPassword.c_str());
        m_user_setting = new IaEtfUserSetting(m_redis, m_mysql);
        m_quote_cache = new IaEtfQuoteDataCache();
        m_signal_center = new IaEtfSignalCenter(m_user_setting, m_quote_cache);
        m_influxdb_client = new IaEtfFactorToInflux(m_config->szInfluxUrl.c_str(), m_config->szInfluxToken.c_str());
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);

    m_quote_client->run(NULL);
}
IaEtfJager::~IaEtfJager(){
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
void IaEtfJager::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData)
{
    Locker locker(&m_mutex);
    m_quote_cache->OnL2IndexSnapshotRtn(pData);
};

void IaEtfJager::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    Locker locker(&m_mutex);
    m_quote_cache->OnL2FutureSnapshotRtn(pData);
};

void IaEtfJager::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    Locker locker(&m_mutex);
    m_quote_cache->OnL2StockSnapshotRtn(pData);
    m_signal_center->OnL2StockSnapshotRtn(pData);
};

void IaEtfJager::OnTimer()
{
    Locker locker(&m_mutex);
    m_signal_center->OnTimer();

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
            std::cout << "[IaEtfJager::OnTimer] signal_size: " << signal_out.size() << std::endl;
        }catch(std::exception& e){
            std::cout << "[IaEtfJager::OnTimer] " << e.what() << std::endl;
        }catch(...){
            std::cout << "[IaEtfJager::OnTimer] " << "unknown exception" << std::endl;
        }
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

void IaEtfJager::onTimer(uv_timer_t* handle)
{
    IaEtfJager* pThis = (IaEtfJager*)handle->data;
    pThis->OnTimer();
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int IaEtfJager::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["ia_etf_follow_trade_bot_gf"]["ip"]);
    m_config->nPort       = _iniFile["ia_etf_follow_trade_bot_gf"]["port"];
    m_config->szAuth      = string(_iniFile["ia_etf_follow_trade_bot_gf"]["auth"]);

    m_config->szSignalMap   = string(_iniFile["ia_etf_follow_trade_bot_gf"]["signal_map"]);
    
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
        m_config->szSignalMap.empty() )
    {
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void IaEtfJager::resetStreamKey()
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