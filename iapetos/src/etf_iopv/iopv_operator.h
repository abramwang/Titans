#ifndef IOPV_OPERATOR_H
#define IOPV_OPERATOR_H

#include <string>
#include <map>
#include <set>
#include <atomic>
#include "ti_quote_callback.h"
#include "etf_constituent_cash.h"

class IopvOperator 
    : public TiQuoteCallback
{
private:
    char m_etf_symbol[TI_SYMBOL_STR_LEN];
    char m_etf_exchange[TI_EXCHANGE_STR_LEN];
    double m_etf_quotient;
    std::map<std::string, ETFConstituentCash*> m_cashMap;

    TiQuoteSnapshotStockField   m_etf_last_snap;
    std::atomic<long long>      m_etf_timestamp;     //  etf 最新价，通过成交更新
    std::atomic<double>         m_etf_last;          //  etf 最新成交价
    
    std::atomic<long long>      m_constituent_timestamp;    // 成分最新时间
    std::atomic<double>         nIOPV;                      // 通过成分拟合的iopv

    void parseConfig(std::string &config);
    void calcIopv();
public:
    IopvOperator(std::string etf, std::string config);
    virtual ~IopvOperator();

public:
    void getSymbolSet(const char* exchange, std::set<std::string> &symbolSet);

public: //行情回调
    void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};

    void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
};

#endif