#ifndef ETF_CONSTITUENT_CASH_H
#define ETF_CONSTITUENT_CASH_H
#include "ti_quote_callback.h"
#include <string>
#include <string.h>
#include <atomic>

using namespace std;

class ETFConstituentCash
    : public TiQuoteCallback
{
public:
    char szSymbol[32];
    char szExchange[32];
    double nWeight;

    std::atomic<double> nLast;
    std::atomic<long long> nTimeStamp;
    std::atomic<double> nFactor;
public:
    ETFConstituentCash(const char* symbol, const char* exchange, double weight);
    virtual ~ETFConstituentCash();

public: //行情回调
    void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};

    void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){};
    void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
};


#endif