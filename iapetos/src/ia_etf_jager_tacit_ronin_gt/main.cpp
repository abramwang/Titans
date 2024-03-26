#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>

#include "ia_etf_follow_trade_bot_gt.h"

int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    IaEtfFollowTradeBotGt* commander = new IaEtfFollowTradeBotGt(loop, "./config.ini");

    commander->listen();
}
