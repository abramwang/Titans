#include <iostream>
#include <unistd.h>
#include <glog/logging.h>

#include "ia_etf_xtp_trader.h"

int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);
	
    uv_loop_t* loop = uv_default_loop();

    IaETFXtpTrader * commander = new IaETFXtpTrader(loop, "./config.ini");
	
    
	return commander->listen();
}
