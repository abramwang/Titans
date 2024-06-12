#ifndef __OC_QUOTE_INFO_MYSQL_H__
#define __OC_QUOTE_INFO_MYSQL_H__

#include "ti_mysql_client.h"
#include "oc_quote_info_struct.h"

class OcQuoteInfoMysql
{
public:
    OcQuoteInfoMysql(const std::string& szIp, int nPort, const std::string& szUser, const std::string& szPassword);
    ~OcQuoteInfoMysql();
private:
    TiMysqlClient* m_ti_data_db;

private:
    std::string format_vec(const std::vector<std::string>& vec);
public:
    void QueryInstrumentInfoList(std::vector<OCInstrumentInfo>& out);
};

#endif // __OC_QUOTE_INFO_MYSQL_H__