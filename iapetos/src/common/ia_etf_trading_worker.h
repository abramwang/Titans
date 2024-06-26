#ifndef __IA_ETF_TRADING_WORKER_H__
#define __IA_ETF_TRADING_WORKER_H__

#include <memory>

#include "ti_trader_callback.h"
#include "ti_trader_client.h"
#include "ia_etf_quote_data_cache.h"
#include "ia_etf_signal_factor.h"
#include "ia_etf_user_setting.h"

class IaETFTradingWorker
    : public TiTraderCallback
{
public:
    struct Status
    {
        std::string symbol;
        std::string exchange;
        double expect_cost;
        int32_t volume;
        double real_cost;
        int32_t finish_volume;
        int32_t wrong_number;
        bool is_limit;  //是否是涨跌停价格
    };

    struct DeleteOrderReqInfo
    {
        int64_t nOrderId;
        char    szOrderStreamId[64];
        char    szAccount[64];
    };
protected:
    TiTraderClient* m_client;
    IaEtfQuoteDataCache* m_quote_cache;
    IaEtfSignalFactorPtr m_etf_factor;
    std::string m_account;

    std::shared_ptr<IaETFTradingWorker> m_pre_worker;
protected:
    int64_t m_check_time;   //检查时间
    Status m_status;
    TiQuoteSnapshotStockField m_open_snap;
    DeleteOrderReqInfo m_canceling_order_info;                      // canceling order
    std::set<int64_t> m_req_id_set;                                 // order_req_id
    std::unordered_map<int64_t, TiRtnOrderStatus> m_order_map;      // nOrderId -> order_status
protected:
    void getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec);
    virtual void updateExpectCost(TiQuoteSnapshotStockField* pData);
    virtual void updateStatus();
    virtual bool hasQueueOrder();
    virtual void cancelOrder(TiRtnOrderStatus* order);
public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){};     //非交易逻辑的统一实现接口
    
    virtual void OnRspAccountInfo(const TiRspAccountInfo* pData){};

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};
    
    virtual void OnTimer();
public:
    std::string getAccount(){return m_account;};
    void setPreWorker(std::shared_ptr<IaETFTradingWorker> pre_worker){m_pre_worker = pre_worker;};
    bool isReady(){return m_pre_worker == nullptr || m_pre_worker->isOver();};
    virtual int64_t open() = 0;
    virtual bool isOver();
public:
    IaETFTradingWorker(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache,IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFTradingWorker(){};
};

typedef std::shared_ptr<IaETFTradingWorker> IaETFTradingWorkerPtr;

#endif