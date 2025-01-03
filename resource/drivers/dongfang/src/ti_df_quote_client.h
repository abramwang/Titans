#ifndef TI_DF_QUOTE_CLIENT_H
#define TI_DF_QUOTE_CLIENT_H

#include <string>
#include "ti_quote_callback.h"
#include "quote_api_lv2.h"


class TiDfQuoteClient : public EMQ::API::QuoteSpiLv2
{
protected:
    // inherit from EMQ::API::QuoteSpiLv2
    // 深交所快照行情
    void OnLv2SnapSze(EMQSzeSnap *snap);
    // 深交所逐笔合并行情
    void OnLv2TickSze(EMQSzeTick *tick);
    // 深交所指数行情
    void OnLv2IndexSze(EMQSzeIdx *idx);
    // 深交所债券快照行情
    void OnLv2BondSnapSze(EMQSzeBondSnap *bond_snap);
    // 深交所债券逐笔合并行情
    void OnLv2BondTickSze(EMQSzeBondTick *bond_tick);
    // 深交所建树行情
    void OnLv2TreeSze(EMQSzeTree *tree);

    // 上交所快照行情
    void OnLv2SnapSse(EMQSseSnap *snap);
    // 上交所逐笔合并行情
    void OnLv2TickSse(EMQSseTick *tick);
    // 上交所指数行情
    void OnLv2IndexSse(EMQSseIdx *idx);
    // 上交所债券快照行情
    void OnLv2BondSnapSse(EMQSseBondSnap *bond_snap);
    // 上交所债券逐笔行情
    void OnLv2BondTickSse(EMQSseBondTick *bond_tick);
    // 上交所建树行情
    void OnLv2TreeSse(EMQSseTree *tree);

private:
    EMQ::API::QuoteApiLv2 *quote_api_;
    
    TiQuoteSnapshotStockField   m_snapStockCash;
    TiQuoteSnapshotIndexField   m_snapIndexCash;
    TiQuoteOrderField           m_orderCash;
    TiQuoteMatchesField         m_matchCash;

    TiQuoteCallback* m_cb;
private:
    void formatQuoteUpdatetime(unsigned long long quote_update_time, int32_t &date, int32_t &time, int64_t &timestamp);
public:
    TiDfQuoteClient();
    ~TiDfQuoteClient();
public:
    void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif