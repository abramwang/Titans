#ifndef __IA_ETF_LIMIT_REDEEMING_WORKER_H__
#define __IA_ETF_LIMIT_REDEEMING_WORKER_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_factor_operator.h"
#include "ia_etf_fund_trading_limit_redeeming_worker.h"
#include "ia_etf_single_stock_trading_worker.h"
#include "ia_etf_semi_trader_xtp.h"
#include "ia_etf_trading_signal_filter.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;


class IaETFLimitRedeemingWorker
    : public IaETFTradingWorker
{
public:
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData);

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnTimer();
public:
    IaETFLimitRedeemingWorker(int64_t id, IaETFFactorOperator* factor, IaETFSemiTraderXtp* trader);
    virtual ~IaETFLimitRedeemingWorker(){};
private:
    IaETFFactorOperator* m_factor_operator;
    IaETFFundTradingLimitRedeemingWorker* m_fund_worker;
    std::unordered_map<std::string, IaETFSingleStockTradingWorkerPtr> m_stock_workers_map;
    std::unordered_map<std::string, IaETFSingleStockTradingWorkerPtr> m_finished_stock_workers_map;
    IaETFSemiTraderXtp* m_trader;

    int64_t m_id;
    bool m_start_stock_trading;
    bool m_is_over;

private:
    void init_worker_task(int64_t id);  // 初始化任务
    void init_fund_work();              // 初始化基金交易任务
    void init_stock_work();             // 初始化股票交易任务
    void update_worker_status();        // 更新任务状态

    void public_worker_status();        // 公布任务状态

public:
    void start();
    bool is_over();
    int32_t get_redemption_failures();
};

#endif