#ifndef __OC_BOOK_WORKER_H__
#define __OC_BOOK_WORKER_H__

#include "ti_book_engine.h"
#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"

class OcBookWorker 
    : public TiQuoteCallback, public TiQuoteDepthCallback
{
public:
    OcBookWorker(int32_t id, TiQuoteCallback* callback, TiQuoteDepthCallback* depth_callback);
    virtual ~OcBookWorker();
private:
    TiBookEngine*                   m_book_engine;
    TiQuoteSnapshotStockField       m_book_snap_cash;
    int32_t m_cout_time;
    int32_t m_id;
    TiQuoteCallback* m_callback;
    TiQuoteDepthCallback* m_depth_callback;
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData);

    virtual void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids);

    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData);
    
    virtual void OnDepthSnapshotRtn(const TiQuoteSnapshotStockField* pData);

};

#endif