#ifndef __IA_ETF_INFO_MYSQL_H__
#define __IA_ETF_INFO_MYSQL_H__

#include "ti_mysql_client.h"
#include "ia_etf_info_struct.h"

class IaEtfInfoMysql
{
public:
    IaEtfInfoMysql(const std::string& szIp, int nPort, const std::string& szUser, const std::string& szPassword);
    ~IaEtfInfoMysql();
private:
    TiMysqlClient* m_pcf_db;
    TiMysqlClient* m_ref_db;
    TiMysqlClient* m_ti_db;

private:
    std::string format_vec(const std::vector<std::string>& vec);
public:
    int32_t QueryLatestTradingDate();
    void QueryEtfInfoList(int32_t date_num, std::vector<std::string>& fund_symbol_vec, std::vector<IaEtfInfo>& out);
    void QueryEtfConstituentInfoList(int32_t date_num, std::vector<std::string> fund_symbol_vec, std::vector<IaEtfConstituentInfo>& out);

    void QueryAccountInfoList(std::vector<IaAccountDBInfo>& out);

    // 和拟合pcf相关的接口
    void QueryFittedEtfList(int32_t date_num, std::vector<std::string>& fund_symbol_vec);
    void QueryFittedEtfInfoList(int32_t date_num, std::vector<std::string>& fund_symbol_vec, std::vector<IaEtfInfo>& out);
    void QueryFittedEtfConstituentInfoList(int32_t date_num, std::vector<std::string>& fund_symbol_vec, std::vector<IaEtfConstituentInfo>& out);
};

#endif // __IA_ETF_INFO_MYSQL_H__