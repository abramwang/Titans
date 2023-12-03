#include "ia_etf_trading_signal_filter.h"
#include "datetime.h"
#include <iostream>

bool IaETFTradingSignalFilter::isPurchasingSignal(const IaETFFactorOperator* pOperator_, json &out)
{
    if(pOperator_->nPurchasingCost >  200)
    {
        calc_purchasing_profit(pOperator_, out);
        if (out["total_high_limit_turnover"] != 0)  //过滤涨跌停
        {
            return false;
        }
        if (out["diff"] == 0)  //过滤没有真实清单
        {
            return false;
        }
        if (out["purchasing_profit_ratio"] < 0.0005)
        {
            return false;
        }
        //std::cout << out.dump() << std::endl;
        return true;
    }
    return false;
}

bool IaETFTradingSignalFilter::isRedeemingSignal(const IaETFFactorOperator* pOperator_, json &out)
{
    if(pOperator_->nRedeemingCost >  200)
    {
        calc_redeeming_profit(pOperator_, out);
        if (out["total_low_limit_turnover"] != 0)  //过滤涨跌停
        {
            return false;
        }
        if (out["diff"] == 0)  //过滤没有真实清单
        {
            return false;
        }
        if (out["redeeming_profit_ratio"] < 0.0005)
        {
            return false;
        }
        //std::cout << out.dump() << std::endl;
        return true;
    }
    return false;
}

void IaETFTradingSignalFilter::calc_purchasing_profit(const IaETFFactorOperator* pOperator_, json &out)
{
    out["symbol"] = pOperator_->m_etf_last_snap.symbol;
    out["exchange"] = pOperator_->m_etf_last_snap.exchange;
    out["time"] = pOperator_->m_etf_last_snap.time;
    out["time_str"] = datetime::get_format_timestamp_ms(pOperator_->m_etf_last_snap.timestamp);
    out["purchasing_profit"] = pOperator_->nPurchasingCost.load();
    out["purchasing_profit_ratio"] = 0;
    out["diff"] = pOperator_->nDiff.load();
    out["trade_stock_array"] = json::array();       //交易股票列表
    out["fund_trade_stock_array"] = json::array();  //公募交易股票列表
    out["replace_stock_array"] = json::array();     //现金替代股票列表
    out["stock_diff"] = json::array();              //差值

    double total_cost = 0;
    double total_occupation_fund = 0;
    double total_high_limit_turnover = 0;
    
    for (auto iter = pOperator_->m_cashMap.begin(); iter != pOperator_->m_cashMap.end(); ++iter)
    {
        out["stock_diff"].push_back({
            { "symbol", iter->second->szSymbol },
            { "exchange", iter->second->szExchange },
            { "diff" , iter->second->nDiffFactor.load() }
        });
        if(!iter->second->bReplace){    //股票交易
            if (!strcmp(pOperator_->m_etf_last_snap.exchange, iter->second->szExchange)) //同一市场自己交易
            {
                out["trade_stock_array"].push_back({
                    { "symbol", iter->second->szSymbol },
                    { "buy_price", iter->second->m_last_snap.ask_price[0] },
                    { "buy_volume", iter->second->m_info.m_disclosure_vol },
                    { "buy_turnover", iter->second->nAskIopvFactor.load() },
                    { "buy_fee" , iter->second->nBuyFee.load() },
                    { "high_limit_turnover", iter->second->nHighLimitValue.load()},
                    { "purchasing_cost" , iter->second->nBuyCost.load() }
                });
                total_cost += iter->second->nBuyCost.load();
                total_occupation_fund += iter->second->nBuyCost.load();
                total_high_limit_turnover += iter->second->nHighLimitValue;
            }else{                                                                      //不同市场公募交易
                out["fund_trade_stock_array"].push_back({
                    { "symbol", iter->second->szSymbol },
                    { "buy_price", iter->second->m_last_snap.ask_price[0] },
                    { "buy_volume", iter->second->m_info.m_disclosure_vol },
                    { "buy_turnover", iter->second->nAskIopvFactor.load() },
                    { "buy_fee" , iter->second->nBuyFee.load() },
                    { "high_limit_turnover", iter->second->nHighLimitValue.load()},
                    { "purchasing_cost" , iter->second->nBuyCost.load() },
                    { "occupation_fund" , iter->second->nBuyCost.load() * (1 + iter->second->m_info.m_creation_premium_ratio) },
                });
                total_cost += iter->second->nBuyCost.load();
                total_occupation_fund += iter->second->nBuyCost.load() * (1 + iter->second->m_info.m_creation_premium_ratio);
                total_high_limit_turnover += iter->second->nHighLimitValue;
            }
        }else{  //现金替代
            double replace_amount = iter->second->m_info.m_creation_amount ? iter->second->m_info.m_creation_amount : iter->second->m_info.m_replace_amount ;
            double occupation_fund = replace_amount * ( 1 + iter->second->m_info.m_creation_premium_ratio);
            out["replace_stock_array"].push_back({
                { "symbol", iter->second->szSymbol },
                { "replace_amount", replace_amount },
                { "premium_ratio", iter->second->m_info.m_creation_premium_ratio },
                { "occupation_fund", occupation_fund },     //资金占用
                { "purchasing_cost" , iter->second->nBuyCost.load() }
            });
            total_cost += iter->second->nBuyCost.load();
            total_occupation_fund += occupation_fund;
        }
    }

    out["total_cost"] = total_cost;
    out["total_occupation_fund"] = total_occupation_fund;
    out["total_high_limit_turnover"] = total_high_limit_turnover;

    out["purchasing_profit_ratio"] = pOperator_->nPurchasingCost.load() / total_occupation_fund;
    out["estimated_cash_difference"] = pOperator_->m_info->m_publicEstimatedCashDifference;     //预估线性差额
    
    // etf 交易信息
    double sell_price = IaEtfPriceTool::get_order_dealt_price(
        pOperator_->m_minUnit, pOperator_->m_etf_last_snap.bid_price, pOperator_->m_etf_last_snap.bid_volume, TI_STOCK_ARRAY_LEN);

    out["etf_trading_info"] = {
        { "bid_price_1", pOperator_->m_etf_last_snap.bid_price[0] },
        { "sell_price" , sell_price } ,
        { "sell_vol" , pOperator_->m_minUnit } ,
        { "sell_turnover", sell_price * pOperator_->m_minUnit },
    };
};

void IaETFTradingSignalFilter::calc_redeeming_profit(const IaETFFactorOperator* pOperator_, json &out)
{
    out["symbol"] = pOperator_->m_etf_last_snap.symbol;
    out["exchange"] = pOperator_->m_etf_last_snap.exchange;
    out["time"] = pOperator_->m_etf_last_snap.time;
    out["time_str"] = datetime::get_format_timestamp_ms(pOperator_->m_etf_last_snap.timestamp);
    out["redeeming_profit"] = pOperator_->nRedeemingCost.load();
    out["redeeming_profit_ratio"] = 0;
    out["diff"] = pOperator_->nDiff.load();
    out["trade_stock_array"] = json::array();       //交易股票列表
    out["fund_trade_stock_array"] = json::array();  //公募交易股票列表
    out["replace_stock_array"] = json::array();     //现金替代股票列表
    out["stock_diff"] = json::array();              //差值

    double total_cost = 0;
    double total_occupation_fund = 0;
    double total_low_limit_turnover = 0;
    
    for (auto iter = pOperator_->m_cashMap.begin(); iter != pOperator_->m_cashMap.end(); ++iter)
    {
        out["stock_diff"].push_back({
            { "symbol", iter->second->szSymbol },
            { "diff" , iter->second->nDiffFactor.load() }
        });
        if(!iter->second->bReplace){    //股票交易
            if (!strcmp(pOperator_->m_etf_last_snap.exchange, iter->second->szExchange)) //同一市场自己交易
            {
                out["trade_stock_array"].push_back({
                    { "symbol", iter->second->szSymbol },
                    { "sell_price", iter->second->m_last_snap.bid_price[0] },
                    { "sell_volume", iter->second->m_info.m_disclosure_vol },
                    { "sell_turnover", iter->second->nBidIopvFactor.load() },
                    { "sell_fee" , iter->second->nSellFee.load() },
                    { "low_limit_turnover", iter->second->nLowLimitValue.load()},
                    { "redeeming_cost" , iter->second->nSellCost.load() }
                });
                total_cost += iter->second->nBuyCost.load();
                total_occupation_fund += iter->second->nBuyCost.load();
                total_low_limit_turnover += iter->second->nLowLimitValue;
            }else{
                out["fund_trade_stock_array"].push_back({
                    { "symbol", iter->second->szSymbol },
                    { "sell_price", iter->second->m_last_snap.bid_price[0] },
                    { "sell_volume", iter->second->m_info.m_disclosure_vol },
                    { "sell_turnover", iter->second->nBidIopvFactor.load() },
                    { "sell_fee" , iter->second->nSellFee.load() },
                    { "low_limit_turnover", iter->second->nLowLimitValue.load()},
                    { "redeeming_cost" , iter->second->nSellCost.load() },
                    { "occupation_fund" , iter->second->nBuyCost.load() * (1 - iter->second->m_info.m_redemption_discount_ratio) },
                });
                total_cost += iter->second->nBuyCost.load();
                total_occupation_fund += iter->second->nBuyCost.load() * (1 - iter->second->m_info.m_redemption_discount_ratio);
                total_low_limit_turnover += iter->second->nLowLimitValue;
            }
        }else{  //现金替代
            double replace_amount = iter->second->m_info.m_redemption_amount ? iter->second->m_info.m_redemption_amount : iter->second->m_info.m_replace_amount ;
            double occupation_fund = replace_amount * ( 1 - iter->second->m_info.m_redemption_discount_ratio);
            out["replace_stock_array"].push_back({
                { "symbol", iter->second->szSymbol },
                { "replace_amount", replace_amount },
                { "premium_ratio", iter->second->m_info.m_redemption_discount_ratio },
                { "occupation_fund", occupation_fund },     //资金占用
                { "redeeming_cost" , iter->second->nSellCost.load() }
            });
            total_cost += iter->second->nSellCost.load();
            total_occupation_fund += occupation_fund;
        }
    }

    out["total_cost"] = total_cost;
    out["total_occupation_fund"] = total_occupation_fund;
    out["total_low_limit_turnover"] = total_low_limit_turnover;

    out["redeeming_profit_ratio"] = pOperator_->nRedeemingCost.load() / total_occupation_fund;
    out["estimated_cash_difference"] = pOperator_->m_info->m_publicEstimatedCashDifference;     //预估线性差额
    
    // etf 交易信息
    double buy_price = IaEtfPriceTool::get_order_dealt_price(
        pOperator_->m_minUnit, pOperator_->m_etf_last_snap.ask_price, pOperator_->m_etf_last_snap.ask_volume, TI_STOCK_ARRAY_LEN);

    out["etf_trading_info"] = {
        { "ask_price_1", pOperator_->m_etf_last_snap.ask_price[0] },
        { "buy_price" , buy_price } ,
        { "buy_vol" , pOperator_->m_minUnit } ,
        { "buy_turnover", buy_price * pOperator_->m_minUnit },
    };
};

void IaETFTradingSignalFilter::calc_limit_redeeming_profit(const IaETFFactorOperator* pOperator_, json &out)
{
    out["symbol"] = pOperator_->m_etf_last_snap.symbol;
    out["exchange"] = pOperator_->m_etf_last_snap.exchange;
    out["time"] = pOperator_->m_etf_last_snap.time;
    out["time_str"] = datetime::get_format_timestamp_ms(pOperator_->m_etf_last_snap.timestamp);
    out["limit_redeeming_profit"] = pOperator_->nLimitOrderDiscountProfit.load();
    out["limit_redeeming_profit_ratio"] = 0;
    out["diff"] = pOperator_->nDiff.load();
    out["trade_stock_array"] = json::array();       //交易股票列表
    out["fund_trade_stock_array"] = json::array();  //公募交易股票列表
    out["replace_stock_array"] = json::array();     //现金替代股票列表
    out["stock_diff"] = json::array();              //差值

    double total_cost = 0;
    double total_occupation_fund = 0;
    double total_low_limit_turnover = 0;
    
    for (auto iter = pOperator_->m_cashMap.begin(); iter != pOperator_->m_cashMap.end(); ++iter)
    {
        out["stock_diff"].push_back({
            { "symbol", iter->second->szSymbol },
            { "diff" , iter->second->nDiffFactor.load() }
        });
        if(!iter->second->bReplace){    //股票交易
            if (!strcmp(pOperator_->m_etf_last_snap.exchange, iter->second->szExchange)) //同一市场自己交易
            {
                out["trade_stock_array"].push_back({
                    { "symbol", iter->second->szSymbol },
                    { "sell_price", iter->second->m_last_snap.bid_price[0] },
                    { "sell_volume", iter->second->m_info.m_disclosure_vol },
                    { "sell_turnover", iter->second->nBidIopvFactor.load() },
                    { "sell_fee" , iter->second->nSellFee.load() },
                    { "low_limit_turnover", iter->second->nLowLimitValue.load()},
                    { "redeeming_cost" , iter->second->nSellCost.load() }
                });
                total_cost += iter->second->nBuyCost.load();
                total_occupation_fund += iter->second->nBuyCost.load();
                total_low_limit_turnover += iter->second->nLowLimitValue;
            }else{
                out["fund_trade_stock_array"].push_back({
                    { "symbol", iter->second->szSymbol },
                    { "sell_price", iter->second->m_last_snap.bid_price[0] },
                    { "sell_volume", iter->second->m_info.m_disclosure_vol },
                    { "sell_turnover", iter->second->nBidIopvFactor.load() },
                    { "sell_fee" , iter->second->nSellFee.load() },
                    { "low_limit_turnover", iter->second->nLowLimitValue.load()},
                    { "redeeming_cost" , iter->second->nSellCost.load() },
                    { "occupation_fund" , iter->second->nBuyCost.load() * (1 - iter->second->m_info.m_redemption_discount_ratio) },
                });
                total_cost += iter->second->nBuyCost.load();
                total_occupation_fund += iter->second->nBuyCost.load() * (1 - iter->second->m_info.m_redemption_discount_ratio);
                total_low_limit_turnover += iter->second->nLowLimitValue;
            }
        }else{  //现金替代
            double replace_amount = iter->second->m_info.m_redemption_amount ? iter->second->m_info.m_redemption_amount : iter->second->m_info.m_replace_amount ;
            double occupation_fund = replace_amount * ( 1 - iter->second->m_info.m_redemption_discount_ratio);
            out["replace_stock_array"].push_back({
                { "symbol", iter->second->szSymbol },
                { "replace_amount", replace_amount },
                { "premium_ratio", iter->second->m_info.m_redemption_discount_ratio },
                { "occupation_fund", occupation_fund },     //资金占用
                { "redeeming_cost" , iter->second->nSellCost.load() }
            });
            total_cost += iter->second->nSellCost.load();
            total_occupation_fund += occupation_fund;
        }
    }

    out["total_cost"] = total_cost;
    out["total_occupation_fund"] = total_occupation_fund;
    out["total_low_limit_turnover"] = total_low_limit_turnover;

    out["limit_redeeming_profit_ratio"] = pOperator_->nLimitOrderDiscountProfit.load() / total_occupation_fund;
    out["estimated_cash_difference"] = pOperator_->m_info->m_publicEstimatedCashDifference;     //预估线性差额
    
    // etf 交易信息
    double buy_price = pOperator_->m_etf_last_snap.bid_price[0];

    out["etf_trading_info"] = {
        { "ask_price_1", pOperator_->m_etf_last_snap.ask_price[0] },
        { "buy_price" , buy_price } ,
        { "buy_vol" , pOperator_->m_minUnit } ,
        { "buy_turnover", buy_price * pOperator_->m_minUnit },
    };
};


double IaETFTradingSignalFilter::get_limit_redeeming_price(const IaETFFactorOperator* pOperator_)
{
    for (size_t i = 0; i < 5; i++)
    {
        if (pOperator_->nLimitOrderDiscountProfitList[i].nDiscountProfit > pOperator_->nDiscountProfitStep )
        {
            return pOperator_->nLimitOrderDiscountProfitList[i].nPrice;
        }
    }
    return 0;
};

bool IaETFTradingSignalFilter::start_limit_redeeming(const IaETFFactorOperator* pOperator_, double price)
{
    if (pOperator_->nLowLimitValue.load())  //过滤涨跌停
    {
        return false;
    }
    if (!pOperator_->nDiff)  //过滤没有真实清单
    {
        return false;
    }
    double _profit = 0;
    int level = 4;
    for (size_t i = 0; i < 5; i++)
    {
        if(std::abs(pOperator_->nLimitOrderDiscountProfitList[i].nPrice - price) <= 0.00001)
        {
            _profit = pOperator_->nLimitOrderDiscountProfitList[i].nDiscountProfit;
            level = i;
            break;
        }
    }
    std::cout << "[start_limit_redeeming] " << pOperator_->m_etf_last_snap.symbol << " profit: " <<  _profit << " level: " << level << " profitStep:" << pOperator_->nDiscountProfitStep << " stop:" <<  pOperator_->nDiscountProfitStep * 0.2 << std::endl;
    if(_profit < (0.5 * pOperator_->nDiscountProfitStep) || level > 1)
    {
        return false;
    }
    return true;
};

bool IaETFTradingSignalFilter::allow_limit_redeeming(const IaETFFactorOperator* pOperator_, double price)
{
    if (pOperator_->nLowLimitValue.load())  //过滤涨跌停
    {
        return false;
    }
    if (!pOperator_->nDiff)  //过滤没有真实清单
    {
        return false;
    }
    double _profit = 0;
    int level = 4;
    for (size_t i = 0; i < 5; i++)
    {
        if(std::abs(pOperator_->nLimitOrderDiscountProfitList[i].nPrice - price) <= 0.00001)
        {
            _profit = pOperator_->nLimitOrderDiscountProfitList[i].nDiscountProfit;
            level = i;
            break;
        }
    }
    if (!level)
    {
        std::cout << "[allow_limit_redeeming] " << pOperator_->m_etf_last_snap.symbol << " profit: " <<  _profit << " level: " << level << " profitStep:" << pOperator_->nDiscountProfitStep << " stop:" <<  pOperator_->nDiscountProfitStep * 0.1 << std::endl;
    }
    if(_profit < (0.1 * pOperator_->nDiscountProfitStep) || level >= 3)
    {
        return false;
    }
    return true;
};
