#include "ia_etf_user_setting.h"
#include "datetime.h"

#include <iostream>

IaEtfUserSetting::IaEtfUserSetting(RedisSyncHandle* redis_client, IaEtfInfoMysql* etf_info_mysql)
{
    m_redis_client = redis_client;
    m_etf_info_mysql_client = etf_info_mysql;

    //init_monitor_etf_symbol();
    init_etf_info();

    std::cout << m_monitor_etf_symbol_vec.size() << std::endl;
}

IaEtfUserSetting::~IaEtfUserSetting()
{
}


void IaEtfUserSetting::init_monitor_etf_symbol()
{
    m_redis_client->smembers("ia_etf_jager_tacit_ronin_gt.monitor_symbol", m_monitor_etf_symbol_vec);
}

void IaEtfUserSetting::init_etf_info()
{
    int32_t last_trading_date_num = m_etf_info_mysql_client->QueryLatestTradingDate();
    int date_num = datetime::get_today();
    date_num = last_trading_date_num ? last_trading_date_num : date_num;

    //date_num = 20240402;

    std::set<std::string> symbol_set;

    std::vector<IaEtfInfo> etfInfoList;
    m_etf_info_mysql_client->QueryEtfInfoList(date_num, etfInfoList);
    //m_etf_info_mysql_client->QueryEtfInfoList(date_num, m_monitor_etf_symbol_vec, etfInfoList);

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