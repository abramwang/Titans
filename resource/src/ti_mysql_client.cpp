#include "ti_mysql_client.h"


TiMysqlClient::TiMysqlClient(std::string host, u_int32_t port, std::string user, std::string password, std::string database)
{
    m_conn = mysql_init(NULL);
    if (m_conn == NULL) {
        std::cout << "Error: mysql_init() failed\n";
    }

    m_host = host;
    m_port = port;
    m_user = user;
    m_password = password;
    m_database = database;

    if (!mysql_real_connect(m_conn, host.c_str(), user.c_str(), password.c_str(), database.c_str(), port, NULL, 0)) {
        std::cout << "Error: mysql_real_connect() failed\n";
        mysql_close(m_conn);
        return;
    }

    if (mysql_query(m_conn, "SET NAMES 'utf8'")) {
        std::cerr << "Failed to set character set to UTF-8: Error: " << mysql_error(m_conn) << std::endl;
    }
}

TiMysqlClient::~TiMysqlClient()
{
    m_conn = NULL;
}


void TiMysqlClient::query(std::string sql, json& j)
{
    if (mysql_query(m_conn, sql.c_str())) {
        std::cout << "Error: mysql_query() failed\n";
        return;
    }

    MYSQL_RES *res = mysql_store_result(m_conn);
    if (res == NULL) {
        std::cout << "Error: mysql_store_result() failed\n";
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        json j_row;
        for (unsigned int i = 0; i < mysql_num_fields(res); i++) {
            //std::cout << mysql_fetch_field_direct(res, i)->type << ": " << row[i] << std::endl;
            j_row[mysql_fetch_field_direct(res, i)->name] = row[i];
        }
        j.push_back(j_row);
    }

    mysql_free_result(res);
}


void TiMysqlClient::query(std::string sql, std::vector<std::map<std::string, std::string>> &out)
{
    if (mysql_query(m_conn, sql.c_str())) {
        std::cout << "Error: mysql_query() failed\n";
        return;
    }

    MYSQL_RES *res = mysql_store_result(m_conn);
    if (res == NULL) {
        std::cout << "Error: mysql_store_result() failed\n";
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        std::map<std::string, std::string> row_map;
        for (unsigned int i = 0; i < mysql_num_fields(res); i++) {
            row_map[mysql_fetch_field_direct(res, i)->name] = row[i];
        }
        out.push_back(row_map);
    }

    mysql_free_result(res);
};