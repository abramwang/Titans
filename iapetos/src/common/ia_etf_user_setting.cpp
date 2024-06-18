#include "ia_etf_user_setting.h"
#include "datetime.h"

#include <iostream>

IaEtfUserSetting::IaEtfUserSetting(RedisSyncHandle* redis_client, IaEtfInfoMysql* etf_info_mysql)
{
    m_redis_client = redis_client;
    m_etf_info_mysql_client = etf_info_mysql;

    init_etf_info(false);
    init_account_info();

    std::cout << m_monitor_etf_symbol_vec.size() << std::endl;
}

IaEtfUserSetting::IaEtfUserSetting(RedisSyncHandle* redis_client, IaEtfInfoMysql* etf_info_mysql, bool only_use_fitted_pcf)
{
    m_redis_client = redis_client;
    m_etf_info_mysql_client = etf_info_mysql;

    init_etf_info(only_use_fitted_pcf);
    init_account_info();

    std::cout << m_monitor_etf_symbol_vec.size() << " only_use_fitted_pcf:" << only_use_fitted_pcf << std::endl;
};

IaEtfUserSetting::~IaEtfUserSetting()
{
}

void IaEtfUserSetting::init_etf_info(bool only_use_fitted_pcf)
{
    int32_t last_trading_date_num = m_etf_info_mysql_client->QueryLatestTradingDate();
    int date_num = datetime::get_today();
    date_num = last_trading_date_num ? last_trading_date_num : date_num;

    //date_num = 20240527;
    // 查询有真实清单的ETF
    if (!only_use_fitted_pcf) {
        std::vector<IaEtfInfo> etfInfoList;
        m_etf_info_mysql_client->QueryEtfInfoList(date_num, m_monitor_etf_symbol_vec, etfInfoList);
        std::cout << "realvol_fund_vec: " << etfInfoList.size() << std::endl;

        for (auto& etfInfo : etfInfoList) {
            std::shared_ptr<IaEtfInfo> info_ptr = std::make_shared<IaEtfInfo>(etfInfo);
            m_etf_info_map[etfInfo.m_fundId] = info_ptr;
        }

        std::vector<IaEtfConstituentInfo> constituentInfoVec;
        m_etf_info_mysql_client->QueryEtfConstituentInfoList(date_num, m_monitor_etf_symbol_vec, constituentInfoVec);
        
        std::cout << constituentInfoVec.size() << std::endl;
        for (auto& constituentInfo : constituentInfoVec) {
            std::shared_ptr<IaEtfConstituentInfo> info_ptr = std::make_shared<IaEtfConstituentInfo>(constituentInfo);
            m_etf_constituent_info_map.insert(std::make_pair(constituentInfo.m_fundId, info_ptr));
        }
    }

    // 补齐有拟合清单的ETF
    {
        std::vector<std::string> all_fitted_symbol_vec;
        m_etf_info_mysql_client->QueryFittedEtfList(date_num, all_fitted_symbol_vec);

        std::cout << "all_fitted_symbol_vec: " << all_fitted_symbol_vec.size() << std::endl;

        std::vector<IaEtfInfo> etfInfoWithFittedPcfList;
        m_etf_info_mysql_client->QueryFittedEtfInfoList(date_num, all_fitted_symbol_vec, etfInfoWithFittedPcfList);
        
        std::vector<std::string> fitted_symbol_vec;
        for (auto& etfInfo : etfInfoWithFittedPcfList) {
            auto iter = m_etf_info_map.find(etfInfo.m_fundId);
            if (iter != m_etf_info_map.end()) {
                continue;
            }

            std::shared_ptr<IaEtfInfo> info_ptr = std::make_shared<IaEtfInfo>(etfInfo);
            m_etf_info_map[etfInfo.m_fundId] = info_ptr;

            fitted_symbol_vec.push_back(etfInfo.m_fundId);
            m_monitor_etf_symbol_vec.push_back(etfInfo.m_fundId);
        }
            
        std::cout << "fitted_symbol_vec: " << fitted_symbol_vec.size() << std::endl;
        std::cout << "m_etf_info_map: " << m_etf_info_map.size() << std::endl;
        
        std::vector<IaEtfConstituentInfo> fittedConstituentInfoVec;
        m_etf_info_mysql_client->QueryFittedEtfConstituentInfoList(date_num, fitted_symbol_vec, fittedConstituentInfoVec);

        std::cout << "fittedConstituentInfoVec: " << fittedConstituentInfoVec.size() << std::endl;

        for (auto& constituentInfo : fittedConstituentInfoVec) {
            std::shared_ptr<IaEtfConstituentInfo> info_ptr = std::make_shared<IaEtfConstituentInfo>(constituentInfo);
            m_etf_constituent_info_map.insert(std::make_pair(constituentInfo.m_fundId, info_ptr));
        }
    }
    return;
#if 0


    std::set<std::string> symbol_set;

    std::vector<IaEtfInfo> etfInfoList;
    m_etf_info_mysql_client->QueryEtfInfoList(date_num, m_monitor_etf_symbol_vec, etfInfoList);

    std::cout << etfInfoList.size() << std::endl;

    for (auto& etfInfo : etfInfoList) {
        std::shared_ptr<IaEtfInfo> info_ptr = std::make_shared<IaEtfInfo>(etfInfo);
        m_etf_info_map[etfInfo.m_fundId] = info_ptr;

        symbol_set.insert(info_ptr->m_fundId);
    }

    std::vector<IaEtfConstituentInfo> constituentInfoVec;
    m_etf_info_mysql_client->QueryEtfConstituentInfoList(date_num, m_monitor_etf_symbol_vec, constituentInfoVec);
    

    std::cout << constituentInfoVec.size() << std::endl;
    for (auto& constituentInfo : constituentInfoVec) {
        std::shared_ptr<IaEtfConstituentInfo> info_ptr = std::make_shared<IaEtfConstituentInfo>(constituentInfo);
        m_etf_constituent_info_map.insert(std::make_pair(constituentInfo.m_fundId, info_ptr));

        symbol_set.insert(info_ptr->m_symbol);
    }

    std::cout << symbol_set.size() << std::endl;
#endif
};

void IaEtfUserSetting::init_account_info()
{
    std::vector<IaAccountDBInfo> accountInfoList;
    m_etf_info_mysql_client->QueryAccountInfoList(accountInfoList);

    for (auto& accountInfo : accountInfoList) {
        std::shared_ptr<IaAccountDBInfo> info_ptr = std::make_shared<IaAccountDBInfo>(accountInfo);
        m_account_info_map[accountInfo.funding_account] = info_ptr;
    }
};


bool IaEtfUserSetting::GetMonitorEtfSymbol(std::vector<std::string> &monitor_etf_symbol_vec)
{
    if (m_monitor_etf_symbol_vec.empty())
    {
        return false;
    }
    monitor_etf_symbol_vec = m_monitor_etf_symbol_vec;
    return true;
};

bool IaEtfUserSetting::GetEtfInfo(std::string fund_symbol, std::shared_ptr<IaEtfInfo> &etf_info_ptr, std::vector<std::shared_ptr<IaEtfConstituentInfo>> &constituent_info_vec)
{
    auto iter = m_etf_info_map.find(fund_symbol);
    if (iter == m_etf_info_map.end())
    {
        return false;
    }
    etf_info_ptr = iter->second;

    auto range = m_etf_constituent_info_map.equal_range(fund_symbol);
    for (auto it = range.first; it != range.second; ++it) {
        constituent_info_vec.push_back(it->second);
    }

    return true;
};

bool IaEtfUserSetting::GetAccountDBInfo(std::string account, std::shared_ptr<IaAccountDBInfo> &account_info_ptr)
{
    auto iter = m_account_info_map.find(account);
    if (iter == m_account_info_map.end())
    {
        return false;
    }
    account_info_ptr = iter->second;
    return true;
};