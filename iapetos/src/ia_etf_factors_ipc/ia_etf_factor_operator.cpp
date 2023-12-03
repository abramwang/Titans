#include <glog/logging.h>
#include <iostream>
#include <string.h>
#include "ia_etf_factor_operator.h"
#include "ti_quote_tools.h"
#include "ia_etf_price_tool.h"
#include "datetime.h"

IaETFFactorOperator::IaETFFactorOperator(std::string etf, IaETFFundInfo::FundInfo* info, TiFactorCallback* factorCallback)
{
    memset(m_etf_symbol, 0, TI_SYMBOL_STR_LEN);
    memset(m_etf_exchange, 0, TI_EXCHANGE_STR_LEN);
    m_minUnit = 0;
    m_info = info;
    m_factorCallback = factorCallback;

    memset(&m_etf_last_snap, 0, sizeof(TiQuoteSnapshotStockField));
    m_etf_timestamp = 0;
    m_etf_last = 0;
    m_constituent_timestamp = 0;
    nAskIOPV = 0;                   // 卖盘口iopv
    nBidIOPV = 0;                   // 买盘口iopv
    nDiff = 0;
    nPurchasingCost = 0;
    nRedeemingCost = 0;
    
    nLimitOrderPremiumProfit = 0;       //挂单申购套利利润
    nLimitOrderDiscountProfit = 0;      //挂单赎回套利利润

    memset(&nLimitOrderDiscountProfitList, 0, sizeof(LimitDiscountProfitInfo)*5);      //挂单赎回套利利润
    nDiscountProfitStep = 0;

    strcpy(m_etf_symbol, etf.c_str());
    parseConfig(info);
}

IaETFFactorOperator::~IaETFFactorOperator()
{
    auto iter = m_cashMap.begin();
    for(;iter != m_cashMap.end(); iter++){
        delete iter->second;
        iter->second = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

void IaETFFactorOperator::parseConfig(IaETFFundInfo::FundInfo* info){
    strcpy(m_etf_exchange, info->m_exchange.c_str());
    m_minUnit = info->m_minUnit;
    
    for (auto i = info->m_stockList.begin(); i != info->m_stockList.end(); ++i)
    {   
        int64_t symbol_id = TiQuoteTools::GetSymbolID( i->m_exchange.c_str(), i->m_symbol.c_str());
        m_cashMap[symbol_id] = new IaETFConstituentCache(i->m_symbol.c_str(), i->m_exchange.c_str(), *i,
            i->m_exchange == "SH" ? m_info->m_sh_commission : m_info->m_sz_commission);
    }
};

/// @brief 计算 IOPV
void IaETFFactorOperator::calcFactors(){
    nDiff    = 0;
    nAskIOPV = 0;
    nBidIOPV = 0;
    
    nHighLimitValue = 0;  //涨停市值
    nLowLimitValue  = 0;  //跌停市值
#if 0
    m_factor_counting_process_json = json::array();
#endif
    for(auto iter = m_cashMap.begin(); iter != m_cashMap.end(); iter++){
        nDiff = (nDiff + iter->second->nDiffFactor);
        nAskIOPV = (nAskIOPV + iter->second->nAskIopvFactor);
        nBidIOPV = (nBidIOPV + iter->second->nBidIopvFactor);
        nHighLimitValue = (nHighLimitValue + iter->second->nHighLimitValue);
        nLowLimitValue  = (nLowLimitValue + iter->second->nLowLimitValue);
#if 0
        m_factor_counting_process_json.push_back(iter->second->m_counting_process_json);
#endif
    }
    nAskIOPV = (nAskIOPV + m_info->m_publicEstimatedCashDifference) / m_minUnit;
    nBidIOPV = (nBidIOPV + m_info->m_publicEstimatedCashDifference) / m_minUnit;
    calcTradingCost();
    calcLimitOrderProfit();
    nPurchasingCost = nPurchasingCost - nDiff - m_info->m_publicEstimatedCashDifference;
    nRedeemingCost = nRedeemingCost + nDiff + m_info->m_publicEstimatedCashDifference;

    nLimitOrderPremiumProfit = nLimitOrderPremiumProfit - nDiff - m_info->m_publicEstimatedCashDifference;
    nLimitOrderDiscountProfit = nLimitOrderDiscountProfit + nDiff + m_info->m_publicEstimatedCashDifference;

    double _purchasingCost = 0;
    json redeeming_array = json::array();
    nDiscountProfitStep = 0;
    for (size_t i = 0; i < 5; i++)
    {
        nLimitOrderDiscountProfitList[i].nPrice = m_etf_last_snap.bid_price[i];
        calcProfitWithEtfDealtPrice(nLimitOrderDiscountProfitList[i].nPrice, _purchasingCost, nLimitOrderDiscountProfitList[i].nDiscountProfit);
        nLimitOrderDiscountProfitList[i].nDiscountProfitRatio = nLimitOrderDiscountProfitList[i].nDiscountProfit / (nLimitOrderDiscountProfitList[i].nPrice * m_minUnit);
        redeeming_array[i] = {
            { "etf_bid_price", nLimitOrderDiscountProfitList[i].nPrice},
            { "redeeming_profit", nLimitOrderDiscountProfitList[i].nDiscountProfit},
        };
        if (i > 1)
        {
            nDiscountProfitStep += nLimitOrderDiscountProfitList[i].nDiscountProfit - nLimitOrderDiscountProfitList[i-1].nDiscountProfit;
        }
    }
    nDiscountProfitStep = nDiscountProfitStep/4;
    
    TI_ISODateTimeType _iso_time;
    
    datetime::get_format_timestamp_ms(m_constituent_timestamp.load(), _iso_time, TI_TIME_STR_LEN);
    
    m_factor_json = {
        { "symbol", m_etf_symbol},
        { "datetime", _iso_time},
        { "ask_iopv", nAskIOPV.load()},
        { "bid_iopv", nBidIOPV.load()},
        { "high_limit_value", nHighLimitValue.load()},
        { "low_limit_value", nLowLimitValue.load()},
    //    { "last", m_etf_last_snap.last ? m_etf_last_snap.last : ( (!m_etf_last_snap.ask_price[0] && !m_etf_last_snap.bid_price[0]) ? m_etf_last_snap.pre_close : (m_etf_last_snap.ask_price[0] + m_etf_last_snap.bid_price[0])/2) },
        { "diff", nDiff.load()},
        { "purchasing", nPurchasingCost.load()},
        { "redeeming", nRedeemingCost.load()},
        //{ "limit_purchasing", nLimitOrderPremiumProfit.load()},
        { "limit_redeeming", nLimitOrderDiscountProfit.load()},
        { "redeeming_array", redeeming_array}
    };
};

void IaETFFactorOperator::calcTradingCost()
{
    nPurchasingCost = 0;
    nRedeemingCost = 0;

    double buy_cost = 0;
    double sell_cost = 0;

#if 0
    int buy_vol = 0;
    int sell_vol = 0;
    double buy_turnover = 0;
    double sell_turnover = 0;
    for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
    {
        // 买
        int _buy_vol_single = m_etf_last_snap.ask_volume[i] > (m_minUnit - buy_vol) ? (m_minUnit - buy_vol) : m_etf_last_snap.ask_volume[i];
        if (_buy_vol_single > 0){
            buy_vol += _buy_vol_single;
            buy_turnover += _buy_vol_single*m_etf_last_snap.ask_price[i];
        }

        // 卖
        int _sell_vol_single = m_etf_last_snap.bid_volume[i] > (m_minUnit - sell_vol) ? (m_minUnit - sell_vol) : m_etf_last_snap.bid_volume[i];
        if (_sell_vol_single > 0){
            sell_vol += _sell_vol_single;
            sell_turnover += _sell_vol_single*m_etf_last_snap.bid_price[i];
        }
    }
    // 涨跌停没有买到的情况
    if (buy_vol !=  m_minUnit){
        int _buy_vol_single = m_minUnit - buy_vol;
        buy_vol += _buy_vol_single;
        buy_turnover += _buy_vol_single*m_etf_last_snap.high_limit*1.05;
        //printf("[IaETFFactorOperator::calcTradingCost] [buy_vol] %s, %d, %f, %f, %d, %d\n", m_etf_last_snap.symbol, buy_vol, m_minUnit, m_etf_last_snap.ask_price[0], m_etf_last_snap.ask_volume[0], m_etf_last_snap.ask_order_num[0]);
    }
    if (sell_vol !=  m_minUnit){
        
        int _sell_vol_single = m_minUnit - sell_vol;
        sell_vol += _sell_vol_single;
        sell_turnover += _sell_vol_single*m_etf_last_snap.low_limit*0.95;
        //printf("[IaETFFactorOperator::calcTradingCost] [sell_vol] %s, %d, %f, %f, %d, %d\n", m_etf_last_snap.symbol, sell_vol, m_minUnit, m_etf_last_snap.bid_price[0], m_etf_last_snap.bid_volume[0], m_etf_last_snap.bid_order_num[0]);
    }
    // 交易成本算上手续费
    buy_cost = buy_turnover + buy_turnover * (0.00001);        // 万1 佣金
    sell_cost = sell_turnover - sell_turnover * (0.00001);     // 万1 佣金
#else
    double buy_price    = IaEtfPriceTool::get_order_dealt_price(
        m_minUnit, m_etf_last_snap.ask_price, m_etf_last_snap.ask_volume, TI_STOCK_ARRAY_LEN);
    double sell_price   = IaEtfPriceTool::get_order_dealt_price(
        m_minUnit, m_etf_last_snap.bid_price, m_etf_last_snap.bid_volume, TI_STOCK_ARRAY_LEN);

#if 0
    buy_cost = m_etf_last_snap.ask_price[0] ? m_etf_last_snap.ask_price[0] * m_minUnit : m_etf_last_snap.bid_price[0] * m_minUnit;
    sell_cost = m_etf_last_snap.bid_price[0] ? m_etf_last_snap.bid_price[0] * m_minUnit : m_etf_last_snap.ask_price[0] * m_minUnit;
#else
    buy_cost = buy_price ? buy_price * m_minUnit : buy_price * m_minUnit;
    sell_cost = sell_price ? sell_price * m_minUnit : sell_price * m_minUnit;
#endif

    if (!buy_cost && !sell_cost)    //早盘集合竞价没有报价时候
    {
        buy_cost = m_etf_last_snap.pre_close * m_minUnit;
        sell_cost = m_etf_last_snap.pre_close * m_minUnit;
    }

    buy_cost = buy_cost + buy_cost * (0.00001);        // 万1 佣金
    sell_cost = sell_cost - sell_cost * (0.00001);     // 万1 佣金
#endif

    auto iter = m_cashMap.begin();
    for(; iter != m_cashMap.end(); iter++){
        nPurchasingCost = (nPurchasingCost + iter->second->nBuyCost);
        nRedeemingCost = (nRedeemingCost + iter->second->nSellCost);
    }
    nPurchasingCost = sell_cost - nPurchasingCost;
    nRedeemingCost = nRedeemingCost - buy_cost;
};

void IaETFFactorOperator::calcLimitOrderProfit()
{
    nLimitOrderPremiumProfit = 0;       //挂单申购套利利润
    nLimitOrderDiscountProfit = 0;      //挂单赎回套利利润
    
    double buy_cost = 0;
    double sell_cost = 0;

    double buy_price    = m_etf_last_snap.bid_price[0];
    double sell_price   = m_etf_last_snap.ask_price[0];

    buy_cost = buy_price ? buy_price * m_minUnit : buy_price * m_minUnit;
    sell_cost = sell_price ? sell_price * m_minUnit : sell_price * m_minUnit;

    if (!buy_cost && !sell_cost)    //早盘集合竞价没有报价时候
    {
        buy_cost = m_etf_last_snap.pre_close * m_minUnit;
        sell_cost = m_etf_last_snap.pre_close * m_minUnit;
    }

    buy_cost = buy_cost + buy_cost * (0.00001);        // 万1 佣金
    sell_cost = sell_cost - sell_cost * (0.00001);     // 万1 佣金

    auto iter = m_cashMap.begin();
    for(; iter != m_cashMap.end(); iter++){
        nLimitOrderPremiumProfit = (nLimitOrderPremiumProfit + iter->second->nBuyCost);
        nLimitOrderDiscountProfit = (nLimitOrderDiscountProfit + iter->second->nSellCost);
    }
    nLimitOrderPremiumProfit = sell_cost - nLimitOrderPremiumProfit;
    nLimitOrderDiscountProfit = nLimitOrderDiscountProfit - buy_cost;
};

////////////////////////////////////////////////////////////////////////
// 工具方法
////////////////////////////////////////////////////////////////////////
void IaETFFactorOperator::getSymbolSet(const char* exchange, std::set<std::string> &symbolSet){
    auto iter = m_cashMap.begin();
    for (; iter != m_cashMap.end() ; iter ++)
    {
        if(!strcmp(exchange, iter->second->szExchange)){
            symbolSet.insert(iter->second->szSymbol);
        }
    }
    if(!strcmp(m_etf_exchange, exchange)){
        symbolSet.insert(m_etf_symbol);
    }
};

void IaETFFactorOperator::calcProfitWithEtfDealtPrice(double etfDealtPrice, double &purchasingCost, double &redeemingCost)
{
    purchasingCost = 0;
    redeemingCost = 0;

    double buy_cost = etfDealtPrice * m_minUnit;
    double sell_cost = etfDealtPrice * m_minUnit;
    
    buy_cost = buy_cost + buy_cost * (0.00001);        // 万1 佣金
    sell_cost = sell_cost - sell_cost * (0.00001);     // 万1 佣金

    auto iter = m_cashMap.begin();
    for(; iter != m_cashMap.end(); iter++){
        purchasingCost = (purchasingCost + iter->second->nBuyCost);
        redeemingCost = (redeemingCost + iter->second->nSellCost);
    }
    purchasingCost = sell_cost - purchasingCost;
    redeemingCost = redeemingCost - buy_cost;

    purchasingCost = purchasingCost - nDiff - m_info->m_publicEstimatedCashDifference;
    redeemingCost = redeemingCost + nDiff + m_info->m_publicEstimatedCashDifference;
};

////////////////////////////////////////////////////////////////////////
// 行情回调
////////////////////////////////////////////////////////////////////////

void IaETFFactorOperator::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    auto iter = m_cashMap.find(TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol));
    if(iter != m_cashMap.end()){
        iter->second->OnL2StockSnapshotRtn(pData);
        //250 毫秒计算一次
        if((pData->timestamp - m_constituent_timestamp) > 250)
        {
            calcFactors();
            m_constituent_timestamp = pData->timestamp;
            if ((pData->time > 93000000 && pData->time < 113000000) ||
                (pData->time > 130000000 && pData->time < 145700000))
            {
                if (m_factorCallback)
                {
                    m_factorCallback->OnFactorRtn(m_etf_symbol, "etf_factor", &m_factor_json, this);
                }
            }
        }
    }

    if(strcmp(pData->symbol, m_etf_symbol)){
        return;
    }
    if( pData->volume && pData->last){
        m_etf_timestamp = pData->timestamp;
        m_etf_last = pData->last;
    }
    memcpy(&m_etf_last_snap, pData, sizeof(TiQuoteSnapshotStockField));

    calcFactors();
    m_constituent_timestamp = pData->timestamp;
    if ((pData->time > 93000000 && pData->time < 113000000) ||
        (pData->time > 130000000 && pData->time < 145700000))
    {
        if (m_factorCallback)
        {
            m_factorCallback->OnFactorRtn(m_etf_symbol, "etf_factor", &m_factor_json, this);
        }
    }
};