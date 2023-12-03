#ifndef __TRADE_COMMANDER_H__
#define __TRADE_COMMANDER_H__
#include <string>
#include "ees_trader_client.h"
#include "redis_commander.h"

using namespace std;

class TradeCommander
    : public RedisCommander 
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        int nBlock;
        std::string szStreamKey;
        std::string szStreamGroup;
        std::string szConsummerId;
    } ConfigInfo;

private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
	TradeCommander(uv_loop_s* loop, std::string configPath);
	virtual ~TradeCommander();

private:
    EESTraderClient* client;
    ConfigInfo* config;

    int loadConfig(std::string iniFileName);

};


#endif