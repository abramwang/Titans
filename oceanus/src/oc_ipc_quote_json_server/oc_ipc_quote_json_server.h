#ifndef __OC_IPC_QUOTE_JSON_SERVER_H__
#define __OC_IPC_QUOTE_JSON_SERVER_H__
#include <string>
#include <list>
#include <mutex>
#include "redis_commander.h"
#include "redis_sync_handle.h"

#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_quote_ipc_client.h"
#include "ti_quote_tools.h"

#include <nlohmann/json.hpp>

using namespace nlohmann;

using namespace std;

class OcIpcQuoteJsonServer
    : public RedisCommander, public TiQuoteCallback, public TiQuoteDepthCallback
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        std::string szQuoteTopic;

        std::string szQuoteStreamKey;
        std::string szQuoteQueryKey;

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
    virtual void OnCommandRtn(const char* type, const char* command);
    virtual void OnTimer();

    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName);
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData);

    virtual void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids){};
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData){};

private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);
    virtual void onCommand(int err, std::vector<std::string> *rsp){};

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){};
public:
	OcIpcQuoteJsonServer(uv_loop_s* loop, std::string configPath);
	virtual ~OcIpcQuoteJsonServer();


    static void onTimer(uv_timer_t* handle);
private:
    RedisSyncHandle m_redis;
    uv_timer_t m_timer;
    ConfigInfo* m_config;
    json m_json_cash;
    int64_t m_cout_time_snap;

    TiQuoteIpcClient* m_quote_client;

private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();

public:
    std::mutex m_mutex;
};

#endif