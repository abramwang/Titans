#ifndef TI_DF_TRADER_CLIENT_H
#define TI_DF_TRADER_CLIENT_H

#include <atomic>
#include <map>
#include <string>
#include <vector>

#include "emt_trader_api.h"

class TiDfTraderClient : public EMT::API::TraderSpi
{
public:
    TiDfTraderClient();
    ~TiDfTraderClient();
    void Run();

protected:
    // inherit from EMT::API::TraderSpi
    void OnOrderEvent(EMTOrderInfo *order_info, EMTRI *error_info, uint64_t session_id) override;
    void OnTradeEvent(EMTTradeReport *trade_info, uint64_t session_id) override;
    void OnCancelOrderError(EMTOrderCancelInfo *cancel_info, EMTRI *error_info, uint64_t session_id) override;

    void OnQueryOrderByPage(EMTQueryOrderRsp *order_info, int64_t req_count, int64_t order_sequence, int64_t query_reference, int request_id, bool is_last,
                            uint64_t session_id) override;
    void OnQueryTrade(EMTQueryTradeRsp *trade_info, EMTRI *error_info, int request_id, bool is_last, uint64_t session_id) override;
    void OnQueryPositionByPage(EMTQueryStkPositionRsp *trade_info, int64_t req_count, int64_t trade_sequence, int64_t query_reference, int request_id,
                               bool is_last, uint64_t session_id) override;

private:
    void Init();
    int Login(const char *fund_acc);

private:
    EMT::API::TraderApi *trader_api_;
    std::map<std::string, uint64_t> session_map_;
    std::atomic<uint32_t> client_order_id_;
    std::atomic<uint32_t> req_id_;
};

#endif // TI_DF_TRADER_CLIENT_H