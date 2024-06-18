#ifndef __IA_ETF_JAGER_H__
#define __IA_ETF_JAGER_H__
#include <string>
#include <list>
#include <mutex>
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

class IaEtfJager
    : public TiQuoteCallback
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        std::string szSignalMap;

        std::string szSqlIp;
        int         nSqlPort;
        std::string szSqlUser;
        std::string szSqlPassword;

        std::string szInfluxUrl;
        std::string szInfluxToken;
        std::string szInfluxOrg;
        std::string szInfluxBucket;
        std::string szInfluxMeasurement;

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

    virtual void OnTimer();

public:
	IaEtfJager(uv_loop_s* loop, std::string configPath, bool only_use_fitted_pcf);
	virtual ~IaEtfJager();

    static void onTimer(uv_timer_t* handle);

private:
    uv_loop_t* m_loop;
    uv_timer_t m_timer;
    RedisSyncHandle* m_redis;
    TiQuoteIpcClient* m_quote_client;
    ConfigInfo* m_config;
    bool m_enable_only_use_fitted_pcf;

    IaEtfUserSetting* m_user_setting;
    IaEtfInfoMysql* m_mysql;
    IaEtfQuoteDataCache* m_quote_cache;
    IaEtfSignalCenter* m_signal_center;
    IaEtfFactorToInflux* m_influxdb_client;
    
    json m_json_cash;
    json m_json_msg;

private:
    int loadConfig(std::string iniFileName);
    void resetStreamKey();

public:
    std::mutex m_mutex;
};

#endif