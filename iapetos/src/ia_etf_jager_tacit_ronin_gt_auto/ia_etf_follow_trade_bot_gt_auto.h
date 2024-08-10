#ifndef __IA_ETF_FOLLOW_TRADE_BOT_GT_AUTO_H__
#define __IA_ETF_FOLLOW_TRADE_BOT_GT_AUTO_H__
#include <string>
#include <list>
#include <mutex>
#include "ti_trader_callback.h"
#include "ti_gt_trader_client.h"
#include "redis_commander.h"
#include "redis_sync_handle.h"

#include "ti_quote_callback.h"
#include "ti_quote_ipc_client.h"

#include "ia_etf_user_setting.h"
#include "ia_etf_info_mysql.h"
#include "ia_etf_quote_data_cache.h"
#include "ia_etf_signal_center.h"
#include "ia_etf_trade_worker_center.h"
#include "ia_etf_factor_to_influx.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaEtfFollowTradeBotGtAuto
    : public RedisCommander, public TiTraderCallback, public TiQuoteCallback
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

        std::string szPositionKey;
        std::string szOrderKey;
        std::string szMatchKey;

        std::string szAccountKey;

        std::string szSignalMap;

        std::string szSqlIp;
        int         nSqlPort;
        std::string szSqlUser;
        std::string szSqlPassword;

        std::string szInfluxUrl;
        std::string szInfluxToken;
        std::string szInfluxOrg;
        std::string szInfluxBucket;

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

/*   行情回调   */
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};
/*   交易回调   */
public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str);     //非交易逻辑的统一实现接口

    virtual void OnRspAccountInfo(const TiRspAccountInfo* pData);

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
	IaEtfFollowTradeBotGtAuto(uv_loop_s* loop, std::string configPath);
	virtual ~IaEtfFollowTradeBotGtAuto();


    static void onTimer(uv_timer_t* handle);

private:
    uv_timer_t m_timer;
    RedisSyncHandle* m_redis;
    TiQuoteIpcClient* m_quote_client;
    TiGtTraderClient* m_trade_client;
    ConfigInfo* m_config;


    IaEtfUserSetting* m_user_setting;
    IaEtfInfoMysql* m_mysql;
    IaEtfQuoteDataCache* m_quote_cache;
    IaEtfSignalCenter* m_signal_center;
    IaEtfTradeWorkerCenter* m_trade_center;
    IaEtfFactorToInflux* m_influxdb_client;
    
    int64_t m_query_time;
    int64_t m_query_interval;

    json m_json_cash;
    json m_json_msg;

private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();
    void enterOrder(json &msg);
    void enterOrders(json &msg);
    void cancelOrder(json &msg);

public:
    std::mutex m_mutex;

    TiTraderClient* GetTraderClient();
};

#endif