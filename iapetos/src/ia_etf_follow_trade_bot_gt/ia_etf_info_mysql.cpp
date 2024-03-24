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


std::string IaEtfInfoMysql::format_vec(const std::vector<std::string>& vec) {
    std::string str;
    for (size_t i = 0; i < vec.size(); i++) {
        str +=  "'" + vec[i] + "',"; 
    }
    str.pop_back();
    return str;
}

void IaEtfInfoMysql::QueryEtfInfoList(int32_t date_num, std::vector<std::string> fund_symbol_vec, std::vector<IaEtfInfo>& out)
{
    std::vector<std::map<std::string, std::string>> result;
    char sql[256] = {0};
    sprintf(sql, "SELECT * FROM etf_info WHERE m_tradeDate = %d", date_num);
    std::string sql_str = sql;
    if (!fund_symbol_vec.empty()) {
        sql_str += " AND m_fundId IN (" + format_vec(fund_symbol_vec) + ");";
    }else{
        sql_str += ";";
    }
    std::cout << sql_str << std::endl;
    query(sql_str, result);
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
        out.push_back(etfInfo);
    }
};

void IaEtfInfoMysql::QueryEtfConstituentInfoList(int32_t date_num, std::vector<std::string> fund_symbol_vec, std::vector<IaEtfConstituentInfo>& out)
{
    std::vector<std::map<std::string, std::string>> result;
    char sql[256] = {0};
    sprintf(sql, "SELECT * FROM constituent_info WHERE m_tradeDate = %d", date_num);
    std::string sql_str = sql;
    if (!fund_symbol_vec.empty()) {
        sql_str += " AND m_fundId IN (" + format_vec(fund_symbol_vec) + ");";
    }else{
        sql_str += ";";
    }
    query(sql_str, result);
    for (auto& row : result) {
        IaEtfConstituentInfo constituentInfo;
        constituentInfo.m_tradeDate = row["m_tradeDate"];
        constituentInfo.m_fundId = row["m_fundId"];
        constituentInfo.m_symbol = row["m_symbol"];
        constituentInfo.m_exchange = row["m_exchange"];
        constituentInfo.m_name = row["m_name"];
        constituentInfo.m_replace_flag = IaEtfInfoStruct::pares_ert_cash_type(row["m_replace_flag"]);
        constituentInfo.m_replace_amount = atof(row["m_replace_amount"].c_str());
        constituentInfo.m_creation_amount = atof(row["m_creation_amount"].c_str());
        constituentInfo.m_redemption_amount = atof(row["m_redemption_amount"].c_str());
        constituentInfo.m_disclosure_vol = atof(row["m_disclosure_vol"].c_str());
        constituentInfo.m_reality_vol = atof(row["m_reality_vol"].c_str());
        constituentInfo.m_cash_replaced_creation_premium_rate = atof(row["m_cash_replaced_creation_premium_rate"].c_str());
        constituentInfo.m_cash_replaced_redemption_discount_rate = atof(row["m_cash_replaced_redemption_discount_rate"].c_str());
        out.push_back(constituentInfo);
    }
};