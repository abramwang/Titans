#include <iostream>
#include <unistd.h>
#include <glog/logging.h>
#include "ti_quote_ipc_client.h"
#include "oc_ipc_quote_depth_server.h"


int main(int argc, char* argv[]) 
{
    char value[32] = {0};
    sprintf(value, "%d", 0);
    setenv("UV_THREADPOOL_SIZE", value, 0);


	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    OcIpcQuoteDepthServer cb(loop);

    TiQuoteIpcClient client("./config.ini", loop, &cb);
	
    client.run(NULL);
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
