#ifndef IA_ETF_FACTOR_CENTER_H
#define IA_ETF_FACTOR_CENTER_H
#include <string>
#include <list>
#include <uv.h>

#include "ti_quote_ipc_client.h"
#include "ti_quote_depth_callback.h"
#include "ti_quote_callback.h"
#include "ti_factor_callback.h"
#include "redis_sync_handle.h"
#include "ia_etf_fund_info.h"
#include "ia_etf_factor_operator.h"
#include "ti_quote_history_client_local.h"

class IaETFFactorCenter
    :  public TiFactorCallback, public TiQuoteCallback, public TiQuoteDepthCallback
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        std::string szConfigETFListKey;
        std::string szConfigETFRealityInfoKey;
        std::string szConfigETFDisclosureInfoKey;
        std::string szStreamKey;

        std::string szUiFactorKey;
        std::string szUiCostKey;

        std::string szIpcQuoteTopic;

        bool        bEnableHistory;     // 是否启用历史数据
        int         nHistoryDateNum;    // 历史数据日期
    } ConfigInfo;

protected:
    ConfigInfo*                     m_config;                    //
    IaETFFundInfo*                  m_fund_info;                 //
    std::atomic<long long>          m_timestamp;                 //  etf 最新价，通过成交更新
    TiQuoteIpcClient*               m_quoteClient;               //
    TiQuoteHistoryClientLocal*      m_quoteHistoryClient;        //
    std::list<IaETFFactorOperator*> m_operatorList;              //
    
    int64_t m_cout_snap_time;
protected:
    bool                            m_output_factors_to_redis;   
    RedisSyncHandle                 m_redis;                     //                 //

    int loadConfig(std::string iniFileName);
    void init_operators(int thread_num);
    void subStock(const char* exchangeName);

public: //行情回调
    void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};

    void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};
    
    void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids){};
    
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData){};
    virtual void OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data);

public: 
    IaETFFactorCenter(int thread_num, uv_loop_t *loop, std::string configPath);
    virtual ~IaETFFactorCenter(){};
};

#endif