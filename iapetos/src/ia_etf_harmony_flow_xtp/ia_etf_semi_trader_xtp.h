#ifndef __IA_ETF_SEMI_TRADER_XTP_H__
#define __IA_ETF_SEMI_TRADER_XTP_H__
#include <string>
#include <list>
#include <mutex>
#include <atomic>
#include "ti_trader_callback.h"
#include "ti_zt_trader_client.h"
#include "redis_commander.h"
#include "redis_sync_handle.h"
#include "ia_etf_trading_worker.h"
#include "ia_command_interface.h"
#include <nlohmann/json.hpp>
using namespace nlohmann;

using namespace std;

class IaETFSemiTraderXtp
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

        std::string szWorkerSignalStreamKey;
        std::string szWorkerStatusKey;
        
        std::string szTradingStreamKey;
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

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast);
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast);
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast);

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData);

    virtual void OnTimer();
private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);
    virtual void onCommand(int err, std::vector<std::string> *rsp);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
	IaETFSemiTraderXtp(uv_loop_s* loop, std::string configPath, IaCommandInterface* commander);
	virtual ~IaETFSemiTraderXtp();


    static void onTimer(uv_timer_t* handle);

private:
    RedisSyncHandle m_redis;
    uv_timer_t m_timer;
    TiZtTraderClient* m_client;
    ConfigInfo* m_config;
    IaCommandInterface* m_commander;
    json m_json_cash;
    json m_json_msg;


    std::list<std::shared_ptr<IaETFTradingWorker>> m_workerList;

private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();
public:
    double m_total_asset;   //总资产
    double m_cash_asset;    //可用资金
    int64_t m_query_time;   //查询时间
    int64_t m_heartbeat_time;    //心跳时间
    std::atomic_int32_t m_frozen_cash;
    std::atomic_int32_t m_signal_count;
    std::mutex m_mutex;

    double GetAvailableCash();
    TiTraderClient* GetTraderClient();
    void AddWorker(std::shared_ptr<IaETFTradingWorker> worker_ptr);
    void PublicWorkerSignal(json &signal);
    void UpdateWorkerStatus(std::string id, json &status);
};

#endif