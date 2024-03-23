#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>

#include "oc_trader_commander_gt.h"
#include "ti_mysql_client.h"

int main(int argc, char* argv[]) {
    TiMysqlClient sql_client(
        "119.13.76.95",
        20000,
        "ustc",
        "72666db4-a446-4db5-9f85-d60b83dfba93",
        "pcf"
    );

    json j;

    sql_client.query("SELECT * FROM etf_info ORDER BY m_tradeDate DESC LIMIT 100;", j);

    std::cout << j << std::endl;

    return 0;

	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    OcTraderCommanderGt* commander = new OcTraderCommanderGt(loop, "./config.ini");

    commander->listen();
}
