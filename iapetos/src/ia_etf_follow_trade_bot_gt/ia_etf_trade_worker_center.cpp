#include "ia_etf_trade_worker_center.h"

IaEtfTradeWorkerCenter::IaEtfTradeWorkerCenter(TiGtTraderClient* trade_client, IaEtfQuoteDataCache* quote_cache, IaEtfInfoMysql* mysql)
{
    m_trade_client = trade_client;
    m_quote_cache = quote_cache;
    m_mysql = mysql;

    init_etf_info();
}

IaEtfTradeWorkerCenter::~IaEtfTradeWorkerCenter()
{
}


void IaEtfTradeWorkerCenter::init_etf_info()
{
    std::vector<std::string> fund_symbol_vec;
    fund_symbol_vec.push_back("159150");
    fund_symbol_vec.push_back("159350");
    fund_symbol_vec.push_back("159801");


    std::vector<IaEtfInfo> etfInfoList;
    m_mysql->QueryEtfInfoList(20240322, fund_symbol_vec, etfInfoList);

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
    m_mysql->QueryEtfConstituentInfoList(20240322, fund_symbol_vec, constituentInfoVec);
    
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