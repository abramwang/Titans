#include <glog/logging.h>
#include <unistd.h>
#include "computing_center.h"

#define LOG_ENABLE True


int main(int argc, char* argv[])
{
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    ComputingCenter* cb = new ComputingCenter("./config.ini");

    while (1)
    {
        sleep(10000);
    }
    return 0;
}
