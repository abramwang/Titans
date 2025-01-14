#include <iostream>
#include <unistd.h>
#include <glog/logging.h>
#include "ti_quote_ipc_client.h"
#include "oc_ipc_quote_depth_server.h"


int main(int argc, char* argv[]) 
{
    int threadpool_size = 6;
    char value[32] = {0};
    sprintf(value, "%d", threadpool_size);
    setenv("UV_THREADPOOL_SIZE", value, 0);

    const char* _threadpool_size = std::getenv("UV_THREADPOOL_SIZE");
    if (_threadpool_size) {
        std::cout << "UV_THREADPOOL_SIZE: " << _threadpool_size << std::endl;
    } else {
        std::cout << "UV_THREADPOOL_SIZE is not set. Default size: 4" << std::endl;
    }

	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    OcIpcQuoteDepthServer cb(loop, threadpool_size);

    TiQuoteIpcClient client("./config.ini", loop, &cb);
	
    client.run(NULL);
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
