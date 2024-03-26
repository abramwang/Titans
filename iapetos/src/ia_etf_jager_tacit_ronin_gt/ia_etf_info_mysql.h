#ifndef __IA_ETF_INFO_MYSQL_H__
#define __IA_ETF_INFO_MYSQL_H__

#include "ti_mysql_client.h"
#include "ia_etf_info_struct.h"

class IaEtfInfoMysql:
    public TiMysqlClient
{

public:
    IaEtfInfoMysql(
        const std::string& szIp,
        int nPort,
        const std::string& szUser,
        const std::string& szPassword,
        const std::string& szDb
    );
    ~IaEtfInfoMysql();

private:
    std::string format_vec(const std::vector<std::string>& vec);
public:
    void QueryEtfInfoList(int32_t date_num, std::vector<std::string> fund_symbol_vec, std::vector<IaEtfInfo>& out);
    void QueryEtfConstituentInfoList(int32_t date_num, std::vector<std::string> fund_symbol_vec, std::vector<IaEtfConstituentInfo>& out);
};

#endif // __IA_ETF_INFO_MYSQL_H__