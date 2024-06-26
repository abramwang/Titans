﻿#include <iostream>
#include <unistd.h>
#include <set>
#include <glog/logging.h>

#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_quote_ipc_client.h"

#include "oc_ipc_quote_json_server.h"


int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    OcIpcQuoteJsonServer commander(loop, "./config.ini");
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
