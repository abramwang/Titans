#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>

#include "oc_quote_ipc_server_ctp_redis_xt.h"

int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    OcQuoteIpcServerCtpRedisXt* server = new OcQuoteIpcServerCtpRedisXt(loop, "./config.ini");

    return server->listen();
}
