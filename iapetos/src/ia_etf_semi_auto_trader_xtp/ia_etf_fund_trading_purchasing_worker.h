#ifndef __IA_ETF_FUND_TRADING_PURCHASING_WORKER_H__
#define __IA_ETF_FUND_TRADING_PURCHASING_WORKER_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_factor_operator.h"
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaETFFundTradingPurchasingWorker :
    public IaETFTradingWorker
{
public:
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnTimer();

private:
    IaETFFactorOperator* m_factor_operator;
    TiQuoteSnapshotStockField m_signal_snap;

    TiRtnOrderStatus* m_purchasing_order;
    bool m_is_over;
    int64_t m_start_sell_time;
    json m_fund_trade_status;
    json m_etf_sell_status;

    //etf 交易列表
    std::vector<TiRtnOrderStatus*> m_order_vec;                     //order
    std::unordered_map<int64_t, TiRtnOrderStatus*> m_order_req_map; //reqid order
    std::unordered_map<int64_t, TiRtnOrderStatus*> m_order_map;     //order_id order
    
    void getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec);
    bool isEtfSellOver();
    int32_t getEtfSellVol();
    bool hasQueueOrder();
    void cancelEtfOrder();

    void sellEtf();
    void sellEtfClose();
public:
    void PurchasingEtf();
public:
    IaETFFundTradingPurchasingWorker(TiTraderClient* client, IaETFFactorOperator* factor_operator, TiQuoteSnapshotStockField *signal_snap);
    ~IaETFFundTradingPurchasingWorker(){};
};


#endif