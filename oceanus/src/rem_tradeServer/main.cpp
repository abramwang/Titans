#include <string>
#include "trade_commander.h"
#include "log.h"
#include "iniFile.h"

#define LOG_ENABLE True

int main() {
    log_init(LL_DEBUG, "rem", "./");
    uv_loop_t* loop = uv_default_loop();

	TradeCommander * commander = new TradeCommander(loop, "./config.ini");
	
	return commander->listen();
}
