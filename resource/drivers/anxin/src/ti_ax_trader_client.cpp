#include "ti_ax_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"

#include <atomic>

std::atomic_bool g_connected_flag(false);           // 已连接
std::atomic_bool g_cust_logined_flag(false);        // 已登录

std::atomic_bool g_waiting_flag(false);             // 等待响应

ATPClientSeqIDType g_client_seq_id = 1;             //  客户系统消息号
ATPClientFeatureCodeType g_client_feature_code = "PC;IIP=10.2.2.184;IPORT=NA;LIP=10.2.2.184;MAC=005056b60a22;HD=WFJ17ZSQ@ICN=R;V0.0.1";  // 终端识别码，由券商指定规则

TiAxTraderClient::TiAxTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_client = NULL;
    //m_api = TraderApi::CreateTraderApi(2, "./", XTP_LOG_LEVEL_DEBUG);
	//m_api->SetHeartBeatInterval(15);
	//m_api->RegisterSpi(this);

    nSessionId = 0;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 100;   //跳过xtp client设置成交模式的区段
};


TiAxTraderClient::~TiAxTraderClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    m_cb = NULL;
    nReqId = 0;
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiAxTraderClient::OnLogin(const std::string& reason)
{
    std::cout << "OnLogin Recv:" << reason << std::endl;
    g_connected_flag.store(true);
}

// 登出回调
void TiAxTraderClient::OnLogout(const std::string& reason)
{
    std::cout << "OnLogout Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接失败
void TiAxTraderClient::OnConnectFailure(const std::string &reason)
{
    std::cout << "OnConnectFailure Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接超时
void TiAxTraderClient::OnConnectTimeOut(const std::string &reason)
{
    std::cout << "OnConnectTimeOut Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 心跳超时
void TiAxTraderClient::OnHeartbeatTimeout(const std::string &reason)
{
    std::cout << "OnHeartbeatTimeout Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接关闭
void TiAxTraderClient::OnClosed(const std::string &reason)
{
    std::cout << "OnClosed Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接结束回调
void TiAxTraderClient::OnEndOfConnection(const std::string& reason)
{
    std::cout << "OnEndOfConnection Recv:" << reason << std::endl;
    g_waiting_flag.store(false);
}

// 客户号登入回调
void TiAxTraderClient::OnRspCustLoginResp(const ATPRspCustLoginRespOtherMsg &cust_login_resp)
{
    std::cout << "OnRspCustLoginResp Recv:" << static_cast<uint32_t>(cust_login_resp.permisson_error_code) << std::endl;
    if (cust_login_resp.permisson_error_code == 0)
    {
        g_cust_logined_flag.store(true);
        std::cout << "CustLogin Success!" << std::endl;
    }
    else
    {
        std::cout << "CustLogin Fail, permisson_error_code :" << static_cast<uint32_t>(cust_login_resp.permisson_error_code) << std::endl;
    }
    
    
    g_waiting_flag.store(false);
}

// 客户号登出回调
void TiAxTraderClient::OnRspCustLogoutResp(const ATPRspCustLogoutRespOtherMsg &cust_logout_resp)
{
    std::cout << "OnRspCustLogoutResp Recv:" << static_cast<uint32_t>(cust_logout_resp.permisson_error_code) << std::endl;
    if (cust_logout_resp.permisson_error_code == 0)
    {
        g_cust_logined_flag.store(false);
        std::cout << "CustLogout Success!" << std::endl;
    }
    else
    {
        std::cout << "CustLogou Fail permisson_error_code :" << static_cast<uint32_t>(cust_logout_resp.permisson_error_code) << std::endl;
    }
    g_waiting_flag.store(false);
}


void TiAxTraderClient::OnRspFundQueryResult(const ATPRspFundQueryResultMsg& fund_query_result)
{
    std::cout << "fund_query_result : " << std::endl;
    json m_json_rsp = {
        { "type", "OnRspFundQueryResult"},
        { "data", {
            {"cust_id", fund_query_result.cust_id},
            {"fund_account_id", fund_query_result.fund_account_id},
            {"account_id", fund_query_result.account_id},
            {"cust_id", fund_query_result.cust_id},
            {"total_asset", fund_query_result.init_leaves_value},
            {"buying_power", fund_query_result.leaves_value},
            {"leaves_value", fund_query_result.leaves_value},
            {"init_leaves_value", fund_query_result.init_leaves_value},
            {"available_t0", fund_query_result.available_t0},
            {"available_t1", fund_query_result.available_t1},
            {"available_t2", fund_query_result.available_t2},
            {"available_t3", fund_query_result.available_t3},
            {"available_tall", fund_query_result.available_tall},
            {"frozen_all", fund_query_result.frozen_all},
            {"te_partition_no", fund_query_result.te_partition_no},
            {"credit_sell_frozen", fund_query_result.credit_sell_frozen},
            {"credit_sell_occupied", fund_query_result.credit_sell_occupied},
            {"credit_sell_pre_occupied", fund_query_result.credit_sell_pre_occupied},
            {"init_credit_sell", fund_query_result.init_credit_sell},
        }}
    };
    m_cb->OnCommonJsonRespones(&m_json_rsp, fund_query_result.client_seq_id, true, fund_query_result.query_result_code, fund_query_result.extra_data.c_str());
};

void TiAxTraderClient::OnRspShareQueryResult(const ATPRspShareQueryResultMsg& share_query_result)
{
    std::cout << "share_query_result : " << std::endl;
	std::cout << "cust_id : " << share_query_result.cust_id<<
	 " fund_account_id : " << share_query_result.fund_account_id<<
	 " account_id : " << share_query_result.account_id<<
	 " client_seq_id : " << share_query_result.client_seq_id<<
	 " query_result_code : " << share_query_result.query_result_code<<
	 " user_info : " << share_query_result.user_info<<
	 " last_index : " << share_query_result.last_index<<
	 " total_num : " << share_query_result.total_num << std::endl;
	//std::vector<APIShareUnit>::iterator it;
	int i = 1;
	for (auto it = share_query_result.order_array.begin();
		it != share_query_result.order_array.end(); it++)
	{
		std::cout << " order_array_" << i << " : " << std::endl;
		std::cout << " security_id : " << it->security_id<<
		 " security_symbol : " << it->security_symbol<<
		 " market_id : " << it->market_id<<
		 " account_id : " << it->account_id<<
		 " init_qty : " << it->init_qty<<
		 " leaves_qty : " << it->leaves_qty<<
		 " available_qty : " << it->available_qty<<
		 " profit_loss : " << it->profit_loss<<
		 " market_value : " << it->market_value<<
		 " cost_price : " << it->cost_price << std::endl;
		i++;
	}
};

void TiAxTraderClient::OnRspOrderQueryResult(const ATPRspOrderQueryResultMsg& order_query_result)
{
    std::cout << "order_query_result : " << std::endl;
	std::cout << "cust_id : " << order_query_result.cust_id<<
	 " fund_account_id : " << order_query_result.fund_account_id<<
	 " account_id : " << order_query_result.account_id<<
	 " client_seq_id : " << order_query_result.client_seq_id<<
	 " query_result_code : " << order_query_result.query_result_code<<
	 " user_info : " << order_query_result.user_info<<
	 " last_index : " << order_query_result.last_index<<
	 " total_num : " << order_query_result.total_num << std::endl;
	//std::vector<APIOrderUnit>::iterator it;
	int i = 1;
	for (auto it = order_query_result.order_array.begin();
		it != order_query_result.order_array.end(); it++)
	{
		std::cout << " order_array_" << i << " : " << std::endl;
		std::cout << " business_type : " << (int32_t)it->business_type<<
		 " security_id : " << it->security_id<<
		 " security_symbol : " << it->security_symbol<<
		 " market_id : " << it->market_id<<
		 " account_id : " << it->account_id<<
		 " side : " << it->side<<
		 " ord_type : " << it->ord_type<<
		 " ord_status : " << (int32_t)it->ord_status<<
		 " transact_time : " << it->transact_time<<
		 " order_price : " << it->order_price<<
		 " exec_price : " << it->exec_price<<
		 " order_qty : " << it->order_qty<<
		 " leaves_qty : " << it->leaves_qty<<
		 " cum_qty : " << it->cum_qty<<
		 " cl_ord_no : " << it->cl_ord_no<<
		 " order_id : " << it->order_id<<
		 " cl_ord_id : " << it->cl_ord_id<<
		 " client_seq_id : " << it->client_seq_id<<
		 " orig_cl_ord_no : " << it->orig_cl_ord_no<<
		 " frozen_trade_value : " << it->frozen_trade_value<<
		 " frozen_fee : " << it->frozen_fee<<
		 " reject_reason_code : " << it->reject_reason_code<<
		 " ord_rej_reason : " << it->ord_rej_reason << std::endl;
		i++;
	}
};

void TiAxTraderClient::OnRspTradeOrderQueryResult(const ATPRspTradeOrderQueryResultMsg& trade_order_query_result)
{
    std::cout << "trade_order_query_result : " << std::endl;
	std::cout << "cust_id : " << trade_order_query_result.cust_id<<
	 " fund_account_id : " << trade_order_query_result.fund_account_id<<
	 " account_id : " << trade_order_query_result.account_id<<
	 " client_seq_id : " << trade_order_query_result.client_seq_id<<
	 " user_info : " << trade_order_query_result.user_info <<
	 " query_result_code : " << trade_order_query_result.query_result_code <<
	 " last_index : " << trade_order_query_result.last_index<<
	 " total_num : " << trade_order_query_result.total_num << std::endl;

	int i = 1;
	for (auto it = trade_order_query_result.order_array.begin();
		it != trade_order_query_result.order_array.end(); it++)
	{
		std::cout << " order_array_" << i << " : " << std::endl;
		std::cout << " business_type : " << (int32_t)it->business_type<<
		 " security_id : " << it->security_id<<
		 " security_symbol : " << it->security_symbol<<
		 " market_id : " << it->market_id<<
		 " account_id : " << it->account_id<<
		 " side : " << it->side<<
		 " ord_type : " << it->ord_type<<
		 " exec_type : " << it->exec_type<<
		 " exec_id : " << it->exec_id<<
		 " cl_ord_no : " << it->cl_ord_no<<
		 " order_id : " << it->order_id<<
		 " cl_ord_id : " << it->cl_ord_id<<
		 " transact_time : " << it->transact_time<<
		 " last_px : " << it->last_px<<
		 " last_qty : " << it->last_qty<<
		 " total_value_traded : " << it->total_value_traded<<
		 " fee : " << it->fee << std::endl;
		i++;
	}
};


// 订单下达内部响应
void TiAxTraderClient::OnRspOrderStatusInternalAck(const ATPRspOrderStatusAckMsg& order_status_ack)
{
    std::shared_ptr<TiRtnOrderStatus> order_ptr;
    auto iter = m_order_map.find(order_status_ack.cl_ord_no);
    if (iter == m_order_map.end())
    {
        iter = m_order_req_map.find(order_status_ack.client_seq_id);
        if (iter == m_order_req_map.end())
        {
            order_ptr = std::make_shared<TiRtnOrderStatus>();
            memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
            strcpy(order_ptr->szSymbol, order_status_ack.security_symbol);
            if (order_status_ack.market_id == ATPMarketIDConst::kShangHai) {
                strcpy(order_ptr->szExchange, "SH");
            } else if (order_status_ack.market_id == ATPMarketIDConst::kShenZhen) {
                strcpy(order_ptr->szExchange, "SZ");
            } else {
                strcpy(order_ptr->szExchange, "");
            }

            switch (order_status_ack.side)
            {
            case ATPSideConst::kSell:
                order_ptr->nTradeSideType = TI_TradeSideType_Sell;
                break;
            case ATPSideConst::kBuy:
                order_ptr->nTradeSideType = TI_TradeSideType_Buy;
                break;
            case ATPSideConst::kPhysicalPurchase:  
                order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
                break;
            case ATPSideConst::kPhysicalRedemption: 
                order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
                break;
            default:
                break;
            }

            switch (order_status_ack.business_type)
            {
            case ATPBusinessTypeConst::kCashAuction:
                order_ptr->nBusinessType = TI_BusinessType_Stock;
                break;
            case ATPBusinessTypeConst::kETFRealTimePurchaseRedemption:
                order_ptr->nBusinessType = TI_BusinessType_ETF;
                break;
            default:
                break;
            }  
            
            order_ptr->nOrderPrice = order_status_ack.price;
            order_ptr->nOrderVol = order_status_ack.order_qty;

            m_order_map[order_status_ack.cl_ord_no] = order_ptr;
        }else{
            order_ptr = iter->second;
            m_order_req_map.erase(iter);
            m_order_map[order_status_ack.cl_ord_no] = order_ptr;
        }
    }else{
        order_ptr = iter->second;
    }

    order_ptr->nReqId = order_status_ack.client_seq_id;
    order_ptr->nOrderId = order_status_ack.cl_ord_no;
    strcpy(order_ptr->szOrderStreamId, order_status_ack.order_id);
    order_ptr->nSubmitVol = order_status_ack.order_qty / 100;
    order_ptr->nDealtVol = order_status_ack.cum_qty / 100;
    order_ptr->nFee = ((double)order_status_ack.frozen_fee) / 10000;
    order_ptr->nStatus = getOrderStatus(order_status_ack.ord_status);
    order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(order_status_ack.transact_time);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(order_status_ack.transact_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    if(order_status_ack.reject_reason_code)
    {
        strcpy(order_ptr->szErr, ATPTradeAPI::GetErrorInfo(-5, order_status_ack.reject_reason_code).c_str());
    }

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
}

// 订单下达交易所确认
void TiAxTraderClient::OnRspOrderStatusAck(const ATPRspOrderStatusAckMsg& order_status_ack)
{
    std::shared_ptr<TiRtnOrderStatus> order_ptr;
    auto iter = m_order_map.find(order_status_ack.cl_ord_no);
    if (iter == m_order_map.end())
    {
        iter = m_order_req_map.find(order_status_ack.client_seq_id);
        if (iter == m_order_req_map.end())
        {
            order_ptr = std::make_shared<TiRtnOrderStatus>();
            memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
            strcpy(order_ptr->szSymbol, order_status_ack.security_symbol);
            if (order_status_ack.market_id == ATPMarketIDConst::kShangHai) {
                strcpy(order_ptr->szExchange, "SH");
            } else if (order_status_ack.market_id == ATPMarketIDConst::kShenZhen) {
                strcpy(order_ptr->szExchange, "SZ");
            } else {
                strcpy(order_ptr->szExchange, "");
            }

            switch (order_status_ack.side)
            {
            case ATPSideConst::kSell:
                order_ptr->nTradeSideType = TI_TradeSideType_Sell;
                break;
            case ATPSideConst::kBuy:
                order_ptr->nTradeSideType = TI_TradeSideType_Buy;
                break;
            case ATPSideConst::kPhysicalPurchase:  
                order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
                break;
            case ATPSideConst::kPhysicalRedemption: 
                order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
                break;
            default:
                break;
            }

            switch (order_status_ack.business_type)
            {
            case ATPBusinessTypeConst::kCashAuction:
                order_ptr->nBusinessType = TI_BusinessType_Stock;
                break;
            case ATPBusinessTypeConst::kETFRealTimePurchaseRedemption:
                order_ptr->nBusinessType = TI_BusinessType_ETF;
                break;
            default:
                break;
            }  
            
            order_ptr->nOrderPrice = order_status_ack.price;
            order_ptr->nOrderVol = order_status_ack.order_qty;

            m_order_map[order_status_ack.cl_ord_no] = order_ptr;
        }else{
            order_ptr = iter->second;
            m_order_req_map.erase(iter);
            m_order_map[order_status_ack.cl_ord_no] = order_ptr;
        }
    }else{
        order_ptr = iter->second;
    }

    order_ptr->nReqId = order_status_ack.client_seq_id;
    order_ptr->nOrderId = order_status_ack.cl_ord_no;
    strcpy(order_ptr->szOrderStreamId, order_status_ack.order_id);
    order_ptr->nSubmitVol = order_status_ack.order_qty / 100;
    order_ptr->nDealtVol = order_status_ack.cum_qty / 100;
    order_ptr->nFee = ((double)order_status_ack.frozen_fee) / 10000;
    order_ptr->nStatus = getOrderStatus(order_status_ack.ord_status);
    //order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(order_status_ack.transact_time);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(order_status_ack.transact_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    if(order_status_ack.reject_reason_code)
    {
        strcpy(order_ptr->szErr, ATPTradeAPI::GetErrorInfo(-5, order_status_ack.reject_reason_code).c_str());
    }

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
    
}

// 成交回报
void TiAxTraderClient::OnRspCashAuctionTradeER(const ATPRspCashAuctionTradeERMsg& cash_auction_trade_er) 
{
    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = cash_auction_trade_er.cl_ord_no;
    strcpy(match_ptr->szStreamId, cash_auction_trade_er.exec_id);
    match_ptr->nMatchPrice = ((double)cash_auction_trade_er.last_px)/10000;
    match_ptr->nMatchVol = cash_auction_trade_er.last_qty / 100;
    strcpy(match_ptr->szSymbol, cash_auction_trade_er.security_id);

    
    if (cash_auction_trade_er.market_id == ATPMarketIDConst::kShangHai) {
        strcpy(match_ptr->szExchange, "SH");
    } else if (cash_auction_trade_er.market_id == ATPMarketIDConst::kShenZhen) {
        strcpy(match_ptr->szExchange, "SZ");
    } else {
        strcpy(match_ptr->szExchange, "");
    }

    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(cash_auction_trade_er.transact_time);
    match_ptr->nTradeSideType = cash_auction_trade_er.side == 1 ? TI_TradeSideType_Buy : TI_TradeSideType_Sell;
    m_matches_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderMatch>>(match_ptr->nOrderId, match_ptr));
    
    m_cb->OnRtnOrderMatchEvent(match_ptr.get());



    std::shared_ptr<TiRtnOrderStatus> order_ptr;
    auto iter = m_order_map.find(cash_auction_trade_er.cl_ord_no);
    if (iter == m_order_map.end())
    {
        iter = m_order_req_map.find(cash_auction_trade_er.client_seq_id);
        if (iter == m_order_req_map.end())
        {
            order_ptr = std::make_shared<TiRtnOrderStatus>();
            memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
            strcpy(order_ptr->szSymbol, cash_auction_trade_er.security_id);
            if (cash_auction_trade_er.market_id == ATPMarketIDConst::kShangHai) {
                strcpy(order_ptr->szExchange, "SH");
            } else if (cash_auction_trade_er.market_id == ATPMarketIDConst::kShenZhen) {
                strcpy(order_ptr->szExchange, "SZ");
            } else {
                strcpy(order_ptr->szExchange, "");
            }

            switch (cash_auction_trade_er.side)
            {
            case ATPSideConst::kSell:
                order_ptr->nTradeSideType = TI_TradeSideType_Sell;
                break;
            case ATPSideConst::kBuy:
                order_ptr->nTradeSideType = TI_TradeSideType_Buy;
                break;
            case ATPSideConst::kPhysicalPurchase:  
                order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
                break;
            case ATPSideConst::kPhysicalRedemption: 
                order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
                break;
            default:
                break;
            }

            switch (cash_auction_trade_er.business_type)
            {
            case ATPBusinessTypeConst::kCashAuction:
                order_ptr->nBusinessType = TI_BusinessType_Stock;
                break;
            case ATPBusinessTypeConst::kETFRealTimePurchaseRedemption:
                order_ptr->nBusinessType = TI_BusinessType_ETF;
                break;
            default:
                break;
            }  
            
            order_ptr->nOrderPrice = cash_auction_trade_er.price;
            order_ptr->nOrderVol = cash_auction_trade_er.order_qty;

            m_order_map[cash_auction_trade_er.cl_ord_no] = order_ptr;
        }else{
            order_ptr = iter->second;
            m_order_req_map.erase(iter);
            m_order_map[cash_auction_trade_er.cl_ord_no] = order_ptr;
        }
    }else{
        order_ptr = iter->second;
    }

    order_ptr->nReqId = cash_auction_trade_er.client_seq_id;
    order_ptr->nOrderId = cash_auction_trade_er.cl_ord_no;
    strcpy(order_ptr->szOrderStreamId, cash_auction_trade_er.order_id);
    order_ptr->nSubmitVol = cash_auction_trade_er.order_qty / 100;
    double _total_dealt_turnover = order_ptr->nDealtPrice * order_ptr->nDealtVol + ((double)cash_auction_trade_er.total_value_traded) / 10000;
    order_ptr->nDealtVol = cash_auction_trade_er.cum_qty / 100;
    order_ptr->nDealtPrice = _total_dealt_turnover / order_ptr->nDealtVol;
    order_ptr->nFee = ((double)cash_auction_trade_er.fee) / 10000;
    order_ptr->nStatus = getOrderStatus(cash_auction_trade_er.ord_status);
    //order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(order_status_ack.transact_time);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(cash_auction_trade_er.transact_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    if(cash_auction_trade_er.reject_reason_code)
    {
        strcpy(order_ptr->szErr, ATPTradeAPI::GetErrorInfo(-5, cash_auction_trade_er.reject_reason_code).c_str());
    }

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
};

void TiAxTraderClient::OnRspETFRedemptionTradeER(const ATPRspETFRedemptionTradeERMsg& etf_redemption_trade_er)
{
    std::shared_ptr<TiRtnOrderStatus> order_ptr;
    auto iter = m_order_map.find(etf_redemption_trade_er.cl_ord_no);
    if (iter == m_order_map.end())
    {
        iter = m_order_req_map.find(etf_redemption_trade_er.client_seq_id);
        if (iter == m_order_req_map.end())
        {
            order_ptr = std::make_shared<TiRtnOrderStatus>();
            memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
            strcpy(order_ptr->szSymbol, etf_redemption_trade_er.security_id);
            if (etf_redemption_trade_er.market_id == ATPMarketIDConst::kShangHai) {
                strcpy(order_ptr->szExchange, "SH");
            } else if (etf_redemption_trade_er.market_id == ATPMarketIDConst::kShenZhen) {
                strcpy(order_ptr->szExchange, "SZ");
            } else {
                strcpy(order_ptr->szExchange, "");
            }

            switch (etf_redemption_trade_er.side)
            {
            case ATPSideConst::kSell:
                order_ptr->nTradeSideType = TI_TradeSideType_Sell;
                break;
            case ATPSideConst::kBuy:
                order_ptr->nTradeSideType = TI_TradeSideType_Buy;
                break;
            case ATPSideConst::kPhysicalPurchase:  
                order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
                break;
            case ATPSideConst::kPhysicalRedemption: 
                order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
                break;
            default:
                break;
            }

            switch (etf_redemption_trade_er.business_type)
            {
            case ATPBusinessTypeConst::kCashAuction:
                order_ptr->nBusinessType = TI_BusinessType_Stock;
                break;
            case ATPBusinessTypeConst::kETFRealTimePurchaseRedemption:
                order_ptr->nBusinessType = TI_BusinessType_ETF;
                break;
            default:
                break;
            }  
            
            order_ptr->nOrderPrice = etf_redemption_trade_er.price;
            order_ptr->nOrderVol = etf_redemption_trade_er.order_qty;

            m_order_map[etf_redemption_trade_er.cl_ord_no] = order_ptr;
        }else{
            order_ptr = iter->second;
            m_order_req_map.erase(iter);
            m_order_map[etf_redemption_trade_er.cl_ord_no] = order_ptr;
        }
    }else{
        order_ptr = iter->second;
    }

    order_ptr->nReqId = etf_redemption_trade_er.client_seq_id;
    order_ptr->nOrderId = etf_redemption_trade_er.cl_ord_no;
    strcpy(order_ptr->szOrderStreamId, etf_redemption_trade_er.order_id);
    order_ptr->nSubmitVol = etf_redemption_trade_er.order_qty / 100;
    order_ptr->nDealtVol = etf_redemption_trade_er.cum_qty / 100;
    order_ptr->nStatus = getOrderStatus(etf_redemption_trade_er.ord_status);
    //order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(order_status_ack.transact_time);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(etf_redemption_trade_er.transact_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    if(etf_redemption_trade_er.reject_reason_code)
    {
        strcpy(order_ptr->szErr, ATPTradeAPI::GetErrorInfo(-5, etf_redemption_trade_er.reject_reason_code).c_str());
    }

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
};

// 订单下达内部拒绝
void TiAxTraderClient::OnRspBizRejection(const ATPRspBizRejectionOtherMsg& biz_rejection)
{
    std::cout << "biz_rejection : " << std::endl;
    std::cout << "transact_time : " << biz_rejection.transact_time <<
        " client_seq_id : " << biz_rejection.client_seq_id <<
        " msg_type : " << biz_rejection.api_msg_type <<
        " reject_reason_code : " << biz_rejection.reject_reason_code <<
        " business_reject_text : " << biz_rejection.business_reject_text <<
        " user_info : " << biz_rejection.user_info << std::endl;
};

////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiAxTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations   = string(_iniFile["ti_ax_trader_client"]["locations"]);
    m_config->szLocations2   = string(_iniFile["ti_ax_trader_client"]["locations2"]);
    
    m_config->szUser                = string(_iniFile["ti_ax_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_ax_trader_client"]["pass"]);

    m_config->szSoftwareName        = string(_iniFile["ti_ax_trader_client"]["software_name"]);
    m_config->szSoftwareVersion     = string(_iniFile["ti_ax_trader_client"]["software_version"]);

    m_config->szBranchCode     = string(_iniFile["ti_ax_trader_client"]["branch_code"]);
    m_config->szCustomerId     = string(_iniFile["ti_ax_trader_client"]["customer_id"]);
    m_config->szFundAccount     = string(_iniFile["ti_ax_trader_client"]["fund_account"]);
    m_config->szFundPass     = string(_iniFile["ti_ax_trader_client"]["fund_pass"]);
    m_config->szShareholderIdSH     = string(_iniFile["ti_ax_trader_client"]["shareholder_id_sh"]);
    m_config->szShareholderIdSZ     = string(_iniFile["ti_ax_trader_client"]["shareholder_id_sz"]);

    
    if( m_config->szLocations.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty() |
        m_config->szBranchCode.empty() |
        m_config->szCustomerId.empty() |
        m_config->szFundAccount.empty() |
        m_config->szFundPass.empty() |
        m_config->szShareholderIdSH.empty() |
        m_config->szShareholderIdSZ.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

bool TiAxTraderClient::init_encrypt()
{
    const std::string station_name = ""; // 站点信息，该字段已经不使用
    const std::string cfg_path=".";      // 配置文件路径
    const std::string log_dir_path = ""; // 日志路径
    bool record_all_flag = true;         // 是否记录所有委托信息
    std::unordered_map<std::string,std::string> encrypt_cfg; // 加密库配置
    bool connection_retention_flag=false;   // 是否启用会话保持

    // encrypt_cfg参数填写：
    encrypt_cfg["ENCRYPT_SCHEMA"]="1";              // 字符 0 表示 不对消息中的所有 password 加密
    encrypt_cfg["ATP_ENCRYPT_PASSWORD"]="";         // 除登入及密码修改外其他消息的密码字段加密算法
    encrypt_cfg["ATP_LOGIN_ENCRYPT_PASSWORD"]="";   // 登入及密码修改消息中密码字段的加密算法so路径
    encrypt_cfg["GM_SM2_PUBLIC_KEY_PATH"]="";       // 采用国密算法时，通过该key配置 GM算法配置加密使用的公钥路径
    encrypt_cfg["RSA_PUBLIC_KEY_PATH"]="./rsa_public_key.pem";          // 如果使用rsa算法加密，通过该key配置 rsa算法配置加密使用的公钥路径

    ATPRetCodeType ec = ATPTradeAPI::Init(station_name, 
        cfg_path,
        log_dir_path,
        record_all_flag,
        encrypt_cfg,
        connection_retention_flag);
    if (ec != ErrorCode::kSuccess)
    {
        std::cout << "Init encrypt failed: " << ec <<std::endl;
        return false;
    }
    return true;
};

ATPRetCodeType TiAxTraderClient::connect(ATPTradeAPI* client, ATPTradeHandler* handler)
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return ErrorCode::kFailure;
    }
    // 设置连接信息
    ATPConnectProperty prop;
    prop.user = m_config->szUser;                                   // 网关用户名
    prop.password = m_config->szPass;                               // 网关用户密码
    if(m_config->szLocations2.empty()){
        prop.locations = { m_config->szLocations };                     // 网关主备节点的地址+端口
    }else{
        prop.locations = { m_config->szLocations, m_config->szLocations2};                     // 网关主备节点的地址+端口
    }
    prop.heartbeat_interval_milli = 5000;                           // 发送心跳的时间间隔，单位：毫秒
    prop.connect_timeout_milli = 5000;                              // 连接超时时间，单位：毫秒
    prop.reconnect_time = 10;                                       // 重试连接次数
    prop.client_name = m_config->szSoftwareName;                    // 客户端程序名字
    prop.client_version = m_config->szSoftwareVersion;              // 客户端程序版本
    prop.report_sync = m_report_sync;                               // 回报同步数据分区号+序号，首次是空，断线重连时填入的是接受到的最新分区号+序号
    prop.mode = 0;                                                  // 模式0-同步回报模式，模式1-快速登录模式，不同步回报

    // 建立连接
    while (!g_connected_flag.load())
    {
        // 在连接中
        if (g_waiting_flag.load())
        {
            sleep(0);
        }
        else
        {
            g_waiting_flag.store(true);
            // 建立连接
            ATPRetCodeType ec = client->Connect(prop, handler);
            if (ec != ErrorCode::kSuccess)
            {
                std::cout << "Invoke Connect error:" << ec << std::endl;
                return ec;
            }
            sleep(0);
        }
    }

    return kSuccess;
};

ATPRetCodeType TiAxTraderClient::close(ATPTradeAPI* client)
{
    g_waiting_flag.store(true);
    ATPRetCodeType ec = client->Close();
    if (ec != ErrorCode::kSuccess)
    {
        std::cout << "Invoke Close error:" << ec << std::endl;
        return ec;
    }

    while(g_waiting_flag.load())
    {
        sleep(0);
    }
    return ErrorCode::kSuccess;
};

ATPRetCodeType TiAxTraderClient::login(ATPTradeAPI* client)
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return ErrorCode::kFailure;
    }
    // 设置登入消息
    ATPReqCustLoginOtherMsg login_msg;
    strncpy(login_msg.fund_account_id, m_config->szFundAccount.c_str(), 17);             // 资金账户ID
    strncpy(login_msg.password, m_config->szFundPass.c_str(), 129);                       // 客户号密码
    login_msg.login_mode = ATPCustLoginModeType::kFundAccountIDMode;   // 登录模式，资金账号登录
    login_msg.client_seq_id = g_client_seq_id++;                // 客户系统消息号
    login_msg.order_way = '0';                                  // 委托方式，自助委托
    login_msg.client_feature_code = g_client_feature_code;      // 终端识别码

    g_waiting_flag.store(true);
    ATPRetCodeType ec = client->ReqCustLoginOther(&login_msg);
    if (ec != ErrorCode::kSuccess)
    {
        std::cout << "Invoke CustLogin error:" << ec << std::endl;
        return ec;
    }

    while(g_waiting_flag.load())
    {
        sleep(0);
    }

    return ErrorCode::kSuccess;
};

ATPRetCodeType TiAxTraderClient::logout(ATPTradeAPI* client)
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return ErrorCode::kFailure;
    }
    // 设置登出消息
    ATPReqCustLogoutOtherMsg logout_msg;
    strncpy(logout_msg.fund_account_id, m_config->szFundAccount.c_str(), 17);             // 资金账户ID
    logout_msg.client_seq_id = g_client_seq_id++;               // 客户系统消息号
    logout_msg.client_feature_code = g_client_feature_code;      // 终端识别码

    g_waiting_flag.store(true);
    ATPRetCodeType ec = client->ReqCustLogoutOther(&logout_msg);
    if (ec != ErrorCode::kSuccess)
    {
        std::cout << "Invoke CustLogout error:" << ec << std::endl;
        return ec;
    }

    while(g_waiting_flag.load())
    {
        sleep(0);
    }

    return ErrorCode::kSuccess;
};

bool TiAxTraderClient::init(ATPTradeAPI* client, ATPTradeHandler* handler)
{
    // 建立连接
    if (connect(client, handler) != ErrorCode::kSuccess)
    {
        return false;
    }

    // 登录
    if (login(client) != ErrorCode::kSuccess)
    {
        return false;
    }
    
    // 检查是否登录成功
    if (!g_cust_logined_flag.load())
    {
        return false;
    }

    return true;
};

TI_OrderStatusType TiAxTraderClient::getOrderStatus(ATPOrdStatusType status)
{
    switch (status)
    {
    case ATPOrdStatusConst::kNew:
        return TI_OrderStatusType_unAccept;
    case ATPOrdStatusConst::kPartiallyFilled:
        return TI_OrderStatusType_dealt;
    case ATPOrdStatusConst::kFilled:
        return TI_OrderStatusType_dealt;
    case ATPOrdStatusConst::kPartiallyFilledPartiallyCancelled:
        return TI_OrderStatusType_removed;
    case ATPOrdStatusConst::kCancelled:
        return TI_OrderStatusType_removed;
    case ATPOrdStatusConst::kPartiallyCancelled:
        return TI_OrderStatusType_removed;
    case ATPOrdStatusConst::kReject:
        return TI_OrderStatusType_fail;
    case ATPOrdStatusConst::kUnSend:
        return TI_OrderStatusType_unAccept;
    case ATPOrdStatusConst::kSended:
        return TI_OrderStatusType_queued;
    case ATPOrdStatusConst::kWaitCancelled:
        return TI_OrderStatusType_queued;
    case ATPOrdStatusConst::kPartiallyFilledWaitCancelled:
        return TI_OrderStatusType_removing;
    case ATPOrdStatusConst::kProcessed:
        return TI_OrderStatusType_queued;
    default:
        return TI_OrderStatusType_unAccept;
    }
   return TI_OrderStatusType_unAccept;
};

int TiAxTraderClient::orderInsertStock(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;
    ATPReqCashAuctionOrderMsg msg;
    strncpy(msg.security_id, req->szSymbol, 9);                   // 证券代码
    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    if (!strcmp(req->szExchange, "SH"))
    {
        msg.market_id = ATPMarketIDConst::kShangHai;             // 市场ID，上海
        strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
    }
    if (!strcmp(req->szExchange, "SZ"))
    {
        msg.market_id = ATPMarketIDConst::kShenZhen;             // 市场ID，上海
        strncpy(msg.account_id, m_config->szShareholderIdSZ.c_str(), 13);               // 账户ID
    }
    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Sell:
        msg.side = ATPSideConst::kSell;   
        break;
    case TI_TradeSideType_Buy:
        msg.side = ATPSideConst::kBuy;   
        break;
    default:
        msg.side = ATPSideConst::kDefault;
        break;
    }
    msg.order_type = ATPOrdTypeConst::kFixedNew;             // 订单类型，限价
    msg.price = req->nOrderPrice * 10000;                    // 委托价格 N13(4)，21.0000元
    msg.order_qty = req->nOrderVol * 100;                    // (需要 * 100才是股数) 申报数量N15(2)；股票为股、基金为份、上海债券默认为张（使用时请务必与券商确认），其他为张；1000.00股
    msg.client_seq_id = nReqId;                              // 用户系统消息序号
    msg.order_way = '0';                                     // 委托方式，自助委托
    //msg.order_way = 'R';                                     // 委托方式，自助委托
    strncpy(msg.password, "password1", 129);                 // 客户密码
    msg.client_feature_code = g_client_feature_code;         // 终端识别码

    std::shared_ptr<TiRtnOrderStatus> order_ptr = std::make_shared<TiRtnOrderStatus>();
    memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
    memcpy(order_ptr.get(), req, sizeof(TiReqOrderInsert));

    m_order_req_map[order_ptr->nReqId] = order_ptr;

    ATPRetCodeType ec = m_client->ReqCashAuctionOrder(&msg);
    if (ec != ErrorCode::kSuccess)
    {
        std::cout << "Invoke Send error:" << ec << std::endl;
    }

    return nReqId;
};
int TiAxTraderClient::orderInsertEtf(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;
	ATPReqETFRedemptionOrderMsg msg;

	strncpy(msg.security_id, req->szSymbol,9);
    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    if (!strcmp(req->szExchange, "SH"))
    {
        msg.market_id = ATPMarketIDConst::kShangHai;             // 市场ID，上海
        strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
    }
    if (!strcmp(req->szExchange, "SZ"))
    {
        msg.market_id = ATPMarketIDConst::kShenZhen;             // 市场ID，上海
        strncpy(msg.account_id, m_config->szShareholderIdSZ.c_str(), 13);               // 账户ID
    }

    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Purchase:
        msg.side = ATPSideConst::kPurchase;   
        break;
    case TI_TradeSideType_Redemption:
        msg.side = ATPSideConst::kRedeem;   
        break;
    default:
        msg.side = ATPSideConst::kDefault;
        break;
    }

	msg.order_qty = req->nOrderVol * 100;                    // (需要 * 100才是股数) 申报数量N15(2)；股票为股、基金为份、上海债券默认为张（使用时请务必与券商确认），其他为张；1000.00股
    msg.client_seq_id = nReqId;                              // 用户系统消息序号
    msg.order_way = '0';                                     // 委托方式，自助委托
    
    strncpy(msg.password, "password1", 129);                 // 客户密码

    ATPRetCodeType ec = m_client->ReqETFRedemptionOrder(&msg);
    if (ec != ErrorCode::kSuccess)
    {
        std::cout << "Invoke Send error:" << ec << std::endl;
    }

    return nReqId;
};

void TiAxTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    if (!init_encrypt())      //如果初始化失败就直接退出
    {
        return;
    }
    if (m_client)   //如果已经实例了就退出
    {
        return;
    }
    m_client = new ATPTradeAPI();
    
    // 建立连接
    if (init(m_client, this))
    {
        std::cout << "Wait for ack. press enter for exit." << std::endl;
    }
};

int TiAxTraderClient::orderInsert(TiReqOrderInsert* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    if(req->nBusinessType == TI_BusinessType_Stock){
        return orderInsertStock(req);
    }else if(req->nBusinessType == TI_BusinessType_ETF){
        return orderInsertEtf(req);
    }
    return -1;
};

int TiAxTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

TiRtnOrderStatus* TiAxTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    if (m_order_map.find(order_id) != m_order_map.end())
    {
        return m_order_map[order_id].get();
    }
    if (m_order_req_map.find(req_id) != m_order_map.end())
    {
        return m_order_req_map[req_id].get();
    }
    return NULL;
};
    

int TiAxTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    ATPClientSeqIDType seq_id = ++nReqId;;
	ATPReqFundQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);

    
    //strncpy(msg.account_id, m_config->szShareholderIdSZ.c_str(), 13);               // 账户ID

    m_client->ReqFundQuery(&msg);

    return nReqId;
};

int TiAxTraderClient::QueryOrders()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    ATPClientSeqIDType seq_id = ++nReqId;;
	ATPReqOrderQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);

    m_client->ReqOrderQuery(&msg);

    return nReqId;
};

int TiAxTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    ATPClientSeqIDType seq_id = ++nReqId;;
	ATPReqTradeOrderQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);

    m_client->ReqTradeOrderQuery(&msg);

    return nReqId;
};

int TiAxTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    ATPClientSeqIDType seq_id = ++nReqId;;
	ATPReqShareQueryMsg msg;
	
    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);

	//APIAccountIDUnit api_account_unit;
	//strncpy(api_account_unit.account_id,account_id.c_str(),13);
	//msg.account_id_array.push_back(api_account_unit);

    m_client->ReqShareQuery(&msg);

    return nReqId;
};