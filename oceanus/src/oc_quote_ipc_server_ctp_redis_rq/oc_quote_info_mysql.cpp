#include "oc_quote_info_mysql.h"
#include "datetime.h"

OcQuoteInfoMysql::OcQuoteInfoMysql(const std::string& szIp, int nPort, const std::string& szUser, const std::string& szPassword)
{
    m_ti_data_db = new TiMysqlClient(szIp, nPort, szUser, szPassword, "ti_data");
};

OcQuoteInfoMysql::~OcQuoteInfoMysql(){};


std::string OcQuoteInfoMysql::format_vec(const std::vector<std::string>& vec)
{
    std::string str;
    for (size_t i = 0; i < vec.size(); i++) {
        str +=  "'" + vec[i] + "',"; 
    }
    str.pop_back();
    return str;
}

void OcQuoteInfoMysql::QueryInstrumentInfoList(std::vector<OCInstrumentInfo>& out)
{
    int date_num = datetime::get_today();

    std::vector<std::map<std::string, std::string>> result;
    char sql[256] = {0};
    sprintf(sql, "SELECT * FROM ti_instrument_info WHERE update_date = %d;", date_num);
    m_ti_data_db->query(sql, result);
    for (auto& row : result) {
        OCInstrumentInfo info;
        info.symbol = row["symbol"];
        info.exchange = row["exchange"];
        info.name = row["name"];
        info.type = OcQuoteInfoStruct::pares_instrument_type(row["type"]);
        info.limit_up = atof(row["limit_up"].c_str());
        info.limit_down = atof(row["limit_down"].c_str());
        info.pre_close = atof(row["pre_close"].c_str());
        info.update_date = row["update_date"];
        out.push_back(info);
    }
};