#ifndef __QUOTE_RESPONSER_H__
#define __QUOTE_RESPONSER_H__

#include <string>
#include "cJSON.h"
#include "redis_sync_handle.h"

class QuoteResponser
    : public RedisSyncHandle 
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        std::string szStreamKey;
    } ConfigInfo;

	QuoteResponser(std::string configPath);
	virtual ~QuoteResponser();

private:
    ConfigInfo* config;
    cJSON* msgContainer;

    int loadConfig(std::string iniFileName);

public:
    void updateSymbolInfo(cJSON* info);
    void updateSnapshot(const char* ex, std::string symbol, cJSON *data);

};


#endif