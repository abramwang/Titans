#ifndef IA_ETF_FACTOR_CENTER_H
#define IA_ETF_FACTOR_CENTER_H
#include <string>
#include <list>
#include <uv.h>

#include "ti_zt_quote_client.h"
#include "ti_quote_callback.h"
#include "ti_factor_callback.h"
#include "redis_sync_handle.h"
#include "ia_etf_fund_info.h"
#include "ia_etf_factor_operator.h"
#include "ti_quote_history_client_local.h"
#include "ti_quote_depth_callback.h"
#include "ti_book_engine.h"

class IaETFFactorCenter
    : public TiQuoteCallback, public TiFactorCallback, public TiQuoteDepthCallback
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

        bool        bEnableHistory;     // 是否启用历史数据
        int         nHistoryDateNum;    // 历史数据日期
    } ConfigInfo;

    struct CalcWorkReqInfo
    { 
        TI_QUOTE_DATA_TYPE              type;             //
        TiQuoteSnapshotStockField*      m_snapshot;       //
        TiQuoteMatchesField*            m_match;          //
        std::list<IaETFFactorOperator*> m_operator_list;  //
    };
   
    static void calc_work(uv_work_t* req);
private:
    uv_loop_t                       m_loop;               //计算 event loop
    ConfigInfo*                     m_config;                    //
    RedisSyncHandle                 m_redis;                     //
    IaETFFundInfo*                  m_fund_info;                 //
    std::atomic<long long>          m_timestamp;            //  etf 最新价，通过成交更新
    TiZtQuoteClient*                m_quoteClient;               //
    TiQuoteHistoryClientLocal*      m_quoteHistoryClient;        //
    std::list<IaETFFactorOperator*> m_operatorList;              //
    std::list<uv_work_t*>           m_calcWorkReqList;    //  CalcWorkReqInfo
    TiBookEngine*                   m_book_engine;
    TiQuoteSnapshotStockField       m_book_snap_cash;
protected:
    bool                            m_output_factors_to_redis;                    //

    int loadConfig(std::string iniFileName);
    void init_operators(int thread_num);
    void subStock(const char* exchangeName);

public: //行情回调
    void OnTradingDayRtn(const unsigned int day, const char* exchangeName);


    void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    void OnL2StockOrderRtn(const TiQuoteOrderField* pData);

    void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids);
    
    void OnDepthSnapshotRtn(const TiQuoteSnapshotStockField* pData);

    virtual void OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data){};

public: 
    IaETFFactorCenter(int thread_num, std::string configPath);
    virtual ~IaETFFactorCenter(){};
};

#endif