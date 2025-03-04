#include <iostream>
#include <unistd.h>
#include <glog/logging.h>
#include "ti_quote_ipc_client.h"
#include "ti_thread_tool.h"
#include "oc_ipc_quote_depth_server.h"


int main(int argc, char* argv[]) 
{
    int threadpool_size = 4;
    char value[32] = {0};
    sprintf(value, "%d", threadpool_size);
    setenv("UV_THREADPOOL_SIZE", value, 0);

    int core_id = 0;
    if (argc == 2)
    {
        core_id = atoi(argv[1]);
    }

    const char* _threadpool_size = std::getenv("UV_THREADPOOL_SIZE");
    if (_threadpool_size) {
        std::cout << "UV_THREADPOOL_SIZE: " << _threadpool_size << std::endl;
    } else {
        std::cout << "UV_THREADPOOL_SIZE is not set. Default size: 4" << std::endl;
    }

    if(core_id > 0)
    {
        TiThreadTool::set_thread_affinity(core_id);
        core_id++;
    }

	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    OcIpcQuoteDepthServer cb(loop, threadpool_size, core_id);

    TiQuoteIpcClient client("./config.ini", loop, &cb);
	
    client.run(NULL);
    
    return uv_run(loop, UV_RUN_DEFAULT);
}
