#ifndef __OC_QUOTE_IPC_SERVER_CTP_REDIS_XT_H__
#define __OC_QUOTE_IPC_SERVER_CTP_REDIS_XT_H__
#include <string>
#include <list>
#include "redis_commander.h"
#include "redis_sync_handle.h"
#include <nlohmann/json.hpp>

#include "ti_ctp_quote_client.h"
#include "oc_quote_ipc_server.h"

using namespace nlohmann;
using namespace std;

class OcQuoteIpcServerCtpRedisRq
    : public RedisCommander
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        int nBlock;
        std::string szQuoteStreamKey;
        std::string szQuoteStreamGroup;
        std::string szQuoteConsumerId;

        std::string szQuoteIpcTopic;
    } ConfigInfo;

public:
    virtual void OnTimer();

    virtual void OnCommandRtn(const char* type, const char* command);
private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);
    virtual void onCommand(int err, std::vector<std::string> *rsp);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
	OcQuoteIpcServerCtpRedisRq(uv_loop_s* loop, std::string configPath);
	virtual ~OcQuoteIpcServerCtpRedisRq();


    static void onTimer(uv_timer_t* handle);


    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
private:
    RedisSyncHandle m_redis;

    uv_timer_t m_timer;
    ConfigInfo* m_config;
    
    TiCtpQuoteClient* m_ctp_client;
    OcQuoteIpcServer m_ipc_server;
private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();

    

};

#endif