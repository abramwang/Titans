#include "redis_uv.h"
#include "log.h"
#include "quote_commander.h"

#define LOG_ENABLE True

int main() {
    log_init(LL_DEBUG, "rem", "./");
	
    uv_loop_t* loop = uv_default_loop();

	QuoteCommander * commander = new QuoteCommander(loop, "./config.ini");

	return commander->listen();
}
