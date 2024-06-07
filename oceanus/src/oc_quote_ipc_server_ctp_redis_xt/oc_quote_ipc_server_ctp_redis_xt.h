#ifndef __OC_QUOTE_IPC_SERVER_CTP_REDIS_XT_H__
#define __OC_QUOTE_IPC_SERVER_CTP_REDIS_XT_H__
#include <string>
#include <list>
#include <mutex>
#include "redis_commander.h"
#include "redis_sync_handle.h"
#include <nlohmann/json.hpp>

using namespace nlohmann;

using namespace std;

class OcQuoteIpcServerCtpRedisXt
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

private:
    RedisSyncHandle m_redis;
    uv_timer_t m_timer;
    ConfigInfo* m_config;
    json m_json_cash;
    json m_json_msg;

private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();

    
    std::mutex m_mutex;

};

#endif