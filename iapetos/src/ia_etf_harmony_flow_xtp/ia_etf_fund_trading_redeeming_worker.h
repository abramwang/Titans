#ifndef __IA_ETF_FUND_TRADING_REDEEMING_WORKER_H__
#define __IA_ETF_FUND_TRADING_REDEEMING_WORKER_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_factor_operator.h"
#include <unordered_map>
#include <vector>

class IaETFFundTradingRedeemingWorker :
    public IaETFTradingWorker
{
public:
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnTimer();

private:
    IaETFFactorOperator* m_factor_operator;
    TiQuoteSnapshotStockField m_signal_snap;

    TiRtnOrderStatus* m_redeeming_order;
    bool m_is_over;
    int64_t m_start_buy_time;
    int64_t m_check_time;   //检查时间
    json m_fund_trade_status;
    json m_etf_buy_status;

    //etf 交易列表
    std::vector<TiRtnOrderStatus*> m_order_vec;                     //order
    std::unordered_map<int64_t, TiRtnOrderStatus*> m_order_req_map; //reqid order
    std::unordered_map<int64_t, TiRtnOrderStatus*> m_order_map;     //order_id order
    
    void getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec);
    bool isEtfBuyOver();
    int32_t getEtfBuyVol();
    bool hasQueueOrder();
    void cancelEtfOrder();

    void redeemingEtf();
public:
    void BuyEtf();
    bool IsOver();
    json GetOrderArrayJson();
public:
    IaETFFundTradingRedeemingWorker(TiTraderClient* client, IaETFFactorOperator* factor_operator);
    ~IaETFFundTradingRedeemingWorker(){};
};


#endif