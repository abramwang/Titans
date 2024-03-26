#include "ia_etf_user_setting.h"
#include <iostream>

IaEtfUserSetting::IaEtfUserSetting(RedisSyncHandle* redis_client)
{
    m_redis_client = redis_client;

    init_monitor_etf_symbol();

    std::cout << m_monitor_etf_symbol_vec.size() << std::endl;
}

IaEtfUserSetting::~IaEtfUserSetting()
{
}


void IaEtfUserSetting::init_monitor_etf_symbol()
{
    m_redis_client->smembers("ia_etf_jager_tacit_ronin_gt.monitor_symbol", m_monitor_etf_symbol_vec);
    /*
    m_monitor_etf_symbol_vec.push_back("159150");
    m_monitor_etf_symbol_vec.push_back("159350");
    m_monitor_etf_symbol_vec.push_back("159801");
    */
}
