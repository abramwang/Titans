#ifndef __OC_TRADER_COMMANDER_ANXIN_H__
#define __OC_TRADER_COMMANDER_ANXIN_H__
#include <string>
#include <list>
#include <mutex>
#include "ti_trader_callback.h"
#include "ti_gf_trader_client.h"
#include "redis_commander.h"
#include "redis_sync_handle.h"
#include <nlohmann/json.hpp>

using namespace nlohmann;

using namespace std;

class OcTraderCommanderGuangfa
    : public RedisCommander, public TiTraderCallback
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        int nBlock;
        std::string szCommandStreamKey;
        std::string szCommandStreamGroup;
        std::string szCommandConsumerId;

        std::string szOrderKey;
        std::string szMatchKey;

    } ConfigInfo;

    class Locker
    {
    private:
        std::mutex* m_mutex;
    public:
        Locker(std::mutex* mutex)
        {
            m_mutex = mutex;
            m_mutex->lock();
        };
        ~Locker()
        {
            m_mutex->unlock();
        };
    };

public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str);     //非交易逻辑的统一实现接口

    virtual void OnRspAccountInfo(const TiRspAccountInfo* pData){};

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast);
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast);
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast);

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData);

    virtual void OnTimer();

    virtual void OnCommandRtn(const char* type, const char* command);
private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);
    virtual void onCommand(int err, std::vector<std::string> *rsp);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
	OcTraderCommanderGuangfa(uv_loop_s* loop, std::string configPath);
	virtual ~OcTraderCommanderGuangfa();


    static void onTimer(uv_timer_t* handle);

private:
    RedisSyncHandle m_redis;
    uv_timer_t m_timer;
    TiGfTraderClient* m_client;
    ConfigInfo* m_config;
    json m_json_cash;
    json m_json_msg;

private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();
    void enterOrder(json &msg);

public:
    double m_total_asset;   //总资产
    double m_cash_asset;    //可用资金
    std::mutex m_mutex;

    TiTraderClient* GetTraderClient();
};

#endif