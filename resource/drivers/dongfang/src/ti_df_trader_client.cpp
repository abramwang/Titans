#include "ti_df_trader_client.h"

#include <chrono>
#include <iostream>
#include <ostream>
#include <string.h>
#include <thread>

TiDfTraderClient::TiDfTraderClient() : client_order_id_(0), req_id_(0), trader_api_(nullptr) { Init(); }

TiDfTraderClient::~TiDfTraderClient() {
    if (trader_api_) {
        trader_api_->Release();
        trader_api_ = nullptr;
    }
}

int TiDfTraderClient::Login(const char *fund_acc) {
    // 交易登录，请将user替换成您的资金账户
    // 柜台通过客户机ip白名单进行登录鉴权控制
    // 密码字段使用EMC客户端时需输入，EMT_API当前不对密码进行校验
    auto session_id_ = trader_api_->Login("61.152.230.41", 19088, fund_acc, "OW4273", EMT_PROTOCOL_TCP);
    if (session_id_ == 0) {
        auto last_error = trader_api_->GetApiLastError();
        std::cout << "Login Failed! fund_acc: " << fund_acc << ", error code: " << last_error->error_id << ", error msg: " << last_error->error_msg << std::endl
                  << std::endl;
        return -1;
    }
    session_map_[fund_acc] = session_id_;
    return 0;
}

void TiDfTraderClient::Init() {
    // 初始化交易类Api: client_id取值在1-127,如有同一资金账号多端登录需求，请保证client_id不重复
    trader_api_ = EMT::API::TraderApi::CreateTraderApi(27, "./emt_api_log/emt.log", EMT_LOG_LEVEL_DEBUG);
    if (!trader_api_) {
        std::cout << "CreateTraderApi Failed!" << std::endl;
        return;
    }

    // 设置接收回报类SPI
    trader_api_->RegisterSpi(this);

    // 订阅公共流，QUICK为只订阅登录后的数据流
    trader_api_->SubscribePublicTopic(EMT_TERT_QUICK);

    // 交易登录
    Login("510100025168");

    // 多资金账号可以同时登录，通过Login接口返回的session_id进行区分
    //Login("user2");
}

void TiDfTraderClient::OnOrderEvent(EMTOrderInfo *order_info, EMTRI *error_info, uint64_t session_id) {
    // 每次订单状态更新时，都会被调用
    std::cout << "-------------OnOrderEvent------------" << std::endl;
    if (error_info && error_info->error_id != 0) {
        // 订单被拒绝或者发生错误，打印错误信息
        std::cout << "OnOrderEvent: error code: " << error_info->error_id << ", error msg: " << error_info->error_msg;
        return;
    }
    // 报单正常，打印回报内容
    std::cout << "order_emt_id: " << order_info->order_emt_id << std::endl
              << "ticker: " << order_info->ticker << std::endl
              << "qty_traded: " << order_info->qty_traded << std::endl
              << "qty_left: " << order_info->qty_left << std::endl
              << "order_status: " << order_info->order_status << std::endl
              << "order_submit_status: " << order_info->order_submit_status << std::endl
              << std::endl;
}

void TiDfTraderClient::OnTradeEvent(EMTTradeReport *trade_info, uint64_t session_id) {
    // 订单有成交发生的时候，会被调用，如果有分笔成交，会调用多次
    // 打印成交回报内容
    std::cout << "-------------OnTradeEvent------------" << std::endl;
    std::cout << "order_emt_id: " << trade_info->order_emt_id << std::endl
              << "order_client_id: " << trade_info->order_client_id << std::endl
              << "ticker: " << trade_info->ticker << std::endl
              << "price: " << trade_info->price << std::endl
              << "quantity: " << trade_info->quantity << std::endl
              << "trade_amount: " << trade_info->trade_amount << std::endl
              << "trade_time: " << trade_info->trade_time << std::endl
              << std::endl;
}

void TiDfTraderClient::OnCancelOrderError(EMTOrderCancelInfo *cancel_info, EMTRI *error_info, uint64_t session_id) {
    // 撤单出错时会被调用
    std::cout << "-------------OnCancelOrderError------------" << std::endl;
    if (error_info && error_info->error_id != 0) {
        // 打印错误信息
        std::cout << "OnCancelOrderError: error code: " << error_info->error_id << ", error msg: " << error_info->error_msg << std::endl;
        return;
    }
    std::cout << "order_cancel_emt_id: " << cancel_info->order_cancel_emt_id << std::endl
              << "order_emt_id: " << cancel_info->order_emt_id << std::endl
              << std::endl;
}

void TiDfTraderClient::OnQueryOrderByPage(EMTQueryOrderRsp *order_info, int64_t req_count, int64_t order_sequence, int64_t query_reference, int request_id,
                                   bool is_last, uint64_t session_id) {
    std::cout << "-------------OnQueryOrderByPage------------" << std::endl;
    std::cout << "req_count: " << req_count << std::endl           // 本次分页请求的数量
              << "order_sequence: " << order_sequence << std::endl // 当前回报在本次分页请求中的序号
              << "query_reference: " << query_reference << std::endl // 下次分页查询的起始reference，实质上是当前回报在整个委托列表中的序号+1
              << "is_last: "
              << is_last // 当is_last为true时，如果sequence等于req_count，表示可能还有后续结果，可以进行下一次分页查询请求；不等则表示所有结果已经查询完毕
              << std::endl;
    if (!order_info) {
        std::cout << "Empty response!" << std::endl << std::endl;
        return;
    }
    std::cout << "order_emt_id: " << order_info->order_emt_id << std::endl
              << "ticker: " << order_info->ticker << std::endl
              << "price: " << order_info->price << std::endl
              << "quantity: " << order_info->quantity << std::endl
              << "qty_traded: " << order_info->qty_traded << std::endl
              << "qty_left: " << order_info->qty_left << std::endl
              << "insert_time: " << order_info->insert_time << std::endl
              << std::endl;
    if (!is_last) {
        // 本次分页查询回报尚未结束，等待下一条回报
        return;
    }
    if (req_count == order_sequence) {
        EMTQueryByPageReq req{
            .req_count = 100,
            .reference = query_reference,
        };
        trader_api_->QueryOrdersByPage(&req, session_id, req_id_++);
    } else {
        std::cout << "----------OnQueryOrderByPage END-----------" << std::endl << std::endl;
    }

    // 休眠5ms, 避免查询过于频繁超过流控限制
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void TiDfTraderClient::OnQueryTrade(EMTQueryTradeRsp *trade_info, EMTRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    std::cout << "-------------OnQueryTrade------------" << std::endl;
    if (error_info && error_info->error_id != 0 || !trade_info) {
        // 发生错误，打印错误信息
        std::cout << "OnQueryTrade: error code: " << error_info->error_id << ", error msg: " << error_info->error_msg << std::endl;
        return;
    }
    std::cout << "order_emt_id: " << trade_info->order_emt_id << std::endl
              << "order_client_id: " << trade_info->order_client_id << std::endl
              << "ticker: " << trade_info->ticker << std::endl
              << "price: " << trade_info->price << std::endl
              << "quantity: " << trade_info->quantity << std::endl
              << "trade_amount: " << trade_info->trade_amount << std::endl
              << "trade_time: " << trade_info->trade_time << std::endl
              << std::endl;
}

void TiDfTraderClient::OnQueryPositionByPage(EMTQueryStkPositionRsp *trade_info, int64_t req_count, int64_t trade_sequence, int64_t query_reference, int request_id,
                                      bool is_last, uint64_t session_id) {
    std::cout << "-------------OnQueryPositionByPage------------" << std::endl;
    std::cout << "req_count: " << req_count << std::endl           // 本次分页请求的数量
              << "trade_sequence: " << trade_sequence << std::endl // 当前回报在本次分页请求中的序号
              << "query_reference: " << query_reference << std::endl // 下次分页查询的起始reference，实质上是当前回报在整个持仓列表中的序号+1
              << "is_last: "
              << is_last // 当is_last为true时，如果sequence等于req_count，表示可能还有后续结果，可以进行下一次分页查询请求；不等则表示所有结果已经查询完毕
              << std::endl;
    if (!trade_info) {
        std::cout << "Empty response!" << std::endl << std::endl;
        return;
    }
    std::cout << "ticker_name: " << trade_info->ticker_name << std::endl
              << "ticker: " << trade_info->ticker << std::endl
              << "market: " << trade_info->market << std::endl
              << "total_qty: " << trade_info->total_qty << std::endl
              << "sellable_qty: " << trade_info->sellable_qty << std::endl
              << "avg_price: " << trade_info->avg_price << std::endl
              << std::endl;
    if (!is_last) {
        // 本次分页查询回报尚未结束，等待下一条回报
        return;
    }
    if (req_count == trade_sequence) {
        EMTQueryByPageReq req{
            .req_count = 100,
            .reference = query_reference,
        };
        trader_api_->QueryPositionByPage(&req, session_id, req_id_++);
    } else {
        std::cout << "----------OnQueryPositionByPage END-----------" << std::endl << std::endl;
    }

    // 休眠5ms, 避免查询过于频繁超过流控限制
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
}


void TiDfTraderClient::connect() {
    // 报单
    EMTOrderInsertInfo order_info{0};
    order_info.order_client_id = client_order_id_++;
    memcpy(order_info.ticker, "000001", 7);
    order_info.market = EMT_MKT_SZ_A;
    order_info.price = 10;
    order_info.price_type = EMT_PRICE_FORWARD_BEST;
    order_info.side = EMT_SIDE_BUY;
    order_info.business_type = EMT_BUSINESS_TYPE_CASH;
    order_info.quantity = 100;
    auto session_id = session_map_["510100025168"];
    auto emt_id = trader_api_->InsertOrder(&order_info, session_id);
    if (emt_id == 0) {
        auto last_error = trader_api_->GetApiLastError();
        std::cout << "InsertOrder Failed! error code: " << last_error->error_id << ", error msg: " << last_error->error_msg << std::endl;
        return;
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 撤单，撤单成功会回调OnOrderEvent，失败回调virtual void OnCancelOrderError
    trader_api_->CancelOrder(emt_id, session_id);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 分页查询报单
    EMTQueryOrderByPageReq query_order{
        .req_count = 100, // 单次查询的数量
        .reference = 0,   // 从头查询
    };
    trader_api_->QueryOrdersByPage(&query_order, session_id, req_id_++);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 分页查询持仓
    EMTQueryPositionByPageReq query_position{
        .req_count = 100, // 单次查询的数量
        .reference = 0,   // 从头查询
    };
    trader_api_->QueryPositionByPage(&query_position, session_id, req_id_++);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // 查询成交
    EMTQueryTraderReq query_trade{};
    trader_api_->QueryTrades(&query_trade, session_id, req_id_++);
    std::cout << "QueryTrades" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
};

int TiDfTraderClient::orderInsertBatch(std::vector<TiReqOrderInsert> &req_vec, std::string account_id)
{
    return 0;
};

int TiDfTraderClient::orderInsert(TiReqOrderInsert* req)
{
    return 0;
};

int TiDfTraderClient::orderDelete(TiReqOrderDelete* req)
{
    return 0;
};

TiRtnOrderStatus* TiDfTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    return nullptr;
};

int TiDfTraderClient::QueryAsset()
{
    return 0;
};
int TiDfTraderClient::QueryOrders()
{
    return 0;
};
int TiDfTraderClient::QueryMatches()
{
    return 0;
};
int TiDfTraderClient::QueryPositions()
{
    return 0;
};