#ifndef __QUOTE_COMMANDER_H__
#define __QUOTE_COMMANDER_H__

#include "redis_commander.h"
#include "ees_quote_client.h"

using namespace std;

class QuoteCommander
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
public:
	QuoteCommander(uv_loop_s* loop, std::string configPath);
	virtual ~QuoteCommander();

private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
    EESQuoteClient* client;
    ConfigInfo* config;

    int loadConfig(std::string iniFileName);

};


#endif