#ifndef TI_QUOTE_CALLBACK_H
#define TI_QUOTE_CALLBACK_H

#include "ti_quote_struct.h"

class TiQuoteCallback
{
public:
    virtual void OnEventRtn(TI_QUOTE_EVENT_TYPE event){};
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName) = 0;
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData) = 0;
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData) = 0;
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData) = 0;
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData) = 0;
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData) = 0;
    /* data */
public:
    TiQuoteCallback(){};
    virtual ~TiQuoteCallback(){};
};

#endif 