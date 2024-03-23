#ifndef TI_MYSQL_CLIENT_H
#define TI_MYSQL_CLIENT_H

#include <string>
#include <iostream>
#include <mysql/mysql.h>

#include <vector>
#include <map>

#include <nlohmann/json.hpp>
using namespace nlohmann;

class TiMysqlClient
{
private:
    MYSQL *m_conn;
    std::string m_host;
    u_int32_t m_port;
    std::string m_user;
    std::string m_password;
    std::string m_database;
public:
    TiMysqlClient(std::string host, u_int32_t port, std::string user, std::string password, std::string database);
    ~TiMysqlClient();

public:
    void query(std::string sql, json& j);
    void query(std::string sql, std::vector<std::map<std::string, std::string>> &out);

};


#endif