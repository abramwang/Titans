#ifndef __XTP_TRADE_COMMANDER_H__
#define __XTP_TRADE_COMMANDER_H__
#include <string>
#include "ti_trader_callback.h"
#include "ti_zt_trader_client.h"
#include "redis_commander.h"

using namespace std;

class IaETFXtpTrader
    : public RedisCommander, public TiTraderCallback
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
    virtual void OnRspOrderInsert(const TiRspOrderInsert* pData){};
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){};
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
	IaETFXtpTrader(uv_loop_s* loop, std::string configPath);
	virtual ~IaETFXtpTrader();

private:
    TiZtTraderClient* client;
    ConfigInfo* config;

    int loadConfig(std::string iniFileName);

};


#endif