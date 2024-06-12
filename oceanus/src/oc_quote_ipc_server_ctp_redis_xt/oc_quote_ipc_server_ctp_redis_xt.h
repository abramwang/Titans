#ifndef __OC_QUOTE_IPC_SERVER_CTP_REDIS_XT_H__
#define __OC_QUOTE_IPC_SERVER_CTP_REDIS_XT_H__
#include <string>
#include <list>
#include <mutex>
#include "redis_commander.h"
#include "redis_sync_handle.h"
#include <nlohmann/json.hpp>
#include "oc_quote_info_mysql.h"
#include "oc_quote_cache.h"

#include "ti_ctp_quote_client.h"
#include "ti_quote_cache.h"
#include "ti_quote_callback.h"
#include "ti_ipc_api.h"
#include "datetime.h"

using namespace nlohmann;
using namespace std;

class OcQuoteIpcServerCtpRedisXt
    : public RedisCommander, public TiQuoteCallback
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

        std::string szSqlIp;
        int         nSqlPort;
        std::string szSqlUser;
        std::string szSqlPassword;
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
    virtual void OnTimer();

    virtual void OnCommandRtn(const char* type, const char* command);
private:
    virtual void onConnect(int status){};
    virtual void onDisconnect(int status){};
    virtual void onAuth(int err, const char* errStr);
    virtual void onCommand(int err, std::vector<std::string> *rsp);

    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg);
public:
	OcQuoteIpcServerCtpRedisXt(uv_loop_s* loop, std::string configPath);
	virtual ~OcQuoteIpcServerCtpRedisXt();


    static void onTimer(uv_timer_t* handle);


    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){};
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};
private:
    RedisSyncHandle m_redis;
    OcQuoteInfoMysql* m_quote_info_mysql_client;

    uv_timer_t m_timer;
    ConfigInfo* m_config;
    
    std::vector<OCInstrumentInfo> m_instrument_info_list;
    OcQuoteCache m_quote_cache;

private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();

    
    std::mutex m_mutex;

};

#endif