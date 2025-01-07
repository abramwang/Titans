#ifndef TI_DF_TRADER_CLIENT_H
#define TI_DF_TRADER_CLIENT_H

#include <atomic>
#include <map>
#include <string>
#include <vector>
#include "emt_trader_api.h"
#include "ti_trader_callback.h"
#include "ti_trader_client.h"

class TiDfTraderClient 
    : public EMT::API::TraderSpi, public TiTraderClient
{
public:
    TiDfTraderClient();
    ~TiDfTraderClient();

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

public:
    void connect();
    int orderInsertBatch(std::vector<TiReqOrderInsert> &req_vec, std::string account_id);
    int orderInsert(TiReqOrderInsert* req);
    int orderDelete(TiReqOrderDelete* req);

public:
    TiRtnOrderStatus* getOrderStatus(int64_t req_id, int64_t order_id);

    int QueryAsset();
    int QueryOrders();
    int QueryMatches();
    int QueryPositions();
};

#endif // TI_DF_TRADER_CLIENT_H