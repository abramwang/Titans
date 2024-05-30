#ifndef __IA_ETF_TRADING_WORKER_H__
#define __IA_ETF_TRADING_WORKER_H__

#include <memory>

#include "ti_trader_callback.h"
#include "ti_trader_client.h"
#include "ti_trader_client.h"
#include "ia_etf_quote_data_cache.h"
#include "ia_etf_signal_factor.h"
#include "ia_etf_user_setting.h"

class IaETFTradingWorker
    : public TiTraderCallback
{
public:
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
    void getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec);
public:
    virtual void OnTimer() = 0;
public:
    std::string getAccount(){return m_account;};
    void setPreWorker(std::shared_ptr<IaETFTradingWorker> pre_worker){m_pre_worker = pre_worker;};
    bool isReady(){return m_pre_worker == nullptr || m_pre_worker->isOver();};
    virtual int64_t open() = 0;
    virtual bool isOver() = 0;
public:
    IaETFTradingWorker(TiTraderClient* client, IaEtfQuoteDataCache* quote_cache,IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFTradingWorker(){};
};

typedef std::shared_ptr<IaETFTradingWorker> IaETFTradingWorkerPtr;

#endif