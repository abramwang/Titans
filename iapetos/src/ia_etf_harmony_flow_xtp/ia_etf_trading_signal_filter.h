#ifndef __IA_ETF_TRADING_SIGNAL_FILTER_H__
#define __IA_ETF_TRADING_SIGNAL_FILTER_H__

#include "ia_etf_factor_operator.h"
#include "ia_etf_price_tool.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

namespace IaETFTradingSignalFilter
{
    ///////////////////////// 对手盘快速套利逻辑 /////////////////////////

    bool isPurchasingSignal(const IaETFFactorOperator* pOperator_, json &out);
    bool isRedeemingSignal(const IaETFFactorOperator* pOperator_, json &out);

    //申购部分
    //计算买入股票成本和利润
    void calc_purchasing_profit(const IaETFFactorOperator* pOperator_, json &out);
    
    //赎回部分
    //计算赎回股票成本和利润
    void calc_redeeming_profit(const IaETFFactorOperator* pOperator_, json &out);

    ///////////////////////// 挂单限价套利逻辑 /////////////////////////

    //计算限价赎回股票成本和利润
    void calc_limit_redeeming_profit(const IaETFFactorOperator* pOperator_, json &out);

    //计算限价赎回
    double get_limit_redeeming_price(const IaETFFactorOperator* pOperator_);
    bool start_limit_redeeming(const IaETFFactorOperator* pOperator_, double price);
    bool allow_limit_redeeming(const IaETFFactorOperator* pOperator_, double price);



};

#endif