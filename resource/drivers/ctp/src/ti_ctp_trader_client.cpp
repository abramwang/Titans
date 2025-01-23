#include "ti_ctp_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"
#include "ti_trader_formater.h"

#include <atomic>


TiCtpTraderClient::TiCtpTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;

    loadConfig(configPath);
    
    nSessionId = 0;
    m_cb = userCb;
    nReqId =  datetime::get_time_sec_num()*10;   //跳过xtp client设置成交模式的区段

    
    std::cout << "[TiCtpTraderClient] GetApiVersion: " << CThostFtdcTraderApi::GetApiVersion() << std::endl;


    m_client = CThostFtdcTraderApi::CreateFtdcTraderApi();
    m_client->RegisterSpi(this);
    m_client->SubscribePrivateTopic(THOST_TERT_RESTART);
    m_client->SubscribePublicTopic(THOST_TERT_RESTART);
    m_client->RegisterFront((char*)m_config->szLocations.c_str());

    m_client->Init();
    std::cout << "[TiCtpTraderClient] Init over" << std::endl;
    //m_client->Join();
    std::cout << "[TiCtpTraderClient] Join over" << std::endl;
};


TiCtpTraderClient::~TiCtpTraderClient()
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
void TiCtpTraderClient::OnFrontConnected()
{
    std::cout << "交易前置已连接" << std::endl;


    CThostFtdcReqAuthenticateField req = {0};
    
    strncpy(req.BrokerID, m_config->szBrokerID.c_str(), sizeof(req.BrokerID));
    strncpy(req.AppID, m_config->szAppID.c_str(), sizeof(req.AppID));
    strncpy(req.AuthCode, m_config->szAuthCode.c_str(), sizeof(req.AuthCode));

    /*
    strcpy(req.BrokerID, "1032");
    strcpy(req.AppID, "client_oceanus_v1.0");
    strcpy(req.AuthCode, "NV6DXTLD3ZZ8R7UL");
    //strcpy(req.UserID, "000001788");
    //strcpy(req.UserProductInfo, "client_oceanus_v1.0");
    */
    
    int flag2 = m_client->ReqAuthenticate(&req, 2);
    std::cout << "ReqAuthenticate: " << flag2 << std::endl;
};

void TiCtpTraderClient::OnFrontDisconnected(int nReason)
{
    std::cout << "交易前置连接断开，原因：" << nReason << std::endl;
    // 连接断开后的处理
};

void TiCtpTraderClient::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    
    std::cout << "OnRspAuthenticate：" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << " ErrorMsg:" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg) << std::endl;
    
    CThostFtdcReqUserLoginField req = {0};
    strncpy(req.BrokerID, m_config->szBrokerID.c_str(), sizeof(req.BrokerID));
    strncpy(req.UserID, m_config->szUser.c_str(), sizeof(req.UserID));
    strncpy(req.Password, m_config->szPass.c_str(), sizeof(req.Password));

    int flag = m_client->ReqUserLogin(&req, 1);

    std::cout << "ReqUserLogin: " << flag << std::endl;
};


///登录请求响应
void TiCtpTraderClient::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    std::cout << "OnRspUserLogin:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << " ErrorMsg:" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg) << std::endl;
 
    nReqId++;
    CThostFtdcSettlementInfoConfirmField req = {0};
    strncpy(req.BrokerID, m_config->szBrokerID.c_str(), sizeof(req.BrokerID));
    strncpy(req.InvestorID, m_config->szUser.c_str(), sizeof(req.InvestorID));

    m_client->ReqSettlementInfoConfirm(&req, nReqId);
    /*
    TiReqOrderInsert req;
    memset(&req, 0, sizeof(TiReqOrderInsert));
    strcpy(req.szSymbol, "IF2501");
    strcpy(req.szExchange, "CF");
    req.nTradeSideType = 'S';
    req.nBusinessType = '0';
    req.nOffsetType = 'C';
    req.nOrderPrice = 3713.2;
    req.nOrderVol = 1;

    orderInsert(&req);
    */
};

////////////////////////////////////////////////////////////////////////
// 订单回调
////////////////////////////////////////////////////////////////////////
///报单录入请求响应
void TiCtpTraderClient::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    std::cout << "OnRspOrderInsert:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << " ErrorMsg:" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg) << std::endl;
};
///报单操作请求响应
void TiCtpTraderClient::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    std::cout << "OnRspOrderAction:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << " ErrorMsg:" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg) << std::endl;
};

///错误应答
void TiCtpTraderClient::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    std::cout << "OnRspError:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << " ErrorMsg:" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg) << std::endl;
};
///报单通知
void TiCtpTraderClient::OnRtnOrder(CThostFtdcOrderField *pOrder){
    LOG(INFO) << "[OnRtnOrder]" << pOrder->OrderSysID <<
        " , CombOffsetFlag: " << pOrder->CombOffsetFlag <<
        " , LimitPrice: " << pOrder->LimitPrice <<
        " , VolumeTotalOriginal: " << pOrder->VolumeTotalOriginal <<
        " , Direction: " << pOrder->Direction <<
        " , VolumeCondition: " << pOrder->VolumeCondition <<
        " , MinVolume: " << pOrder->MinVolume <<
        " , ContingentCondition: " << pOrder->ContingentCondition <<
        " , StopPrice: " << pOrder->StopPrice <<
        " , RequestID: " << pOrder->RequestID <<
        " , OrderLocalID: " << pOrder->OrderLocalID <<
        " , ExchangeID: " << pOrder->ExchangeID <<
        " , ParticipantID: " << pOrder->ParticipantID <<
        " , ClientID: " << pOrder->ClientID <<
        " , TraderID: " << pOrder->TraderID <<
        " , TradingDay: " << pOrder->TradingDay <<
        " , SettlementID: " << pOrder->SettlementID <<
        " , OrderSysID: " << pOrder->OrderSysID <<
        " , OrderSource: " << pOrder->OrderSource <<
        " , OrderStatus: " << pOrder->OrderStatus <<
        " , OrderType: " << pOrder->OrderType <<
        " , VolumeTraded: " << pOrder->VolumeTraded <<
        " , VolumeTotal: " << pOrder->VolumeTotal <<
        " , InsertDate: " << pOrder->InsertDate <<
        " , InsertTime: " << pOrder->InsertTime <<
        " , ActiveTime: " << pOrder->ActiveTime << std::endl;

    
    int64_t req_id = pOrder->RequestID;
    std::string order_stream_id = pOrder->OrderSysID;

    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(req_id, order_stream_id);
    if (!order_ptr)
    {
        order_ptr = std::make_shared<TiRtnOrderStatus>();
        memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
        order_ptr->nReqId = req_id;
        order_ptr->nOrderId = std::stoll(pOrder->OrderRef);
        strcpy(order_ptr->szOrderStreamId, order_stream_id.c_str());
        updateOrderMap(order_ptr);
    }

    strcpy(order_ptr->szAccount, pOrder->InvestorID);
    strcpy(order_ptr->szName, pOrder->InstrumentID);
    if (strcmp(pOrder->ExchangeID, "CFFEX") == 0)
    {
        strcpy(order_ptr->szExchange, "CF");
    }else{
        strcpy(order_ptr->szExchange, pOrder->ExchangeID);
    }
    
    if(pOrder->Direction == THOST_FTDC_D_Buy)
    {
        order_ptr->nTradeSideType = TI_TradeSideType_Buy;
    }
    if(pOrder->Direction == THOST_FTDC_D_Sell)
    {
        order_ptr->nTradeSideType = TI_TradeSideType_Sell;
    }
    order_ptr->nOffsetType = getOffsetType(pOrder->CombOffsetFlag[0]);
    
    order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(pOrder->InsertDate, pOrder->InsertTime);
    order_ptr->nLastUpdateTimestamp = datetime::get_now_timestamp_ms();
    order_ptr->nStatus = getOrderStatus(pOrder->OrderStatus);
    order_ptr->nSubmitVol = pOrder->VolumeTotalOriginal;
    order_ptr->nDealtPrice = pOrder->LimitPrice;
    order_ptr->nDealtVol = pOrder->VolumeTraded;
    order_ptr->nOrderPrice = pOrder->LimitPrice;
    order_ptr->nOrderVol = pOrder->VolumeTotalOriginal;

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
};
///成交通知
void TiCtpTraderClient::OnRtnTrade(CThostFtdcTradeField *pTrade){
    LOG(INFO) << "[OnRtnTrade]" << pTrade->TradeID <<
        " , OrderSysID: " << pTrade->OrderSysID <<
        " , TradeTime: " << pTrade->TradeTime <<
        " , ExchangeID: " << pTrade->ExchangeID <<
        " , ParticipantID: " << pTrade->ParticipantID <<
        " , ClientID: " << pTrade->ClientID <<
        " , TraderID: " << pTrade->TraderID <<
        " , TradingDay: " << pTrade->TradingDay <<
        " , SettlementID: " << pTrade->SettlementID <<
        " , OrderSysID: " << pTrade->OrderSysID <<
        " , OrderLocalID: " << pTrade->OrderLocalID <<
        " , TradeID: " << pTrade->TradeID <<
        " , InstrumentID: " << pTrade->InstrumentID <<
        " , Direction: " << pTrade->Direction <<
        " , OffsetFlag: " << pTrade->OffsetFlag <<
        " , Price: " << pTrade->Price <<
        " , Volume: " << pTrade->Volume <<
        " , TradeType: " << pTrade->TradeType <<
        " , PriceSource: " << pTrade->PriceSource <<
        " , TradeSource: " << pTrade->TradeSource << std::endl;

    std::cout << "OnRtnTrade:" << pTrade->TradeID << std::endl;
    
    std::string order_stream_id = pTrade->OrderSysID;

    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = std::stoll(pTrade->OrderRef);
    strcpy(match_ptr->szOrderStreamId, order_stream_id.c_str());
    strcpy(match_ptr->szStreamId, pTrade->TradeID);
    strcpy(match_ptr->szAccount, pTrade->InvestorID);
    strcpy(match_ptr->szSymbol, pTrade->InstrumentID);
    if (strcmp(pTrade->ExchangeID, "CFFEX") == 0)
    {
        strcpy(match_ptr->szExchange, "CF");
    }else{
        strcpy(match_ptr->szExchange, pTrade->ExchangeID);
    }

    match_ptr->nMatchPrice = pTrade->Price;
    match_ptr->nMatchVol = pTrade->Volume;

    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(pTrade->TradeDate, pTrade->TradeTime);
    updateOrderMatch(match_ptr);

    std::shared_ptr<TiRtnOrderStatus> order_ptr = getOrderPtr(0, order_stream_id);
    if (order_ptr != nullptr)
    {
        strcpy(match_ptr->szOrderStreamId, order_ptr->szOrderStreamId);
        strcpy(match_ptr->szName, order_ptr->szName);
        match_ptr->nReqId = order_ptr->nReqId;
        match_ptr->nTradeSideType = order_ptr->nTradeSideType;

        order_ptr->nLastUpdateTimestamp = match_ptr->nMatchTimestamp;
        order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;
    }
    
    m_cb->OnRtnOrderMatchEvent(match_ptr.get());
};
///报单录入错误回报
void TiCtpTraderClient::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo){
    std::cout << "OnErrRtnOrderInsert:" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg).c_str() << std::endl;
};
///报单操作错误回报
void TiCtpTraderClient::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo){
    std::cout << "OnErrRtnOrderAction:" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg).c_str() << std::endl;
};
////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiCtpTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations       = string(_iniFile["ti_ctp_trader_client"]["locations"]);

    m_config->szBrokerID        = string(_iniFile["ti_ctp_trader_client"]["broker_id"]);
    m_config->szAppID           = string(_iniFile["ti_ctp_trader_client"]["app_id"]);
    m_config->szAuthCode        = string(_iniFile["ti_ctp_trader_client"]["auth_code"]);
    
    m_config->szUser            = string(_iniFile["ti_ctp_trader_client"]["user"]);
    m_config->szPass            = string(_iniFile["ti_ctp_trader_client"]["pass"]);
    
    if( m_config->szLocations.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty() |
        m_config->szBrokerID.empty() |
        m_config->szAuthCode.empty() |
        m_config->szAuthCode.empty() )
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};


TI_OrderStatusType TiCtpTraderClient::getOrderStatus(TThostFtdcOrderStatusType status)
{
    switch (status)
    {
    case THOST_FTDC_OST_NoTradeQueueing:
        return TI_OrderStatusType_queued;
    case THOST_FTDC_OST_PartTradedQueueing:
        return TI_OrderStatusType_queued;
    case THOST_FTDC_OST_Canceled:
        return TI_OrderStatusType_removed;
    case THOST_FTDC_OST_Unknown:
        return TI_OrderStatusType_fail;
    case THOST_FTDC_OST_AllTraded:
        return TI_OrderStatusType_dealt;
    default:
        return TI_OrderStatusType_unAccept;
    }
    return TI_OrderStatusType_unAccept;
};

TI_OffsetType TiCtpTraderClient::getOffsetType(TThostFtdcOffsetFlagType offset)
{
    switch (offset)
    {
    case THOST_FTDC_OF_Open:
        return TI_OffsetType_Open;
    case THOST_FTDC_OF_Close:
        return TI_OffsetType_Close;
    case THOST_FTDC_OF_ForceClose:
        return TI_OffsetType_Close;
    case THOST_FTDC_OF_CloseToday:
        return TI_OffsetType_Close;
    case THOST_FTDC_OF_CloseYesterday:
        return TI_OffsetType_Close;
    case THOST_FTDC_OF_ForceOff:
        return TI_OffsetType_Close;
    case THOST_FTDC_OF_LocalForceClose:
        return TI_OffsetType_Close;
    default:
        return TI_OffsetType_Close;
    }
    return TI_OffsetType_Close;
};

std::shared_ptr<TiRtnOrderStatus> TiCtpTraderClient::getOrderPtr(int64_t req_id, std::string order_stream_id)
{
    if (m_order_map.find(order_stream_id) != m_order_map.end())
    {
        return m_order_map[order_stream_id];
    }
    if (m_order_req_map.find(req_id) != m_order_req_map.end())
    {
        return m_order_req_map[req_id];
    }
    return NULL;
};

void TiCtpTraderClient::updateOrderMap(std::shared_ptr<TiRtnOrderStatus> order_ptr)
{
    auto iter = m_order_map.find(order_ptr->szOrderStreamId);
    if (iter == m_order_map.end())
    {
        m_order_map[order_ptr->szOrderStreamId] = order_ptr;

        auto req_iter = m_order_req_map.find(order_ptr->nReqId);
        if (req_iter != m_order_req_map.end())
        {
            m_order_req_map.erase(req_iter);
        }
    }
};

void TiCtpTraderClient::updateOrderMatch(std::shared_ptr<TiRtnOrderMatch> match_ptr)
{
    m_matches_map.insert(std::pair<std::string, std::shared_ptr<TiRtnOrderMatch>>(match_ptr->szOrderStreamId, match_ptr));
};

////////////////////////////////////////////////////////////////////////
// 公用方法
////////////////////////////////////////////////////////////////////////

void TiCtpTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    if (m_client)   //如果已经实例了就退出
    {
        return;
    }
    
};

int TiCtpTraderClient::orderInsert(TiReqOrderInsert* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    req->nReqId = ++nReqId;


    CThostFtdcInputOrderField order;
    memset(&order, 0, sizeof(order));

    // 设置下单信息
    order.RequestID = req->nReqId;
    if (!strcmp(req->szExchange, "CF"))
    {
        strncpy(order.ExchangeID, "CFFEX", sizeof(order.ExchangeID));       // 交易所
    }else{
        strncpy(order.ExchangeID, req->szExchange, sizeof(order.ExchangeID));       // 交易所
    }

    strncpy(order.BrokerID, m_config->szBrokerID.c_str(), sizeof(order.BrokerID));
    strncpy(order.InvestorID, m_config->szUser.c_str(), sizeof(order.InvestorID));
    strncpy(order.InstrumentID, req->szSymbol, sizeof(order.InstrumentID));  // 指定期货合约
    strncpy(order.UserID, m_config->szUser.c_str(), sizeof(order.UserID));
    order.OrderPriceType = THOST_FTDC_OPT_LimitPrice;  // 限价单

    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Sell:
        order.Direction = THOST_FTDC_D_Sell;  // 买入
        break;
    case TI_TradeSideType_Buy:
        order.Direction = THOST_FTDC_D_Buy;  // 买入
        break;
    }

    switch (req->nOffsetType)
    {
    case TI_OffsetType_Open:
        order.CombOffsetFlag[0] = THOST_FTDC_OF_Open;  // 开仓
        break;
    case TI_OffsetType_Close:
        order.CombOffsetFlag[0] = THOST_FTDC_OF_Close;  // 平仓
        break;
    }

    order.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;  // 投机
    order.LimitPrice = req->nOrderPrice;  // 设置买入价格
    order.VolumeTotalOriginal = req->nOrderVol;  // 设置买入数量
    order.MinVolume = 1;  // 最小成交量
    order.ContingentCondition = THOST_FTDC_CC_Immediately;  // 立即触发
    order.StopPrice = 0;  // 止损价
    order.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;  // 非强平
    order.IsAutoSuspend = 0;  // 非自动挂起
    order.TimeCondition = THOST_FTDC_TC_GFD;  // 当日有效
    order.VolumeCondition = THOST_FTDC_VC_AV;  // 任意成交量
    //strcpy(order.OrderRef, "000000000001");  // 投资单元代码
    sprintf(order.OrderRef, "%d", req->nReqId);  // 投资单元代码
    

    std::cout << "OrderRef: " << order.OrderRef << std::endl;
    order.IsSwapOrder = 0;  // 非互换单
    order.UserForceClose = 0;  // 非强平


    int result = m_client->ReqOrderInsert(&order, nReqId);
    if (result != 0) {
        std::cout << "Failed to place order!" << std::endl;
    } else {
        std::cout << "Order placed successfully! " << order.InstrumentID << std::endl;
    }

    return -1;
};

int TiCtpTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

TiRtnOrderStatus* TiCtpTraderClient::getOrderStatus(int64_t req_id, std::string order_stream_id)
{
    if (m_order_map.find(order_stream_id) != m_order_map.end())
    {
        return m_order_map[order_stream_id].get();
    }
    if (m_order_req_map.find(req_id) != m_order_req_map.end())
    {
        return m_order_req_map[req_id].get();
    }
    return NULL;
};

TiRtnOrderStatus* TiCtpTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    if (m_order_req_map.find(req_id) != m_order_req_map.end())
    {
        return m_order_req_map[req_id].get();
    }
    return NULL;
};

int TiCtpTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

int TiCtpTraderClient::QueryOrders()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

int TiCtpTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

int TiCtpTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};