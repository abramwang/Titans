#ifndef __IA_ETF_TRADING_SIGNAL_FILTER_H__
#define __IA_ETF_TRADING_SIGNAL_FILTER_H__

#include "ia_etf_factor_operator.h"
#include "ia_etf_price_tool.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

namespace IaETFTradingSignalFilter
{

    bool isPurchasingSignal(const IaETFFactorOperator* pOperator_, json &out);
    bool isRedeemingSignal(const IaETFFactorOperator* pOperator_, json &out);

    //申购部分
    //计算买入股票成本和利润
    void calc_purchasing_profit(const IaETFFactorOperator* pOperator_, json &out);
    
    //赎回部分
    //计算赎回股票成本和利润
    void calc_redeeming_profit(const IaETFFactorOperator* pOperator_, json &out);
    
};

#endif