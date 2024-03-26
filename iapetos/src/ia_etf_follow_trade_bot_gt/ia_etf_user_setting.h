#ifndef __IA_ETF_USER_SETTING_H__
#define __IA_ETF_USER_SETTING_H__

#include <string>
#include <vector>

#include "redis_sync_handle.h"

class IaEtfUserSetting
{
public:
    IaEtfUserSetting(RedisSyncHandle* redis_client);
    ~IaEtfUserSetting();

private:
    RedisSyncHandle* m_redis_client;

private:
    std::vector<std::string> m_monitor_etf_symbol_vec;  //监控ETF清单

};

#endif // __IA_ETF_USER_SETTING_H__