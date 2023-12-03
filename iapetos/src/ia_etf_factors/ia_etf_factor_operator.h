#ifndef IA_ETF_FACTOR_OPERATOR_H
#define IA_ETF_FACTOR_OPERATOR_H

#include <string>
#include <unordered_map>
#include <set>
#include <atomic>
#include "ti_quote_callback.h"
#include "ti_factor_callback.h"
#include "ia_etf_constituent_cache.h"
#include "ia_etf_fund_info.h"
#include "redis_sync_handle.h"
#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaETFFactorOperator 
    : public TiQuoteCallback
{
private:
    char m_etf_symbol[TI_SYMBOL_STR_LEN];
    char m_etf_exchange[TI_EXCHANGE_STR_LEN];
    TiFactorCallback* m_factorCallback;

public:
    TiQuoteSnapshotStockField   m_etf_last_snap;
    std::atomic<long long>      m_etf_timestamp;     //  etf 最新价，通过成交更新
    std::atomic<double>         m_etf_last;          //  etf 最新成交价
    IaETFFundInfo::FundInfo*    m_info;
    int32_t                     m_minUnit;
    
    std::unordered_map<std::string, IaETFConstituentCache*> m_cashMap;
    
public:
    std::atomic<long long>      m_constituent_timestamp;  // 成分最新时间
    std::atomic<double> nAskIOPV;                            // 卖盘口iopv
    std::atomic<double> nBidIOPV;                            // 买盘口iopv
    
    std::atomic<double> nHighLimitValue;  //涨停市值
    std::atomic<double> nLowLimitValue;   //跌停市值

    std::atomic<double> nDiff;            // 现金插值
    std::atomic<double> nPurchasingCost;  // 申购利润
    std::atomic<double> nRedeemingCost;   // 赎回利润


    json                m_factor_json;              // 因子数据
private:
    void parseConfig(IaETFFundInfo::FundInfo* info);
    void calcFactors();
    void calcTradingCost();
public:
    IaETFFactorOperator(std::string etf, IaETFFundInfo::FundInfo* info, TiFactorCallback* factorCallback);
    virtual ~IaETFFactorOperator();

public:
    void getSymbolSet(const char* exchange, std::set<std::string> &symbolSet);

public: //行情回调
    void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};

    void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

};

#endif