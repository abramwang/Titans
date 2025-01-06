#ifndef TI_DF_QUOTE_L1_CLIENT_H
#define TI_DF_QUOTE_L1_CLIENT_H

#include "quote_api.h"

class TiDfQuoteL1Client : public EMQ::API::QuoteSpi
{
public:
    TiDfQuoteL1Client();
    ~TiDfQuoteL1Client();
    void Run();

protected:
    // inherit from EMQ::API::QuoteSpi
    // 订阅快照行情应答
    void OnSubMarketData(EMTSpecificTickerStruct *ticker, EMTRspInfoStruct *error_info, bool is_last) override;
    // 现货集中竞价快照行情
    void OnDepthMarketData(EMTMarketDataStruct *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[],
                           int32_t ask1_count, int32_t max_ask1_count) override;
    // 订阅全市场的指数行情应答
    void OnSubscribeAllIndexData(EMQ_EXCHANGE_TYPE exchange_id, EMTRspInfoStruct *error_info) override;
    // 指数行情通知
    void OnIndexData(EMTIndexDataStruct *index_data) override;

private:
    void Init();
    int Login(const char *fund_acc);

private:
    EMQ::API::QuoteApi *m_quote_api;
};

#endif