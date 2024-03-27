#ifndef __IA_ETF_SIGNAL_WORKER_CENTER_H__
#define __IA_ETF_SIGNAL_WORKER_CENTER_H__

#include "ti_quote_callback.h"

class IaEtfSignalCenter 
    : public TiQuoteCallback
{
/*   行情回调   */
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){};
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};
public:
    IaEtfSignalCenter(/* args */);
    ~IaEtfSignalCenter();
};

IaEtfSignalCenter::IaEtfSignalCenter(/* args */)
{
}

IaEtfSignalCenter::~IaEtfSignalCenter()
{
}



#endif