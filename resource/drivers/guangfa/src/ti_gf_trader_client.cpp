#include "ti_gf_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"

#include <atomic>


std::atomic_bool g_connected_flag(false);           // 已连接
std::atomic_bool g_cust_logined_flag(false);        // 已登录

std::atomic_bool g_waiting_flag(false);             // 等待响应

ATPClientSeqIDType g_client_seq_id = 1;             //  客户系统消息号
ATPClientFeatureCodeType g_client_feature_code = "OH;IIP=60.216.86.190;IPORT=20184;LIP=192.168.124.5;MAC=3448EDF6B29C;HD=64cd98f01b78d800289a9a0c706b7052@QuantStage.Moneta;v1.0.1;TAIMENGLI;QuantStage.Oceanus.v2.0.3;192.168.124.5;121.8.190.120";  // 终端识别码，由券商指定规则

TiGfTraderClient::TiGfTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_client = NULL;

    nSessionId = 0;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = datetime::get_time_sec_num()*100;   //跳过xtp client设置成交模式的区段
};


TiGfTraderClient::~TiGfTraderClient()
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

void TiGfTraderClient::OnLogin(const std::string& reason)
{
    LOG(INFO) << "OnLogin Recv:" << reason << std::endl;
    g_connected_flag.store(true);
}

// 登出回调
void TiGfTraderClient::OnLogout(const std::string& reason)
{
    LOG(INFO) << "OnLogout Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接失败
void TiGfTraderClient::OnConnectFailure(const std::string &reason)
{
    LOG(INFO) << "OnConnectFailure Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接超时
void TiGfTraderClient::OnConnectTimeOut(const std::string &reason)
{
    LOG(INFO) << "OnConnectTimeOut Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 心跳超时
void TiGfTraderClient::OnHeartbeatTimeout(const std::string &reason)
{
    LOG(INFO) << "OnHeartbeatTimeout Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接关闭
void TiGfTraderClient::OnClosed(const std::string &reason)
{
    LOG(INFO) << "OnClosed Recv:" << reason << std::endl;
    g_connected_flag.store(false);
    g_cust_logined_flag.store(false);
}

// 连接结束回调
void TiGfTraderClient::OnEndOfConnection(const std::string& reason)
{
    LOG(INFO) << "OnEndOfConnection Recv:" << reason << std::endl;
    g_waiting_flag.store(false);
}

// 客户号登入回调
void TiGfTraderClient::OnRspCustLoginResp(const ATPRspCustLoginRespOtherMsg &cust_login_resp)
{
    LOG(INFO) << "OnRspCustLoginResp Recv:" << static_cast<uint32_t>(cust_login_resp.permisson_error_code) << std::endl;
    if (cust_login_resp.permisson_error_code == 0)
    {
        g_cust_logined_flag.store(true);
        LOG(INFO) << "CustLogin Success!" << std::endl;
    }
    else
    {
        LOG(INFO) << "CustLogin Fail, permisson_error_code :" << static_cast<uint32_t>(cust_login_resp.permisson_error_code) << std::endl;
    }
    
    
    g_waiting_flag.store(false);
}

// 客户号登出回调
void TiGfTraderClient::OnRspCustLogoutResp(const ATPRspCustLogoutRespOtherMsg &cust_logout_resp)
{
    LOG(INFO) << "OnRspCustLogoutResp Recv:" << static_cast<uint32_t>(cust_logout_resp.permisson_error_code) << std::endl;
    if (cust_logout_resp.permisson_error_code == 0)
    {
        g_cust_logined_flag.store(false);
        LOG(INFO) << "CustLogout Success!" << std::endl;
    }
    else
    {
        LOG(INFO) << "CustLogou Fail permisson_error_code :" << static_cast<uint32_t>(cust_logout_resp.permisson_error_code) << std::endl;
    }
    g_waiting_flag.store(false);
}


void TiGfTraderClient::OnRspFundQueryResult(const ATPRspFundQueryResultMsg& fund_query_result)
{
    LOG(INFO) << "fund_query_result : " << std::endl;
    TiRspAccountInfo account_info = {0};
    strcpy(account_info.szAccount, fund_query_result.fund_account_id);
    //strcpy(account_info.szName, "");
    //account_info.nNav = 0;
    account_info.nBalance = fund_query_result.leaves_value;
    account_info.nAvailable = fund_query_result.available_tall;
    account_info.nAssureAsset = fund_query_result.init_leaves_value;
    //account_info.nCommission = 0;
    //account_info.nDaysProfit = fund_query_result.m_dDaysProfit;
    //account_info.nPositionProfit = fund_query_result.m_dPositionProfit;
    //account_info.nCloseProfit = fund_query_result.m_dCloseProfit;
    //account_info.nCredit = fund_query_result.m_dCredit;
    //account_info.nStockValue = fund_query_result.m_dStockValue;
    //account_info.nFundValue = fund_query_result.m_dFundValue;
    m_cb->OnRspAccountInfo(&account_info);

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
    //LOG(INFO) << "OnRspFundQueryResult:"<< m_json_rsp.dump() << std::endl;
    m_cb->OnCommonJsonRespones(&m_json_rsp, fund_query_result.client_seq_id, true, fund_query_result.query_result_code, fund_query_result.extra_data.c_str());
};

void TiGfTraderClient::OnRspShareQueryResult(const ATPRspShareQueryResultMsg& share_query_result)
{
    LOG(INFO) << "share_query_result : " << std::endl; 
	LOG(INFO) << "cust_id : " << share_query_result.cust_id<<
	 " fund_account_id : " << share_query_result.fund_account_id<<
	 " account_id : " << share_query_result.account_id<<
	 " client_seq_id : " << share_query_result.client_seq_id<<
	 " query_result_code : " << share_query_result.query_result_code<<
	 " user_info : " << share_query_result.user_info<<
	 " last_index : " << share_query_result.last_index<<
	 " total_num : " << share_query_result.total_num << std::endl;
	//std::vector<APIShareUnit>::iterator it;

	size_t i = 1;
	for (auto it = share_query_result.order_array.begin();
		it != share_query_result.order_array.end(); it++)
	{
        TiRspQryPosition position;
        memset(&position, 0, sizeof(TiRspQryPosition));
        strcpy(position.szSymbol, it->security_id);
        strcpy(position.szName, it->security_symbol);
        if (it->market_id == ATPMarketIDConst::kShangHai) {
            strcpy(position.szExchange, "SH");
        } else if (it->market_id == ATPMarketIDConst::kShenZhen) {
            strcpy(position.szExchange, "SZ");
        } else {
            strcpy(position.szExchange, "");
        }
        strcpy(position.szAccount, it->fund_account_id);
        position.nInitVol = it->init_qty / 100;
        position.nLeavesVol = it->leaves_qty / 100;
        position.nAvailableVol = it->available_qty / 100;
        position.nMarketValue = (double)it->market_value / 10000;
        position.nPrice = (double)it->cost_price / 10000;
        position.nProfit = (double)it->profit_loss / 10000;
        position.nSettledProfit = 0;
        strcpy(position.szShareholderId, it->account_id);

        if (m_cb)
        {
            bool is_last = i == share_query_result.order_array.size();
            if (is_last)
            {
                is_last = share_query_result.last_index >= (share_query_result.total_num - 1);
            }
            m_cb->OnRspQryPosition(&position, is_last);
        }

		i++;
	}

    if (share_query_result.last_index < (share_query_result.total_num - 1))
    {
        queryPositions(share_query_result.last_index);
    }else{
        LOG(INFO) << "QueryPositions Done!" << std::endl;
        std::cout << "QueryPositions Done!" << std::endl;
    }
};

void TiGfTraderClient::OnRspOrderQueryResult(const ATPRspOrderQueryResultMsg& order_query_result)
{
    LOG(INFO) << "[OnRspOrderQueryResult]" << 
        " cust_id : " << order_query_result.cust_id<<
        " fund_account_id : " << order_query_result.fund_account_id<<
        " account_id : " << order_query_result.account_id<<
        " client_seq_id : " << order_query_result.client_seq_id<<
        " query_result_code : " << order_query_result.query_result_code<<
        " user_info : " << order_query_result.user_info<<
        " last_index : " << order_query_result.last_index<<
        " total_num : " << order_query_result.total_num << std::endl;
	//std::vector<APIOrderUnit>::iterator it;
	size_t i = 1;
	for (auto it = order_query_result.order_array.begin();
		it != order_query_result.order_array.end(); it++)
	{
        std::shared_ptr<TiRtnOrderStatus> order_ptr;
        auto iter = m_order_map.find(it->cl_ord_no);
        if (iter == m_order_map.end())
        {
            order_ptr = std::make_shared<TiRtnOrderStatus>();
            memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
        }else{
            order_ptr = iter->second;
        }

        strcpy(order_ptr->szSymbol, it->security_id);
        strcpy(order_ptr->szName, it->security_symbol);
        if (it->market_id == ATPMarketIDConst::kShangHai) {
            strcpy(order_ptr->szExchange, "SH");
        } else if (it->market_id == ATPMarketIDConst::kShenZhen) {
            strcpy(order_ptr->szExchange, "SZ");
        } else {
            strcpy(order_ptr->szExchange, "");
        }

        switch (it->side)
        {
        case ATPSideConst::kSell:
            order_ptr->nTradeSideType = TI_TradeSideType_Sell;
            order_ptr->nBusinessType = TI_BusinessType_Stock;
            break;
        case ATPSideConst::kBuy:
            order_ptr->nTradeSideType = TI_TradeSideType_Buy;
            order_ptr->nBusinessType = TI_BusinessType_Stock;
            break;
        case ATPSideConst::kPurchase:
            order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
            order_ptr->nBusinessType = TI_BusinessType_ETF;
            break;
        case ATPSideConst::kRedeem:
            order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
            order_ptr->nBusinessType = TI_BusinessType_ETF;
            break;
        default:
            break;
        }

        order_ptr->nOrderPrice = (double)it->order_price / 10000;
        order_ptr->nOrderVol = it->order_qty / 100;
        order_ptr->nDealtVol = it->cum_qty / 100;
        order_ptr->nDealtPrice = (double)it->exec_price / 10000;
        
        order_ptr->nReqId = it->client_seq_id;
        order_ptr->nOrderId = it->cl_ord_no;
        if(strcmp(it->order_id, "") != 0)
        {
            strcpy(order_ptr->szOrderStreamId, it->order_id);
        }
        strcpy(order_ptr->szAccount, order_query_result.fund_account_id);
        order_ptr->nSubmitVol = it->order_qty / 100;
        order_ptr->nFee = ((double)it->frozen_fee) / 10000;
        order_ptr->nStatus = getOrderStatus(it->exec_type);
        order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(it->transact_time);
        order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(it->transact_time);

        updateOrderMap(order_ptr);
        if (m_cb)
        {
            bool is_last = i == order_query_result.order_array.size();
            if (is_last)
            {
                is_last = order_query_result.last_index >= (order_query_result.total_num - 1);
            }
            m_cb->OnRspQryOrder(order_ptr.get(), is_last);
        }

		i++;
	}

    if (order_query_result.last_index < (order_query_result.total_num - 1))
    {
        queryOrders(order_query_result.last_index);
    }else{
        LOG(INFO) << "QueryOrders Done!" << std::endl;
        std::cout << "QueryOrders Done!" << std::endl;
    }
};

void TiGfTraderClient::OnRspEtfTradeOrderQueryResult(const ATPRspETFTradeOrderQueryResultMsg &trade_order_query_result)
{
    LOG(INFO) << "[OnRspEtfTradeOrderQueryResult]" <<
	    " cust_id : " << trade_order_query_result.cust_id<<
        " fund_account_id : " << trade_order_query_result.fund_account_id<<
        " account_id : " << trade_order_query_result.account_id<<
        " client_seq_id : " << trade_order_query_result.client_seq_id<<
        " user_info : " << trade_order_query_result.user_info <<
        " query_result_code : " << trade_order_query_result.query_result_code <<
        " last_index : " << trade_order_query_result.last_index<<
        " total_num : " << trade_order_query_result.total_num << std::endl;

    std::vector<std::shared_ptr<TiRtnOrderMatch>> match_array;

	for (auto it = trade_order_query_result.order_array.begin();
		it != trade_order_query_result.order_array.end(); it++)
    {
        if (it->exec_type != ATPExecTypeConst::kTrade)
        {
            continue;
        }

        for(auto iter = it->order_array.begin(), end = it->order_array.end(); iter != end; iter++)
        {
            std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
            memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));

            strcpy(match_ptr->szSymbol, iter->security_id);

            if (iter->market_id == ATPMarketIDConst::kShangHai) {
                strcpy(match_ptr->szExchange, "SH");
            } else if (iter->market_id == ATPMarketIDConst::kShenZhen) {
                strcpy(match_ptr->szExchange, "SZ");
            } else {
                strcpy(match_ptr->szExchange, "");
            }

            switch (it->side)
            {
            case ATPSideConst::kPurchase:
                match_ptr->nTradeSideType = TI_TradeSideType_Purchase;
                break;
            case ATPSideConst::kRedeem:
                match_ptr->nTradeSideType = TI_TradeSideType_Redemption;
                break;
            default:
                continue;
            }
           
            match_ptr->nMatchVol = iter->qty / 100;
            match_ptr->nMatchPrice = (double)iter->price / 10000;

            match_ptr->nOrderId = it->cl_ord_no;
            strcpy(match_ptr->szOrderStreamId, it->order_id);
            sprintf(match_ptr->szStreamId, "%s%s", it->exec_id, iter->security_id);
            strcpy(match_ptr->szAccount, trade_order_query_result.fund_account_id);

            match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(it->transact_time);

            match_array.push_back(match_ptr);
        }
    }

    size_t i = 1;
    for (auto it = match_array.begin(); it != match_array.end(); it++)
    {
        updateOrderMatch(*it);

        if (m_cb)
        {
            bool is_last = i == match_array.size();
            if (is_last)
            {
                is_last = trade_order_query_result.last_index >= (trade_order_query_result.total_num - 1);
            }
            m_cb->OnRspQryMatch(it->get(), is_last);
        }
        i++;
    }

    if (trade_order_query_result.last_index < (trade_order_query_result.total_num - 1))
    {
        queryEtfMatches(trade_order_query_result.last_index);
    }else{
        LOG(INFO) << "QueryEtfMatches Done!" << std::endl;
        std::cout << "QueryEtfMatches Done!" << std::endl;
    }
};

void TiGfTraderClient::OnRspTradeOrderQueryResult(const ATPRspTradeOrderQueryResultMsg& trade_order_query_result)
{
    LOG(INFO) << "[OnRspTradeOrderQueryResult]" <<
	    " cust_id : " << trade_order_query_result.cust_id<<
        " fund_account_id : " << trade_order_query_result.fund_account_id<<
        " account_id : " << trade_order_query_result.account_id<<
        " client_seq_id : " << trade_order_query_result.client_seq_id<<
        " user_info : " << trade_order_query_result.user_info <<
        " query_result_code : " << trade_order_query_result.query_result_code <<
        " last_index : " << trade_order_query_result.last_index<<
        " total_num : " << trade_order_query_result.total_num << std::endl;

    std::vector<std::shared_ptr<TiRtnOrderMatch>> match_array;

	for (auto it = trade_order_query_result.order_array.begin();
		it != trade_order_query_result.order_array.end(); it++)
    {
        if (it->exec_type != ATPExecTypeConst::kTrade)
        {
            continue;
        }
        
        std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
        memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));

        strcpy(match_ptr->szSymbol, it->security_id);
        strcpy(match_ptr->szName, it->security_symbol);
        if (it->market_id == ATPMarketIDConst::kShangHai) {
            strcpy(match_ptr->szExchange, "SH");
        } else if (it->market_id == ATPMarketIDConst::kShenZhen) {
            strcpy(match_ptr->szExchange, "SZ");
        } else {
            strcpy(match_ptr->szExchange, "");
        }

        switch (it->side)
        {
        case ATPSideConst::kSell:
            match_ptr->nTradeSideType = TI_TradeSideType_Sell;
            break;
        case ATPSideConst::kBuy:
            match_ptr->nTradeSideType = TI_TradeSideType_Buy;
            break;
        default:
            continue;
        }

        match_ptr->nMatchVol = it->last_qty / 100;
        match_ptr->nMatchPrice = ((double)it->total_value_traded / 10000) / (it->last_qty / 100);
        match_ptr->nFee = ((double)it->fee) / 10000;
        match_ptr->nReqId = it->client_seq_id;
        match_ptr->nOrderId = it->cl_ord_no;
        /*
        if(strcmp(it->order_id, "") != 0)
        {
            strcpy(match_ptr->szOrderStreamId, it->order_id);
        }
        */
        strcpy(match_ptr->szOrderStreamId, it->order_id);
        strcpy(match_ptr->szStreamId, it->exec_id);
        strcpy(match_ptr->szAccount, trade_order_query_result.fund_account_id);
        match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(it->transact_time);
    
        match_array.push_back(match_ptr);
	}

	size_t i = 1;
    for (auto it = match_array.begin(); it != match_array.end(); it++)
    {
        updateOrderMatch(*it);

        if (m_cb)
        {
            bool is_last = i == match_array.size();
            if (is_last)
            {
                is_last = trade_order_query_result.last_index >= (trade_order_query_result.total_num - 1);
            }
            m_cb->OnRspQryMatch(it->get(), is_last);
        }
        i++;
    }

    
    if (trade_order_query_result.last_index < (trade_order_query_result.total_num - 1))
    {
        queryMatches(trade_order_query_result.last_index);
    }else{
        LOG(INFO) << "QueryMatches Done!" << std::endl;
        std::cout << "QueryMatches Done!" << std::endl;
    }
};


// 订单下达内部响应
void TiGfTraderClient::OnRspOrderStatusInternalAck(const ATPRspOrderStatusAckMsg& order_status_ack)
{
    LOG(INFO) << "[OnRspOrderStatusInternalAck]: "
        << " partition: " << order_status_ack.partition 
        << " index: " << order_status_ack.index 
        << " business_type: " << order_status_ack.business_type 
        << " cl_ord_no: " << order_status_ack.cl_ord_no 
        << " security_id: " << order_status_ack.security_id 
        << " market_id: " << order_status_ack.market_id 
        << " exec_type: " << order_status_ack.exec_type 
        << " ord_status: " << order_status_ack.ord_status 
        << " cust_id: " << order_status_ack.cust_id 
        << " fund_account_id: " << order_status_ack.fund_account_id 
        << " account_id: " << order_status_ack.account_id 
        << " price: " << order_status_ack.price 
        << " order_qty: " << order_status_ack.order_qty 
        << " leaves_qty: " << order_status_ack.leaves_qty 
        << " cum_qty: " << order_status_ack.cum_qty 
        << " side: " << order_status_ack.side 
        << " transact_time: " << order_status_ack.transact_time 
        << " user_info: " << order_status_ack.user_info 
        << " order_id: " << order_status_ack.order_id 
        << " cl_ord_id: " << order_status_ack.cl_ord_id 
        << " client_seq_id: " << order_status_ack.client_seq_id 
        << " orig_cl_ord_no: " << order_status_ack.orig_cl_ord_no 
        << " frozen_trade_value: " << order_status_ack.frozen_trade_value 
        << " frozen_fee: " << order_status_ack.frozen_fee 
        << " reject_reason_code: " << order_status_ack.reject_reason_code 
        << " ord_rej_reason: " << order_status_ack.ord_rej_reason 
        << " order_type: " << order_status_ack.order_type 
        << " time_in_force: " << order_status_ack.time_in_force 
        << " position_effect: " << order_status_ack.position_effect 
        << " covered_or_uncovered: " << order_status_ack.covered_or_uncovered 
        << " account_sub_code: " << order_status_ack.account_sub_code 
        << " quote_flag: " << order_status_ack.quote_flag 
        << " security_symbol: " << order_status_ack.security_symbol 
        << " secondary_order_id: " << order_status_ack.secondary_order_id 
        << " parent_cl_ord_no: " << order_status_ack.parent_cl_ord_no 
        << " particular_flags: " << order_status_ack.particular_flags 
        << " batch_cl_ord_no: " << order_status_ack.batch_cl_ord_no << std::endl;

    if (order_status_ack.orig_cl_ord_no)    // 有原始订单号 撤单确认
    {
        onCancelOrderRtn(order_status_ack);
    }else{                                  // 没有原始订单号 委托确认
        onOrderRtn(order_status_ack);
    }
    return;
}

// 订单下达交易所确认
void TiGfTraderClient::OnRspOrderStatusAck(const ATPRspOrderStatusAckMsg& order_status_ack)
{
    LOG(INFO) << "[OnRspOrderStatusAck]:"
        << " partition: " << order_status_ack.partition 
        << " index: " << order_status_ack.index 
        << " business_type: " << order_status_ack.business_type 
        << " cl_ord_no: " << order_status_ack.cl_ord_no 
        << " security_id: " << order_status_ack.security_id 
        << " market_id: " << order_status_ack.market_id 
        << " exec_type: " << order_status_ack.exec_type 
        << " ord_status: " << order_status_ack.ord_status 
        << " cust_id: " << order_status_ack.cust_id 
        << " fund_account_id: " << order_status_ack.fund_account_id 
        << " account_id: " << order_status_ack.account_id 
        << " price: " << order_status_ack.price 
        << " order_qty: " << order_status_ack.order_qty 
        << " leaves_qty: " << order_status_ack.leaves_qty 
        << " cum_qty: " << order_status_ack.cum_qty 
        << " side: " << order_status_ack.side 
        << " transact_time: " << order_status_ack.transact_time 
        << " user_info: " << order_status_ack.user_info 
        << " order_id: " << order_status_ack.order_id 
        << " cl_ord_id: " << order_status_ack.cl_ord_id 
        << " client_seq_id: " << order_status_ack.client_seq_id 
        << " orig_cl_ord_no: " << order_status_ack.orig_cl_ord_no 
        << " frozen_trade_value: " << order_status_ack.frozen_trade_value 
        << " frozen_fee: " << order_status_ack.frozen_fee 
        << " reject_reason_code: " << order_status_ack.reject_reason_code 
        << " ord_rej_reason: " << order_status_ack.ord_rej_reason 
        << " order_type: " << order_status_ack.order_type 
        << " time_in_force: " << order_status_ack.time_in_force 
        << " position_effect: " << order_status_ack.position_effect 
        << " covered_or_uncovered: " << order_status_ack.covered_or_uncovered 
        << " account_sub_code: " << order_status_ack.account_sub_code 
        << " quote_flag: " << order_status_ack.quote_flag 
        << " security_symbol: " << order_status_ack.security_symbol 
        << " secondary_order_id: " << order_status_ack.secondary_order_id 
        << " parent_cl_ord_no: " << order_status_ack.parent_cl_ord_no 
        << " particular_flags: " << order_status_ack.particular_flags 
        << " batch_cl_ord_no: " << order_status_ack.batch_cl_ord_no << std::endl;

    if (order_status_ack.orig_cl_ord_no)    // 有原始订单号 撤单确认
    {
        onCancelOrderRtn(order_status_ack);
    }else{                                  // 没有原始订单号 委托确认
        onOrderRtn(order_status_ack);
    }
    return;
}

// 成交回报
void TiGfTraderClient::OnRspCashAuctionTradeER(const ATPRspCashAuctionTradeERMsg& cash_auction_trade_er) 
{
    int64_t order_id = cash_auction_trade_er.cl_ord_no;
    /*
    if (cash_auction_trade_er.orig_cl_ord_no)
    {
        order_id = cash_auction_trade_er.orig_cl_ord_no;
    }else{
        order_id = cash_auction_trade_er.cl_ord_no;
    }
    */

    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = order_id;
    strcpy(match_ptr->szStreamId, cash_auction_trade_er.exec_id);
    strcpy(match_ptr->szAccount, m_config->szFundAccount.c_str());
    strcpy(match_ptr->szSymbol, cash_auction_trade_er.security_id);
    if (cash_auction_trade_er.market_id == ATPMarketIDConst::kShangHai) {
        strcpy(match_ptr->szExchange, "SH");
    } else if (cash_auction_trade_er.market_id == ATPMarketIDConst::kShenZhen) {
        strcpy(match_ptr->szExchange, "SZ");
    } else {
        strcpy(match_ptr->szExchange, "");
    }
    match_ptr->nMatchPrice = ((double)cash_auction_trade_er.last_px)/10000;
    match_ptr->nMatchVol = cash_auction_trade_er.last_qty / 100;
    match_ptr->nFee = ((double)cash_auction_trade_er.fee) / 10000;

    switch (cash_auction_trade_er.side)
    {
    case ATPSideConst::kSell:
        match_ptr->nTradeSideType = TI_TradeSideType_Sell;
        break;
    case ATPSideConst::kBuy:
        match_ptr->nTradeSideType = TI_TradeSideType_Buy;
        break;
    default:
        break;
    }

    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(cash_auction_trade_er.transact_time);
    updateOrderMatch(match_ptr);

    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(0, order_id);
    if (order_ptr != nullptr)
    {
        strcpy(match_ptr->szOrderStreamId, order_ptr->szOrderStreamId);
        strcpy(match_ptr->szName, order_ptr->szName);
        match_ptr->nReqId = order_ptr->nReqId;

        order_ptr->nDealtVol += match_ptr->nMatchVol;
        order_ptr->nDealtPrice = (order_ptr->nDealtPrice * (order_ptr->nDealtVol - match_ptr->nMatchVol) + match_ptr->nMatchPrice * match_ptr->nMatchVol) / order_ptr->nDealtVol;
        order_ptr->nFee += match_ptr->nFee;
        order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(cash_auction_trade_er.transact_time);
        order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;

        if (order_ptr->nDealtVol == order_ptr->nOrderVol)
        {
            order_ptr->nStatus = TI_OrderStatusType_dealt;
        }
        
        m_cb->OnRtnOrderStatusEvent(order_ptr.get());
    }
    
    m_cb->OnRtnOrderMatchEvent(match_ptr.get());
};

void TiGfTraderClient::OnRspETFRedemptionTradeER(const ATPRspETFRedemptionTradeERMsg& etf_redemption_trade_er)
{
    int64_t order_id = etf_redemption_trade_er.cl_ord_no;
    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(0, order_id);
    if (!order_ptr)
    {
        return;
    }
    
    order_ptr->nDealtPrice = ((double)etf_redemption_trade_er.last_px)/10000;
    order_ptr->nDealtVol = etf_redemption_trade_er.cum_qty / 100;
    order_ptr->nStatus = getOrderStatus(etf_redemption_trade_er.exec_type);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(etf_redemption_trade_er.transact_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;

    if (m_cb)
    {
        m_cb->OnRtnOrderStatusEvent(order_ptr.get());
    }
    
    for(auto iter = etf_redemption_trade_er.constituent_stock.begin(), end = etf_redemption_trade_er.constituent_stock.end(); iter != end; iter++)
    {
        std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
        memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
        match_ptr->nOrderId = order_id;
        sprintf(match_ptr->szStreamId, "%s%s", iter->exec_id, iter->security_id);
        strcpy(match_ptr->szOrderStreamId, order_ptr->szOrderStreamId);
        strcpy(match_ptr->szAccount, order_ptr->szAccount);
        strcpy(match_ptr->szSymbol, iter->security_id);
        if (iter->market_id == ATPMarketIDConst::kShangHai) {
            strcpy(match_ptr->szExchange, "SH");
        } else if (iter->market_id == ATPMarketIDConst::kShenZhen) {
            strcpy(match_ptr->szExchange, "SZ");
        } else {
            strcpy(match_ptr->szExchange, "");
        }
        match_ptr->nMatchPrice = ((double)iter->price)/10000;
        match_ptr->nMatchVol = iter->qty / 100;
        match_ptr->nTradeSideType = order_ptr->nTradeSideType;
        match_ptr->nMatchTimestamp = order_ptr->nLastUpdateTimestamp;
        
        updateOrderMatch(match_ptr);
        if (m_cb)
        {
            m_cb->OnRtnOrderMatchEvent(match_ptr.get());
        }
    }
};

// 订单下达内部拒绝
void TiGfTraderClient::OnRspBizRejection(const ATPRspBizRejectionOtherMsg& biz_rejection)
{
    std::cout << "[OnRspBizRejection]" <<
        " transact_time : " << biz_rejection.transact_time <<
        " client_seq_id : " << biz_rejection.client_seq_id <<
        " msg_type : " << biz_rejection.api_msg_type <<
        " reject_reason_code : " << biz_rejection.reject_reason_code <<
        " business_reject_text : " << biz_rejection.business_reject_text <<
        " user_info : " << biz_rejection.user_info << std::endl;
};


void TiGfTraderClient::onOrderRtn(const ATPRspOrderStatusAckMsg& msg)
{
    LOG(INFO) << "onOrderRtn" << std::endl;
    int64_t req_id = msg.client_seq_id;
    int64_t order_id = msg.cl_ord_no;
    LOG(INFO) << "req_id: " << req_id << std::endl;
    LOG(INFO) << "order_id: " << order_id << std::endl;

    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(req_id, order_id);
    if (!order_ptr)
    {
        return;
    }

    strcpy(order_ptr->szName, msg.security_symbol);
    order_ptr->nOrderId = order_id;
    strcpy(order_ptr->szOrderStreamId, msg.order_id);
    //order_ptr->nStatus = getOrderStatus(msg.exec_type);
    order_ptr->nStatus = getOrderStatus(msg.ord_status);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(msg.transact_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    
    updateOrderMap(order_ptr);

    if(m_cb){
        m_cb->OnRtnOrderStatusEvent(order_ptr.get());
    }
};
void TiGfTraderClient::onCancelOrderRtn(const ATPRspOrderStatusAckMsg& msg)
{
    LOG(INFO) << "onCancelOrderRtn" << std::endl;
    int64_t req_id = msg.client_seq_id;
    int64_t order_id = msg.orig_cl_ord_no;
    LOG(INFO) << "req_id: " << req_id << std::endl;
    LOG(INFO) << "order_id: " << order_id << std::endl;

    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(req_id, order_id);
    if (!order_ptr)
    {
        return;
    }

    order_ptr->nStatus = getOrderStatus(msg.ord_status);

    if(m_cb){
        m_cb->OnRtnOrderStatusEvent(order_ptr.get());
    }
};
////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiGfTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations   = string(_iniFile["ti_gf_trader_client"]["locations"]);
    m_config->szLocations2   = string(_iniFile["ti_gf_trader_client"]["locations2"]);
    
    m_config->szUser                = string(_iniFile["ti_gf_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_gf_trader_client"]["pass"]);

    m_config->szSoftwareName        = string(_iniFile["ti_gf_trader_client"]["software_name"]);
    m_config->szSoftwareVersion     = string(_iniFile["ti_gf_trader_client"]["software_version"]);

    m_config->szBranchCode          = string(_iniFile["ti_gf_trader_client"]["branch_code"]);
    m_config->szCustomerId          = string(_iniFile["ti_gf_trader_client"]["customer_id"]);
    m_config->szFundAccount         = string(_iniFile["ti_gf_trader_client"]["fund_account"]);
    m_config->szFundPass            = string(_iniFile["ti_gf_trader_client"]["fund_pass"]);
    m_config->szShareholderIdSH     = string(_iniFile["ti_gf_trader_client"]["shareholder_id_sh"]);
    m_config->szShareholderIdSZ     = string(_iniFile["ti_gf_trader_client"]["shareholder_id_sz"]);

    
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

bool TiGfTraderClient::init_encrypt()
{
    ATPRetCodeType ec = ATPTradeAPI::Init();
    if (ec != ErrorCode::kSuccess)
    {
        LOG(INFO) << "Init encrypt failed: " << ec <<std::endl;
        return false;
    }
    return true;
};

ATPRetCodeType TiGfTraderClient::connect(ATPTradeAPI* client, ATPTradeHandler* handler)
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
                LOG(INFO) << "Invoke Connect error:" << ec << std::endl;
                return ec;
            }
            sleep(0);
        }
    }

    return kSuccess;
};

ATPRetCodeType TiGfTraderClient::close(ATPTradeAPI* client)
{
    g_waiting_flag.store(true);
    ATPRetCodeType ec = client->Close();
    if (ec != ErrorCode::kSuccess)
    {
        LOG(INFO) << "Invoke Close error:" << ec << std::endl;
        return ec;
    }

    while(g_waiting_flag.load())
    {
        sleep(0);
    }
    return ErrorCode::kSuccess;
};

ATPRetCodeType TiGfTraderClient::login(ATPTradeAPI* client)
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
        LOG(INFO) << "Invoke CustLogin error:" << ec << std::endl;
        return ec;
    }

    while(g_waiting_flag.load())
    {
        sleep(0);
    }

    return ErrorCode::kSuccess;
};

ATPRetCodeType TiGfTraderClient::logout(ATPTradeAPI* client)
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
        LOG(INFO) << "Invoke CustLogout error:" << ec << std::endl;
        return ec;
    }

    while(g_waiting_flag.load())
    {
        sleep(0);
    }

    return ErrorCode::kSuccess;
};

bool TiGfTraderClient::init(ATPTradeAPI* client, ATPTradeHandler* handler)
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

TI_OrderStatusType TiGfTraderClient::getOrderStatus(ATPExecTypeType status)
{
    switch (status)
    {
    case ATPExecTypeConst::kNew:
        return TI_OrderStatusType_queued;
    case ATPExecTypeConst::kInternal:
        return TI_OrderStatusType_queued;
    case ATPExecTypeConst::kCancelled:
        return TI_OrderStatusType_removed;
    case ATPExecTypeConst::kReject:
        return TI_OrderStatusType_fail;
    case ATPExecTypeConst::kTrade:
        return TI_OrderStatusType_dealt;
    case ATPExecTypeConst::kGold:
        return TI_OrderStatusType_dealt;
    default:
        return TI_OrderStatusType_unAccept;
    }
    return TI_OrderStatusType_unAccept;
};

TI_OrderStatusType TiGfTraderClient::getOrderStatus(ATPOrdStatusType status)
{
    switch (status)
    {
    case ATPOrdStatusConst::kNew:
        return TI_OrderStatusType_queued;
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

std::shared_ptr<TiRtnOrderStatus> TiGfTraderClient::getOrderPtr(int64_t req_id, int64_t order_id)
{
    if (m_order_map.find(order_id) != m_order_map.end())
    {
        return m_order_map[order_id];
    }
    if (m_order_req_map.find(req_id) != m_order_req_map.end())
    {
        return m_order_req_map[req_id];
    }
    return NULL;
};

void TiGfTraderClient::updateOrderMap(std::shared_ptr<TiRtnOrderStatus> order_ptr)
{
    auto iter = m_order_map.find(order_ptr->nOrderId);
    if (iter == m_order_map.end())
    {
        m_order_map[order_ptr->nOrderId] = order_ptr;

        auto req_iter = m_order_req_map.find(order_ptr->nReqId);
        if (req_iter != m_order_req_map.end())
        {
            m_order_req_map.erase(req_iter);
        }
    }
};

void TiGfTraderClient::updateOrderMatch(std::shared_ptr<TiRtnOrderMatch> match)
{
    m_matches_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderMatch>>(match->nOrderId, match));
};

int TiGfTraderClient::orderInsertStock(TiReqOrderInsert* req){
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
    msg.price = (int(req->nOrderPrice * 10000 + 5)/10 * 10);   // 委托价格 N13(4)，21.0000元
    msg.order_qty = req->nOrderVol * 100;                    // (需要 * 100才是股数) 申报数量N15(2)；股票为股、基金为份、上海债券默认为张（使用时请务必与券商确认），其他为张；1000.00股
    msg.client_seq_id = nReqId;                              // 用户系统消息序号
    msg.order_way = '0';                                     // 委托方式，自助委托
    //msg.order_way = 'R';                                     // 委托方式，自助委托
    strncpy(msg.password, "password1", 129);                 // 客户密码
    msg.client_feature_code = g_client_feature_code;         // 终端识别码

    ATPRetCodeType ec = m_client->ReqCashAuctionOrder(&msg);
    if (ec != ErrorCode::kSuccess)
    {
        LOG(INFO) << "Invoke Send error:" << ec << std::endl;
    }

    return nReqId;
};
int TiGfTraderClient::orderInsertEtf(TiReqOrderInsert* req){
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
    msg.client_feature_code = g_client_feature_code;         // 终端识别码

    ATPRetCodeType ec = m_client->ReqETFRedemptionOrder(&msg);
    if (ec != ErrorCode::kSuccess)
    {
        LOG(INFO) << "Invoke Send error:" << ec << std::endl;
    }

    return nReqId;
};

int TiGfTraderClient::queryOrders(int64_t start_index)
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    ATPClientSeqIDType seq_id = ++nReqId;
	ATPReqOrderQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    //strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);       // 股东户ID
    msg.query_index = start_index;                                            // 查询起始位置
	msg.client_seq_id = seq_id;
    //msg.return_seq = ATPReturnSeqConst::kTimeOrderRe;                       // 返回的顺序
	strncpy(msg.password, m_config->szFundPass.c_str(),129);
    msg.client_feature_code = g_client_feature_code;                          // 终端识别码

    m_client->ReqOrderQuery(&msg);

    return nReqId;
};

int TiGfTraderClient::queryEtfMatches(int64_t start_index)
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    ATPClientSeqIDType seq_id = ++nReqId;
    ATPReqETFQueryOrderExMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    //strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);       // 股东户ID
    msg.query_index = start_index;                                            // 查询起始位置
    msg.client_seq_id = seq_id;
    //msg.return_seq = ATPReturnSeqConst::kTimeOrderRe;                       // 返回的顺序
    strncpy(msg.password, m_config->szFundPass.c_str(),129);
    msg.client_feature_code = g_client_feature_code;                          // 终端识别码

    m_client->ReqETFQueryOrderEx(&msg);

    return nReqId;
};

int TiGfTraderClient::queryMatches(int64_t start_index)
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    ATPClientSeqIDType seq_id = ++nReqId;
	ATPReqTradeOrderQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    //strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);       // 股东户ID
	msg.client_seq_id = seq_id;
    msg.query_index = start_index; 
	strncpy(msg.password, m_config->szFundPass.c_str(),129);
    msg.etf_purchase_redemption_flag = 0;
    //msg.return_seq = ATPReturnSeqConst::kTimeOrderRe;                         // 返回的顺序
    msg.client_feature_code = g_client_feature_code;                          // 终端识别码

    m_client->ReqTradeOrderQuery(&msg);

    return nReqId;
};

int TiGfTraderClient::queryPositions(int64_t start_index)
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    ATPClientSeqIDType seq_id = ++nReqId;
	ATPReqShareQueryMsg msg;
	
    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    //strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);       // 账户ID
	msg.client_seq_id = seq_id;
    msg.query_index = start_index; 
	strncpy(msg.password, m_config->szFundPass.c_str(),129);
    msg.client_feature_code = g_client_feature_code;                          // 终端识别码

    m_client->ReqShareQuery(&msg);

    return nReqId;
};

////////////////////////////////////////////////////////////////////////
// 对外接口
////////////////////////////////////////////////////////////////////////

void TiGfTraderClient::connect(){
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
        LOG(INFO) << "Wait for ack. press enter for exit." << std::endl;
    }
};

int TiGfTraderClient::orderInsert(TiReqOrderInsert* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    if (strcmp(req->szAccount, m_config->szFundAccount.c_str()) != 0)
    {
        LOG(INFO) << "[orderInsert] Wrong account id";
        return -1;
    }
    LOG(INFO) << "TiGfTraderClient::orderInsert: " << req->szSymbol 
        << "szName " << req->szName
        << ", szExchange " << req->szExchange
        << ", szAccount " << req->szAccount
        << ", nTradeSideType " << req->nTradeSideType
        << ", nOffsetType " << req->nOffsetType
        << ", nBusinessType " << req->nBusinessType
        << ", nOrderPrice " << req->nOrderPrice
        << ", nOrderVol " << req->nOrderVol << std::endl;
    
    std::shared_ptr<TiRtnOrderStatus> order_ptr = std::make_shared<TiRtnOrderStatus>();
    memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));

    int req_id = -1;

    if(req->nBusinessType == TI_BusinessType_Stock){
        req_id = orderInsertStock(req);
    }else if(req->nBusinessType == TI_BusinessType_ETF){
        req_id = orderInsertEtf(req);
    }
    if (req_id != -1)
    {
        memcpy(order_ptr.get(), req, sizeof(TiReqOrderInsert));
        order_ptr->nInsertTimestamp = datetime::get_now_timestamp_ms();
        m_order_req_map[req_id] = order_ptr;
    }
    return req_id;
};

int TiGfTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    if (strcmp(req->szAccount, m_config->szFundAccount.c_str()) != 0)
    {
        LOG(INFO) << "[orderInsert] Wrong account id";
        return -1;
    }
    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(0, req->nOrderId);
    LOG(INFO) << "orderDelete: " << req->nOrderId << " " << order_ptr << std::endl;
    if (order_ptr == NULL)
    {
        return -1;
    }

    ATPReqCancelOrderMsg msg = {};

    msg.client_seq_id = ++nReqId;
    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);               // 账户ID
	msg.orig_cl_ord_no = req->nOrderId;

    if (!strcmp(order_ptr->szExchange, "SH"))
    {
        msg.market_id = ATPMarketIDConst::kShangHai;             // 市场ID，上海
        strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
    }
    if (!strcmp(order_ptr->szExchange, "SZ"))
    {
        msg.market_id = ATPMarketIDConst::kShenZhen;             // 市场ID，上海
        strncpy(msg.account_id, m_config->szShareholderIdSZ.c_str(), 13);               // 账户ID
    }

	strncpy(msg.password, m_config->szFundPass.c_str(),129);
    msg.client_feature_code = g_client_feature_code;         // 终端识别码

    ATPRetCodeType ec = m_client->ReqCancelOrder(&msg);
    if (ec != ErrorCode::kSuccess)
    {
        LOG(INFO) << "Invoke Send error:" << ec << std::endl;
    }

    return nReqId;
};

TiRtnOrderStatus* TiGfTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(req_id, order_id);
    if (order_ptr)
    {
        return order_ptr.get();
    }else{
        return nullptr;
    }
};
    

int TiGfTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    ATPClientSeqIDType seq_id = ++nReqId;
	ATPReqFundQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);         // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);
    msg.client_feature_code = g_client_feature_code;                          // 终端识别码

    m_client->ReqFundQuery(&msg);

    return nReqId;
};

int TiGfTraderClient::QueryOrders()
{
    return queryOrders(0);
};

int TiGfTraderClient::QueryMatches()
{
    queryMatches(0);
    return queryEtfMatches(0);
};

int TiGfTraderClient::QueryPositions()
{
    return queryPositions(0);
};