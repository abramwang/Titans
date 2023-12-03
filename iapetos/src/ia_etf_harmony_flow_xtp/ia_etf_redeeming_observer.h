#ifndef __IA_ETF_REDEEMING_OBSERVER_H__
#define __IA_ETF_REDEEMING_OBSERVER_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_factor_operator.h"
#include "ia_etf_limit_redeeming_worker.h"
#include "ia_etf_semi_trader_xtp.h"

#include <vector>

#include <nlohmann/json.hpp>
using namespace nlohmann;


class IaETFRedeemingObserver
    : public IaETFTradingWorker
{
public:
    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){};
    
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData);

    virtual void OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data);

    virtual void OnTimer(){};
public:
    IaETFRedeemingObserver(int64_t id, IaETFFactorOperator* factor, IaETFSemiTraderXtp* trader);
    virtual ~IaETFRedeemingObserver(){};
private:
    IaETFFactorOperator* m_factor_operator;
    IaETFSemiTraderXtp* m_semi_trader;

    IaETFLimitRedeemingWorker* m_worker;
    double m_reservedCapital;
    int32_t m_redemption_failures;

    std::vector<IaETFLimitRedeemingWorker*> m_ex_worker_vec;
};

#endif