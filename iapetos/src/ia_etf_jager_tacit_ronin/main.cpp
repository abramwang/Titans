#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>

#include "ia_etf_jager.h"

int main(int argc, char* argv[]) {
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    bool enable_only_use_fitted_pcf = false;
    if (argc == 2)
    {
        enable_only_use_fitted_pcf = atoi(argv[1]);
    }

    uv_loop_t* loop = uv_default_loop();

    IaEtfJager* commander = new IaEtfJager(loop, "./config.ini", enable_only_use_fitted_pcf);

    uv_run(loop, UV_RUN_DEFAULT);
}
