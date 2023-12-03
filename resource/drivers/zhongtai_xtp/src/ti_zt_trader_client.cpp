#include "ti_zt_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"

TiZtTraderClient::TiZtTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_api = TraderApi::CreateTraderApi(2, "./", XTP_LOG_LEVEL_DEBUG);
	m_api->SetHeartBeatInterval(15);
	m_api->RegisterSpi(this);

    nSessionId = 0;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 100;   //跳过xtp client设置成交模式的区段
};


TiZtTraderClient::~TiZtTraderClient()
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
void TiZtTraderClient::OnOrderEvent(XTPOrderInfo *order_info, XTPRI *error_info, uint64_t session_id) {
    std::shared_ptr<TiRtnOrderStatus> order_ptr;
    auto iter = m_order_map.find(order_info->order_xtp_id);
    if (iter == m_order_map.end())
    {
        iter = m_order_req_map.find(order_info->order_client_id);
        if (iter == m_order_req_map.end())
        {
            order_ptr = std::make_shared<TiRtnOrderStatus>();
            memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
            strcpy(order_ptr->szSymbol, order_info->ticker);
            if (order_info->market == XTP_MKT_SZ_A) {
                strcpy(order_ptr->szExchange, "SZ");
            } else if (order_info->market == XTP_MKT_SH_A) {
                strcpy(order_ptr->szExchange, "SH");
            } else {
                strcpy(order_ptr->szExchange, "");
            }

            switch (order_info->side)
            {
            case XTP_SIDE_BUY:
                order_ptr->nTradeSideType = TI_TradeSideType_Buy;
                break;
            case XTP_SIDE_SELL:
                order_ptr->nTradeSideType = TI_TradeSideType_Sell;
                break;
            case XTP_SIDE_PURCHASE:
                order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
                break;
            case XTP_SIDE_REDEMPTION:
                order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
                break;
            default:
                break;
            }

            if (order_info->business_type == XTP_BUSINESS_TYPE_CASH) {
                order_ptr->nBusinessType = TI_BusinessType_Stock;
            }
            if (order_info->business_type == XTP_BUSINESS_TYPE_ETF) {
                order_ptr->nBusinessType = TI_BusinessType_ETF;
            }

            order_ptr->nOrderPrice = order_info->price;
            order_ptr->nOrderVol = order_info->quantity;

            m_order_map[order_info->order_xtp_id] = order_ptr;
        }else{
            order_ptr = iter->second;
            m_order_req_map.erase(iter);
            m_order_map[order_info->order_xtp_id] = order_ptr;
        }
    }else{
        order_ptr = iter->second;
    }
    order_ptr->nReqId = order_info->order_client_id;
    order_ptr->nOrderId = order_info->order_xtp_id;
    order_ptr->nSubmitVol = order_info->quantity;
    order_ptr->nDealtPrice = order_info->qty_traded ? order_info->trade_amount/order_info->qty_traded : 0;
    order_ptr->nDealtVol = order_info->trade_amount ? order_info->qty_traded : 0;
    order_ptr->nStatus = getOrderStatus(order_info->order_status);
    order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(order_info->insert_time);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(order_info->update_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    strcpy(order_ptr->szErr, error_info->error_msg);

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
};
void TiZtTraderClient::OnTradeEvent(XTPTradeReport *trade_info, uint64_t session_id) {
    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = trade_info->order_xtp_id;
    strcpy(match_ptr->szStreamId, trade_info->exec_id);
    match_ptr->nMatchPrice = trade_info->price;
    match_ptr->nMatchVol = trade_info->quantity;
    strcpy(match_ptr->szSymbol, trade_info->ticker);

    if (trade_info->market == XTP_MKT_SZ_A) {
        strcpy(match_ptr->szExchange, "SZ");
    } else if (trade_info->market == XTP_MKT_SH_A) {
        strcpy(match_ptr->szExchange, "SH");
    } else {
        strcpy(match_ptr->szExchange, "");
    }
    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(trade_info->trade_time);
    match_ptr->nTradeSideType = trade_info->side == XTP_SIDE_BUY ? TI_TradeSideType_Buy : TI_TradeSideType_Sell;
    m_matches_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderMatch>>(match_ptr->nOrderId, match_ptr));
    
    m_cb->OnRtnOrderMatchEvent(match_ptr.get());
};
void TiZtTraderClient::OnCancelOrderError(XTPOrderCancelInfo *cancel_info, XTPRI *error_info, uint64_t session_id) {
    std::cout << "TiZtTraderClient::OnCancelOrderError" << std::endl;
};

void TiZtTraderClient::OnQueryOrderEx(XTPOrderInfoEx *order_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) {
    std::shared_ptr<TiRtnOrderStatus> order_ptr;
    auto iter = m_order_map.find(order_info->order_xtp_id);
    if (iter == m_order_map.end())
    {
        order_ptr = std::make_shared<TiRtnOrderStatus>();
        memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
        m_order_map[order_info->order_xtp_id] = order_ptr;
    }else{
        order_ptr = iter->second;
    }
    
    strcpy(order_ptr->szSymbol, order_info->ticker);
    if (order_info->market == XTP_MKT_SZ_A) {
        strcpy(order_ptr->szExchange, "SZ");
    } else if (order_info->market == XTP_MKT_SH_A) {
        strcpy(order_ptr->szExchange, "SH");
    } else {
        strcpy(order_ptr->szExchange, "");
    }

    switch (order_info->side)
    {
    case XTP_SIDE_BUY:
        order_ptr->nTradeSideType = TI_TradeSideType_Buy;
        break;
    case XTP_SIDE_SELL:
        order_ptr->nTradeSideType = TI_TradeSideType_Sell;
        break;
    case XTP_SIDE_PURCHASE:
        order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
        break;
    case XTP_SIDE_REDEMPTION:
        order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
        break;
    
    default:
        break;
    }

    if (order_info->business_type == XTP_BUSINESS_TYPE_CASH) {
        order_ptr->nBusinessType = TI_BusinessType_Stock;
    }
    if (order_info->business_type == XTP_BUSINESS_TYPE_ETF) {
        order_ptr->nBusinessType = TI_BusinessType_ETF;
    }

    order_ptr->nOrderPrice = order_info->price;
    order_ptr->nOrderVol = order_info->quantity;

    order_ptr->nOrderId = order_info->order_xtp_id;
    order_ptr->nSubmitVol = order_info->quantity;
    order_ptr->nDealtPrice = order_info->qty_traded ? order_info->trade_amount/order_info->qty_traded : 0;
    order_ptr->nDealtVol =  order_info->trade_amount ? order_info->qty_traded : 0;
    order_ptr->nStatus = getOrderStatus(order_info->order_status);
    order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(order_info->insert_time);
    order_ptr->nLastUpdateTimestamp = datetime::get_timestamp_ms(order_info->update_time);
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    strcpy(order_ptr->szOrderStreamId, order_info->order_exch_id);
    strcpy(order_ptr->szErr, order_info->order_err_t.error_msg);
    
    m_cb->OnRspQryOrder(order_ptr.get(), is_last);
};

void TiZtTraderClient::OnQueryTrade(XTPQueryTradeRsp *trade_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id)
{
    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = trade_info->order_xtp_id;
    strcpy(match_ptr->szStreamId, trade_info->exec_id);
    match_ptr->nMatchPrice = trade_info->price;
    match_ptr->nMatchVol = trade_info->quantity;
    strcpy(match_ptr->szSymbol, trade_info->ticker);

    if (trade_info->market == XTP_MKT_SZ_A) {
        strcpy(match_ptr->szExchange, "SZ");
    } else if (trade_info->market == XTP_MKT_SH_A) {
        strcpy(match_ptr->szExchange, "SH");
    } else {
        strcpy(match_ptr->szExchange, "");
    }
    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(trade_info->trade_time);
    match_ptr->nTradeSideType = trade_info->side == XTP_SIDE_BUY ? TI_TradeSideType_Buy : TI_TradeSideType_Sell;
    m_matches_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderMatch>>(match_ptr->nOrderId, match_ptr));
    
    m_cb->OnRspQryMatch(match_ptr.get(), is_last);
};

void TiZtTraderClient::OnQueryPosition(XTPQueryStkPositionRsp *position, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id)
{
    return;
    std::cout << "TiZtTraderClient::OnQueryPosition" << std::endl;
    json j = {
        {"ticker", position->ticker},
        {"ticker_name", position->ticker_name},
        {"market", position->market},
        {"total_qty", position->total_qty},
        {"sellable_qty", position->sellable_qty},
        {"avg_price", position->avg_price},
        {"unrealized_pnl", position->unrealized_pnl},
        {"yesterday_position", position->yesterday_position},
        {"purchase_redeemable_qty", position->purchase_redeemable_qty},
        {"position_direction", position->position_direction},
        {"position_security_type", position->position_security_type},
        {"executable_option", position->executable_option},
        {"lockable_position", position->lockable_position},
        {"executable_underlying", position->executable_underlying},
        {"locked_position", position->locked_position},
        {"usable_locked_position", position->usable_locked_position},
        {"profit_price", position->profit_price},
        {"buy_cost", position->buy_cost},
        {"profit_cost", position->profit_cost},
        {"market_value", position->market_value},
    };
    std::cout << j << std::endl;
};

void TiZtTraderClient::OnQueryAsset(XTPQueryAssetRsp *asset, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id)
{
    std::cout << "TiZtTraderClient::OnQueryAsset" << std::endl;
    json m_json_rsp = {
        { "type", "XtpOnQueryAsset"},
        { "data", {
            {"total_asset", asset->total_asset},
            {"buying_power", asset->buying_power},
            {"security_asset", asset->security_asset},
            {"fund_buy_amount", asset->fund_buy_amount},
            {"fund_buy_fee", asset->fund_buy_fee},
            {"fund_sell_amount", asset->fund_sell_amount},
            {"fund_sell_fee", asset->fund_sell_fee},
            {"withholding_amount", asset->withholding_amount},
            {"account_type", asset->account_type},
            {"frozen_margin", asset->frozen_margin},
            {"frozen_exec_cash", asset->frozen_exec_cash},
            {"frozen_exec_fee", asset->frozen_exec_fee},
            {"pay_later", asset->pay_later},
            {"preadva_pay", asset->preadva_pay},
            {"orig_banlance", asset->orig_banlance},
            {"banlance", asset->banlance},
            {"deposit_withdraw", asset->deposit_withdraw},
            {"trade_netting", asset->trade_netting},
            {"captial_asset", asset->captial_asset},
            {"force_freeze_amount", asset->force_freeze_amount},
            {"preferred_amount", asset->preferred_amount},
            {"repay_stock_aval_banlance", asset->repay_stock_aval_banlance},

            {"fund_order_data_charges", asset->fund_order_data_charges},
            {"fund_cancel_data_charges", asset->fund_cancel_data_charges},
            {"exchange_cur_risk_degree", asset->exchange_cur_risk_degree},
            {"company_cur_risk_degree", asset->company_cur_risk_degree},
        }}
    };
    m_cb->OnCommonJsonRespones(&m_json_rsp, request_id, is_last, error_info->error_id, error_info->error_msg);
};

void TiZtTraderClient::OnQueryETF(XTPQueryETFBaseRsp *etf_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id) 
{
    if (error_info->error_id != 0)
    {
         m_cb->OnCommonJsonRespones(NULL, request_id, is_last, error_info->error_id, error_info->error_msg);
        return;
    }
    TI_ExchangeType exchange;
    if(etf_info->market == XTP_MKT_SH_A){
        strcpy(exchange, "SH");
    }
    if(etf_info->market == XTP_MKT_SZ_A){
        strcpy(exchange, "SZ");
    }
    m_json_rsp = {
        { "type", "XtpQueryETFRsp"},
        { "data", {
            { "exchange", exchange },
            { "symbol", etf_info->etf },
            { "unit", etf_info->unit },
            { "subscribe_status", etf_info->subscribe_status },
            { "redemption_status", etf_info->redemption_status },
            { "max_cash_ratio", etf_info->max_cash_ratio },
            { "estimate_amount", etf_info->estimate_amount },
            { "cash_component", etf_info->cash_component },
            { "net_value", etf_info->net_value },
            { "total_amount", etf_info->total_amount },
        }},
        { "session_id", session_id }
    };
    m_cb->OnCommonJsonRespones(&m_json_rsp, request_id, is_last, error_info->error_id, error_info->error_msg);
};

void TiZtTraderClient::OnQueryETFBasket(XTPQueryETFComponentRsp *etf_component_info, XTPRI *error_info, int request_id, bool is_last, uint64_t session_id)
{
    if (error_info->error_id != 0)
    {
        m_cb->OnCommonJsonRespones(NULL, request_id, is_last, error_info->error_id, error_info->error_msg);
        return;
    }
    
    TI_ExchangeType exchange;
    if(etf_component_info->component_market == XTP_MKT_SH_A){
        strcpy(exchange, "SH");
    }
    if(etf_component_info->component_market == XTP_MKT_SZ_A){
        strcpy(exchange, "SZ");
    }
    m_json_rsp = {
        { "type", "XtpQueryETFBasketRsp"},
        { "data", {
            { "etf_symbol", etf_component_info->ticker },
            { "exchange", exchange },
            { "symbol", etf_component_info->component_ticker },
            { "name", etf_component_info->component_name },
            { "disclosure_vol", etf_component_info->quantity },
            { "replace_type", etf_component_info->replace_type },
            { "premium_ratio", etf_component_info->premium_ratio },
            { "amount", etf_component_info->amount },
            { "creation_premium_ratio", etf_component_info->creation_premium_ratio },
            { "redemption_discount_ratio", etf_component_info->redemption_discount_ratio },
            { "creation_amount", etf_component_info->creation_amount },
            { "redemption_amount", etf_component_info->redemption_amount },
        }},
        { "session_id", session_id }
    };
    m_cb->OnCommonJsonRespones(&m_json_rsp, request_id, is_last, error_info->error_id, error_info->error_msg);
};


////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiZtTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szIp                  = string(_iniFile["ti_zt_trader_client"]["ip"]);
    m_config->nPort                 = int(_iniFile["ti_zt_trader_client"]["port"]);
    m_config->szLocalIp             = string(_iniFile["ti_zt_trader_client"]["local_ip"]);

    m_config->szUser                = string(_iniFile["ti_zt_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_zt_trader_client"]["pass"]);

    m_config->szSoftwareKey         = string(_iniFile["ti_zt_trader_client"]["software_key"]);
    m_config->szSoftwareVersion     = string(_iniFile["ti_zt_trader_client"]["software_version"]);

    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szLocalIp.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty() |
        m_config->szSoftwareKey.empty() |
        m_config->szSoftwareVersion.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

TI_OrderStatusType TiZtTraderClient::getOrderStatus(XTP_ORDER_STATUS_TYPE status)
{
    switch (status)
    {
    case XTP_ORDER_STATUS_INIT:
        return TI_OrderStatusType_unAccept;
    case XTP_ORDER_STATUS_ALLTRADED:
        return TI_OrderStatusType_dealt;
    case XTP_ORDER_STATUS_PARTTRADEDQUEUEING:
        return TI_OrderStatusType_queued;
    case XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING:
        return TI_OrderStatusType_dealt;
    case XTP_ORDER_STATUS_NOTRADEQUEUEING:
        return TI_OrderStatusType_queued;
    case XTP_ORDER_STATUS_CANCELED:
        return TI_OrderStatusType_removed;
    case XTP_ORDER_STATUS_REJECTED:
        return TI_OrderStatusType_fail;
    case XTP_ORDER_STATUS_UNKNOWN:
        return TI_OrderStatusType_unAccept;
    default:
        return TI_OrderStatusType_unAccept;
    }
};

void TiZtTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    m_api->SubscribePublicTopic(XTP_TERT_QUICK);

    m_api->SetSoftwareKey(m_config->szSoftwareKey.c_str());
    m_api->SetSoftwareVersion(m_config->szSoftwareVersion.c_str());

    nSessionId = m_api->Login(m_config->szIp.c_str(), m_config->nPort, 
        m_config->szUser.c_str(), m_config->szPass.c_str(), 
        XTP_PROTOCOL_TCP, m_config->szLocalIp.c_str());
	if (!nSessionId)
	{
		// 登录失败，获取错误信息
		XTPRI* error_info = m_api->GetApiLastError();
		LOG(INFO) << "login to server error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
	}
    LOG(INFO) << "login successed:" << nSessionId << std::endl;
    if(nSessionId){
        QueryAsset();
        //QueryOrders();
        //QueryMatches();
        //QueryPositions();
    }
    return;
};

int TiZtTraderClient::orderInsert(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;
    XTPOrderInsertInfo order = {0};
    order.order_client_id = req->nReqId;
    strcpy(order.ticker, req->szSymbol);
    if (!strcmp(req->szExchange, "SH"))
    {
        order.market = XTP_MKT_SH_A;
    }
    if (!strcmp(req->szExchange, "SZ"))
    {
        order.market = XTP_MKT_SZ_A;
    }
    //order.price = req->nOrderPrice + 0.00000005;
    order.price = req->nOrderPrice;
    order.quantity = req->nOrderVol;
    order.price_type = XTP_PRICE_LIMIT;

    if (req->nTradeSideType == TI_TradeSideType_Buy)
    {
        order.side = XTP_SIDE_BUY;
    }
    if (req->nTradeSideType == TI_TradeSideType_Sell)
    {
        order.side = XTP_SIDE_SELL;
    }
    if (req->nTradeSideType == TI_TradeSideType_Purchase)
    {
        order.side = XTP_SIDE_PURCHASE;
    }
    if (req->nTradeSideType == TI_TradeSideType_Redemption)
    {
        order.side = XTP_SIDE_REDEMPTION;
    }

    if (req->nBusinessType == TI_BusinessType_Stock)
    {
        order.business_type = XTP_BUSINESS_TYPE_CASH;
    }
    if (req->nBusinessType == TI_BusinessType_ETF)
    {
        order.business_type = XTP_BUSINESS_TYPE_ETF;
    }

    std::shared_ptr<TiRtnOrderStatus> order_ptr = std::make_shared<TiRtnOrderStatus>();
    memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
    memcpy(order_ptr.get(), req, sizeof(TiReqOrderInsert));

    m_order_req_map[order_ptr->nReqId] = order_ptr;

    m_api->InsertOrder(&order, nSessionId); 
    XTPRI* error_info = m_api->GetApiLastError();
    LOG(INFO) << "InsertOrder error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    return req->nReqId;
};

int TiZtTraderClient::orderDelete(TiReqOrderDelete* req){
    TiRtnOrderStatus* order_status = getOrderStatus(req->nReqId, req->nOrderId);
    if (order_status)
    {
        m_api->CancelOrder(order_status->nOrderId, nSessionId);
        XTPRI* error_info = m_api->GetApiLastError();
        LOG(INFO) << "CancelOrder error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    }
    return 0;
};

TiRtnOrderStatus* TiZtTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
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
    

int TiZtTraderClient::QueryAsset()
{
    int flag = m_api->QueryAsset(nSessionId, ++nReqId);
    printf("[QueryAsset] %d\n", flag);
    XTPRI* error_info = m_api->GetApiLastError();
    LOG(INFO) << "QueryAsset error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    return nReqId;
};

int TiZtTraderClient::QueryOrders()
{
    XTPQueryOrderReq req = {0};
    int flag = m_api->QueryOrdersEx(&req, nSessionId, ++nReqId);
    printf("[QueryOrders] %d\n", flag);
    XTPRI* error_info = m_api->GetApiLastError();
    LOG(INFO) << "QueryOrders error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    return nReqId;
};

int TiZtTraderClient::QueryMatches()
{
    XTPQueryTraderReq req = {0};
    int flag = m_api->QueryTrades(&req, nSessionId, ++nReqId);
    printf("[QueryMatches] %d\n", flag);
    XTPRI* error_info = m_api->GetApiLastError();
    LOG(INFO) << "QueryMatches error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    return nReqId;
};

int TiZtTraderClient::QueryPositions()
{
    int flag = m_api->QueryPosition(NULL, nSessionId, ++nReqId);
    printf("[QueryPositions] %d\n", flag);
    XTPRI* error_info = m_api->GetApiLastError();
    LOG(INFO) << "QueryPositions error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    return nReqId;
};

int TiZtTraderClient::QueryAllETFBaseInfo()
{
    XTPQueryETFBaseReq req = {
        XTP_MKT_INIT, 
        ""
    };
    int flag = m_api->QueryETF(&req, nSessionId, ++nReqId);
    printf("[QueryAllETFBaseInfo] %d\n", flag);
    XTPRI* error_info = m_api->GetApiLastError();
    LOG(INFO) << "QueryETF error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    return nReqId;
};

int TiZtTraderClient::QueryETFBasketList(TI_ExchangeType exchange, const char* symbol)
{
    std::cout << "QueryETFBasketList" << exchange << " : " << symbol << std::endl;

    XTP_MARKET_TYPE _market_type = XTP_MKT_INIT;
    if (!strcmp(exchange, "SH"))
    {
        _market_type = XTP_MKT_SH_A;
    }
    if (!strcmp(exchange, "SZ"))
    {
        _market_type = XTP_MKT_SZ_A;
    }
    
    XTPQueryETFComponentReq query_param = {
        _market_type,
        ""
    };
    strcpy(query_param.ticker, symbol);
    
    m_api->QueryETFTickerBasket(&query_param, nSessionId, ++nReqId);
    XTPRI* error_info = m_api->GetApiLastError();
    LOG(INFO) << "QueryETFTickerBasket error, " << error_info->error_id << " : " << error_info->error_msg << std::endl;
    return nReqId;
};