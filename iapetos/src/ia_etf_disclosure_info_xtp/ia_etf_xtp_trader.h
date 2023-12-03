#ifndef __IA_ETF_XTP_TRADER_H__
#define __IA_ETF_XTP_TRADER_H__
#include <string>
#include <list>
#include <map>
#include "ti_trader_callback.h"
#include "ti_zt_trader_client.h"
#include "redis_commander.h"
#include "redis_sync_handle.h"

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
        
        std::string szConfigETFDisclosureInfoKey;
    } ConfigInfo;

    static void init_redis_command_work(uv_work_t* req);
    static void after_redis_command_work(uv_work_t* req, int status);
public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str);     //非交易逻辑的统一实现接口

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
    virtual void onCommand(int err, std::vector<std::string> *rsp);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
	IaETFXtpTrader(uv_loop_s* loop, std::string configPath);
	virtual ~IaETFXtpTrader();


private:
    RedisSyncHandle m_redis;
    TiZtTraderClient* m_client;
    ConfigInfo* m_config;

    std::map<std::string, json> m_etf_info_map;
    std::map<std::string, std::string> m_etf_basket_task;   // symbol, exchange

    int loadConfig(std::string iniFileName);

};


#endif