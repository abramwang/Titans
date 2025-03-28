#ifndef TI_DF_QUOTE_L1_CLIENT_H
#define TI_DF_QUOTE_L1_CLIENT_H

#include <memory>
#include <unordered_map>
#include "quote_api.h"
#include "ti_quote_callback.h"
#include "ti_quote_tools.h"

class TiDfQuoteL1Client : public EMQ::API::QuoteSpi
{
public:
    TiDfQuoteL1Client(std::string host, int port, 
        std::string account, std::string pass,
        TiQuoteCallback* userCb);
    ~TiDfQuoteL1Client();

protected:
    virtual void OnQueryAllTickersFullInfo(EMTQuoteFullInfo* qfi, EMTRspInfoStruct* error_info, bool is_last);

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
    TiQuoteCallback* m_cb;
    EMQ::API::QuoteApi *m_quote_api;

    unsigned int m_trading_day;
    TiQuoteSnapshotStockField   m_snapStockCash;
    std::unordered_map<int64_t, std::shared_ptr<TiQuoteSnapshotStockField>> m_snapshot_map;
    std::unordered_map<int64_t, std::shared_ptr<TiQuoteContractInfoField>> m_contract_map;

private:
    void Init(std::string host, int port, 
        std::string account, std::string pass);
    void formatQuoteUpdatetime(unsigned long long quote_update_time, int32_t &date, int32_t &time, int64_t &timestamp);

public:
    void subData(const char* exchangeName, char* codeList[], size_t len);
    TiQuoteSnapshotStockField* GetStockSnapshot(const char* symbol, const char* exchange);
    TiQuoteContractInfoField* GetContractInfo(const char* symbol, const char* exchange);
};

#endif