#include "ia_etf_user_setting.h"
#include <iostream>

IaEtfUserSetting::IaEtfUserSetting(RedisSyncHandle* redis_client, IaEtfInfoMysql* etf_info_mysql)
{
    m_redis_client = redis_client;
    m_etf_info_mysql_client = etf_info_mysql;

    init_monitor_etf_symbol();
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
    std::vector<IaEtfInfo> etfInfoList;
    m_etf_info_mysql_client->QueryEtfInfoList(20240322, m_monitor_etf_symbol_vec, etfInfoList);

    std::cout << etfInfoList.size() << std::endl;

    for (auto& etfInfo : etfInfoList) {
        std::cout << "etfInfo.m_tradeDate:" << etfInfo.m_tradeDate << std::endl;
        std::cout << "etfInfo.m_fundId:" << etfInfo.m_fundId << std::endl;
        std::cout << "etfInfo.m_exchange:" << etfInfo.m_exchange << std::endl;
        std::cout << "etfInfo.m_company:" << etfInfo.m_company << std::endl;
        std::cout << "etfInfo.m_fundName:" << etfInfo.m_fundName << std::endl;
        std::cout << "etfInfo.m_minUnit:" << etfInfo.m_minUnit << std::endl;
        std::cout << "etfInfo.m_preMinUnitTurnover:" << etfInfo.m_preMinUnitTurnover << std::endl;
        std::cout << "etfInfo.m_publicEstimatedCashDifference:" << etfInfo.m_publicEstimatedCashDifference << std::endl;
        std::cout << "etfInfo.m_realEstimatedCashDifference:" << etfInfo.m_realEstimatedCashDifference << std::endl;
        std::cout << "etfInfo.m_allowCreation:" << etfInfo.m_allowCreation << std::endl;
        std::cout << "etfInfo.m_allowRedemption:" << etfInfo.m_allowRedemption << std::endl;
        std::cout << "etfInfo.m_maxCashReplacePercentage:" << etfInfo.m_maxCashReplacePercentage << std::endl;
        std::cout << "etfInfo.m_maxCreationVol:" << etfInfo.m_maxCreationVol << std::endl;
        std::cout << "etfInfo.m_maxRedemptionVol:" << etfInfo.m_maxRedemptionVol << std::endl;
        std::cout << "etfInfo.m_requiredToDiscloseIOPV:" << etfInfo.m_requiredToDiscloseIOPV << std::endl;
        std::cout << "etfInfo.m_constituentStockNum:" << etfInfo.m_constituentStockNum << std::endl;
        break;
    }

    std::vector<IaEtfConstituentInfo> constituentInfoVec;
    m_etf_info_mysql_client->QueryEtfConstituentInfoList(20240322, m_monitor_etf_symbol_vec, constituentInfoVec);
    
    std::cout << constituentInfoVec.size() << std::endl;
    for (auto& constituentInfo : constituentInfoVec) {
        std::cout << "constituentInfo.m_tradeDate:" << constituentInfo.m_tradeDate << std::endl;
        std::cout << "constituentInfo.m_fundId:" << constituentInfo.m_fundId << std::endl;
        std::cout << "constituentInfo.m_symbol:" << constituentInfo.m_symbol << std::endl;
        std::cout << "constituentInfo.m_exchange:" << constituentInfo.m_exchange << std::endl;
        std::cout << "constituentInfo.m_name:" << constituentInfo.m_name << std::endl;
        std::cout << "constituentInfo.m_replace_flag:" << (int)constituentInfo.m_replace_flag << std::endl;
        std::cout << "constituentInfo.m_replace_amount:" << constituentInfo.m_replace_amount << std::endl;
        std::cout << "constituentInfo.m_creation_amount:" << constituentInfo.m_creation_amount << std::endl;
        std::cout << "constituentInfo.m_redemption_amount:" << constituentInfo.m_redemption_amount << std::endl;
        std::cout << "constituentInfo.m_disclosure_vol:" << constituentInfo.m_disclosure_vol << std::endl;
        std::cout << "constituentInfo.m_reality_vol:" << constituentInfo.m_reality_vol << std::endl;
        std::cout << "constituentInfo.m_cash_replaced_creation_premium_rate:" << constituentInfo.m_cash_replaced_creation_premium_rate << std::endl;
        std::cout << "constituentInfo.m_cash_replaced_redemption_discount_rate:" << constituentInfo.m_cash_replaced_redemption_discount_rate << std::endl;

        break;
    }
    
};