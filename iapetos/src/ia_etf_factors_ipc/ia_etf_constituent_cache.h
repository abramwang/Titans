#ifndef IA_ETF_CONSTITUENT_CASH_H
#define IA_ETF_CONSTITUENT_CASH_H
#include "ti_quote_callback.h"
#include <string>
#include <string.h>
#include <atomic>
#include "ia_etf_fund_info.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

using namespace std;

class IaETFConstituentCache
    : public TiQuoteCallback
{
public:
    char szSymbol[32];
    char szExchange[32];
    IaETFFundInfo::ConstituentStockInfo m_info;
    double m_commission;     // 手续费

    TiQuoteSnapshotStockField m_last_snap;
    std::atomic<long long> nTimeStamp;
    std::atomic<double> nDiffFactor;

#if 0
    json m_counting_process_json;
#endif

    std::atomic<double> nBidIopvFactor; //买
    std::atomic<double> nAskIopvFactor; //卖

    std::atomic<double> nSellFee;   //卖手续费
    std::atomic<double> nBuyFee;    //买手续费

    std::atomic<bool> bReplace;   //是否现金替代

    std::atomic<double> nBuyCost;
    std::atomic<double> nSellCost;
    
    std::atomic<double> nHighLimitValue;    //涨停市值
    std::atomic<double> nLowLimitValue;     //跌停市值
public:
    IaETFConstituentCache(const char* symbol, const char* exchange, IaETFFundInfo::ConstituentStockInfo info, double commission);
    virtual ~IaETFConstituentCache();

public: //行情回调
    void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};

    void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};
    void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

private:
    void calcTradingCostByIopv(const TiQuoteSnapshotStockField* pData);
    void calcTradingCost(const TiQuoteSnapshotStockField* pData);
};


#endif
