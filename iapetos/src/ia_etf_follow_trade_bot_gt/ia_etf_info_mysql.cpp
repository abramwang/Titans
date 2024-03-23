#include "ia_etf_info_mysql.h"

IaEtfInfoMysql::IaEtfInfoMysql(
    const std::string& szIp,
    int nPort,
    const std::string& szUser,
    const std::string& szPassword,
    const std::string& szDb
)
    : TiMysqlClient(szIp, nPort, szUser, szPassword, szDb)
{

};

IaEtfInfoMysql::~IaEtfInfoMysql(){};


void IaEtfInfoMysql::QueryEtfInfoList(std::vector<IaEtfInfo>& etfInfoList)
{
    std::vector<std::map<std::string, std::string>> result;
    query("SELECT * FROM etf_info ORDER BY m_tradeDate DESC LIMIT 100;", result);

    for (auto& row : result) {
        IaEtfInfo etfInfo;
        etfInfo.m_tradeDate = row["m_tradeDate"];
        etfInfo.m_fundId = row["m_fundId"];
        etfInfo.m_exchange = row["m_exchange"];
        etfInfo.m_company = row["m_company"].c_str();
        etfInfo.m_fundName = row["m_fundName"].c_str();
        etfInfo.m_minUnit = atof(row["m_minUnit"].c_str());
        etfInfo.m_preMinUnitTurnover = atof(row["m_preMinUnitTurnover"].c_str());
        etfInfo.m_publicEstimatedCashDifference = atof(row["m_publicEstimatedCashDifference"].c_str());
        etfInfo.m_realEstimatedCashDifference = atof(row["m_realEstimatedCashDifference"].c_str());
        etfInfo.m_allowCreation = atof(row["m_allowCreation"].c_str());
        etfInfo.m_allowRedemption = atof(row["m_allowRedemption"].c_str());
        etfInfo.m_maxCashReplacePercentage = atof(row["m_maxCashReplacePercentage"].c_str());
        etfInfo.m_maxCreationVol = atof(row["m_maxCreationVol"].c_str());
        etfInfo.m_maxRedemptionVol = atof(row["m_maxRedemptionVol"].c_str());
        etfInfo.m_requiredToDiscloseIOPV = atof(row["m_requiredToDiscloseIOPV"].c_str());
        etfInfo.m_constituentStockNum = atof(row["m_constituentStockNum"].c_str());
        etfInfoList.push_back(etfInfo);
    }
};