#ifndef __IA_ETF_USER_SETTING_H__
#define __IA_ETF_USER_SETTING_H__

#include <string>
#include <vector>

#include "redis_sync_handle.h"
#include "ia_etf_info_mysql.h"

class IaEtfUserSetting
{
private:
    RedisSyncHandle* m_redis_client;
    IaEtfInfoMysql* m_etf_info_mysql_client;

private:
    std::vector<std::string> m_monitor_etf_symbol_vec;  //监控ETF清单

private:
    void init_monitor_etf_symbol();
    void init_etf_info();

public:
    std::vector<std::string>& get_monitor_etf_symbol_vec()
    {
        return m_monitor_etf_symbol_vec;
    }

public:
    IaEtfUserSetting(RedisSyncHandle* redis_client, IaEtfInfoMysql* etf_info_mysql);
    ~IaEtfUserSetting();
};

#endif // __IA_ETF_USER_SETTING_H__