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

public:
    void QueryEtfInfoList(std::vector<IaEtfInfo>& etfInfoList);
};

#endif // __IA_ETF_INFO_MYSQL_H__