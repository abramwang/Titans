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
    void OnLv2SnapSze(EMQSzeSnap *snap) override;
    // 深交所逐笔合并行情
    void OnLv2TickSze(EMQSzeTick *tick) override;
    // 深交所指数行情
    void OnLv2IndexSze(EMQSzeIdx *idx) override;
    // 深交所债券快照行情
    void OnLv2BondSnapSze(EMQSzeBondSnap *bond_snap) override;
    // 深交所债券逐笔合并行情
    void OnLv2BondTickSze(EMQSzeBondTick *bond_tick) override;
    // 深交所建树行情
    void OnLv2TreeSze(EMQSzeTree *tree) override;

    // 上交所快照行情
    void OnLv2SnapSse(EMQSseSnap *snap) override;
    // 上交所逐笔合并行情
    void OnLv2TickSse(EMQSseTick *tick) override;
    // 上交所指数行情
    void OnLv2IndexSse(EMQSseIdx *idx) override;
    // 上交所债券快照行情
    void OnLv2BondSnapSse(EMQSseBondSnap *bond_snap) override;
    // 上交所债券逐笔行情
    void OnLv2BondTickSse(EMQSseBondTick *bond_tick) override;
    // 上交所建树行情
    void OnLv2TreeSse(EMQSseTree *tree) override;

private:
    EMQ::API::QuoteApiLv2 *quote_api_;


public:
    TiDfQuoteClient();
    ~TiDfQuoteClient();
public:
    void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif