#ifndef __IA_ETF_USER_SETTING_H__
#define __IA_ETF_USER_SETTING_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>

#include "redis_sync_handle.h"
#include "ia_etf_info_mysql.h"

class IaEtfUserSetting
{
private:
    RedisSyncHandle* m_redis_client;
    IaEtfInfoMysql* m_etf_info_mysql_client;

private:
    std::vector<std::string> m_monitor_etf_symbol_vec;  //监控ETF清单

    std::unordered_map<std::string, std::shared_ptr<IaEtfInfo>> m_etf_info_map;                     //etf info map              fund_symbol -> etf info
    std::multimap<std::string, std::shared_ptr<IaEtfConstituentInfo>> m_etf_constituent_info_map;   //etf constituent info map  fund_symbol -> etf constituent info

    std::unordered_map<std::string, std::shared_ptr<IaAccountDBInfo>> m_account_info_map;           //account info map          account_id -> account info
private:
    void init_etf_info(bool only_use_fitted_pcf);
    void init_account_info();

public:
    IaEtfUserSetting(RedisSyncHandle* redis_client, IaEtfInfoMysql* etf_info_mysql);
    IaEtfUserSetting(RedisSyncHandle* redis_client, IaEtfInfoMysql* etf_info_mysql, bool only_use_fitted_pcf);
    ~IaEtfUserSetting();

public:
    bool GetMonitorEtfSymbol(std::vector<std::string> &monitor_etf_symbol_vec);
    bool GetEtfInfo(std::string fund_symbol, std::shared_ptr<IaEtfInfo> &etf_info_ptr, std::vector<std::shared_ptr<IaEtfConstituentInfo>> &constituent_info_vec);
    bool GetAccountDBInfo(std::string account, std::shared_ptr<IaAccountDBInfo> &account_info_ptr);

};

#endif // __IA_ETF_USER_SETTING_H__