#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>

#include "oc_trader_commander_huaxin.h"

int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    OcTraderCommanderHuaxin* commander = new OcTraderCommanderHuaxin(loop, "./config.ini");

    commander->listen();
}
