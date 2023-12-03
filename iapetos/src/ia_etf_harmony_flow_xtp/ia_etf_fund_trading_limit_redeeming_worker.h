#ifndef __IA_ETF_FUND_TRADING_LIMIT_REDEEMING_WORKER_H__
#define __IA_ETF_FUND_TRADING_LIMIT_REDEEMING_WORKER_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_factor_operator.h"
#include "ia_etf_trading_signal_filter.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

class IaETFFundTradingLimitRedeemingWorker :
    public IaETFTradingWorker
{

public:
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData);

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    
    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnTimer();

private:
    IaETFFactorOperator* m_factor_operator;
    TiQuoteSnapshotStockField m_signal_snap;
    TiQuoteOrderBookField m_order_book;
    double m_etf_init_price;    //etf 初始信号价格

    TiRtnOrderStatus* m_redeeming_order;
    bool m_is_redeeming;
    bool m_is_over;
    int32_t m_redemption_failures;
    double m_expected_redeeming_profit;
    int64_t m_start_buy_time;
    int64_t m_check_time;   //检查时间
    int32_t m_queue_order_seq;  // 排队时前面一位订单编号
    bool m_is_wait_model;

    //etf 交易列表
    std::vector<TiRtnOrderStatus*> m_order_vec;                     //order
    std::unordered_map<int64_t, TiRtnOrderStatus*> m_order_req_map; //reqid order
    std::unordered_map<int64_t, TiRtnOrderStatus*> m_order_map;     //order_id order
    std::unordered_set<int64_t> m_cancel_order_set;     //order_id
    
    void getSplitOrderVol(int32_t order_vol, std::vector<int32_t>& order_vol_vec);
    bool isEtfBuyOver();
    double calculateTheoreticalProfit(double order_price);
    int32_t getEtfBuyVol();         //获取还需要买多少量
    int32_t getEtfDealtVol();       //获取已经买了多少量
    double getEtfDealtPrice();      //获取成交均价
    double getEtfQueuePrice();      //获取排队订单均价
    bool hasQueueOrder();           
    void cancelEtfOrder();

    int32_t getQueueOrderSeq();
    int32_t getQueueOrderVol();

    void processMatchEtfOrder();    //主动吃单
    void processPendingEtfOrder();  //被动挂单
    void redeemingEtf();

    void periodicProfitCheck();
public:
    void WaitForEtfOrderExecution();
    bool IsOver();
    bool IsCancel();
    json GetOrderArrayJson();
    int32_t GetRedemptionFailures();
public:
    IaETFFundTradingLimitRedeemingWorker(TiTraderClient* client, IaETFFactorOperator* factor_operator);
    ~IaETFFundTradingLimitRedeemingWorker(){};
};


#endif