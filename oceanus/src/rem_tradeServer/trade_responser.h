#ifndef __TRADE_RESPONSER_H__
#define __TRADE_RESPONSER_H__

#include <string>
#include "cJSON.h"
#include "redis_sync_handle.h"

class TradeResponser
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

	TradeResponser(std::string configPath);
	virtual ~TradeResponser();

private:
    ConfigInfo* m_config;
    cJSON* msgContainer;

    int loadConfig(std::string iniFileName);

public:
    void response(const char* type, cJSON *msg);

};


#endif