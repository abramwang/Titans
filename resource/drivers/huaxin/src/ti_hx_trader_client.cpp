#include "ti_hx_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"

#include <atomic>

TiHxTraderClient::TiHxTraderClient(std::string configPath, TiTraderCallback* userCb)
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


TiHxTraderClient::~TiHxTraderClient()
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
///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void TiHxTraderClient::OnFrontConnected(){
    LOG(INFO) << "[OnFrontConnected] ";
    std::cout << "[OnFrontConnected] " << std::endl;

    CTORATstpReqUserLoginField req;

    memset(&req, 0, sizeof(req));
    strcpy(req.LogInAccount, m_config->szUser.c_str());
    req.LogInAccountType = TORA_TSTP_LACT_UserID;

    strcpy(req.Password, m_config->szPass.c_str());
    strcpy(req.UserProductInfo, m_config->szUserProductInfo.c_str());
    strcpy(req.TerminalInfo, m_config->szTerminalInfo.c_str());
    
    int ret = m_client->ReqUserLogin(&req, ++nReqId);

    if (ret != 0)
    {   
        printf("ReqUserLogin fail, ret[%d]\n", ret);
    }
};

void TiHxTraderClient::OnFrontDisconnected(int nReason){
    LOG(INFO) << "[OnFrontDisconnected] nReason: " << nReason;
    std::cout << "[OnFrontDisconnected] nReason: " << nReason << std::endl;
};

void TiHxTraderClient::OnRspUserLogin(CTORATstpRspUserLoginField *pRspUserLoginField, CTORATstpRspInfoField *pRspInfoField, int nRequestID)
{
    std::cout << "[OnRspUserLogin] nRequestID: " << nRequestID << std::endl;
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspUserLogin fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, pRspInfoField->ErrorMsg);
        return;
    }
    printf("login success\n");
    CTORATstpQryShareholderAccountField req = {0};
    m_client->ReqQryShareholderAccount(&req, ++nReqId);
};

void TiHxTraderClient::OnRspQryShareholderAccount(CTORATstpShareholderAccountField *pShareholderAccountField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) 
{
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryShareholderAccount fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, pRspInfoField->ErrorMsg);
        return;
    }

    if (bIsLast){
        return;
    }

    printf("OnRspQryShareholderAccount ExchangeID %c, ShareholderID %s, InvestorID %s\n", 
        pShareholderAccountField->ExchangeID,
        pShareholderAccountField->ShareholderID,
        pShareholderAccountField->InvestorID);
    
    if (pShareholderAccountField->ExchangeID == TORA_TSTP_EXD_SSE)
    {
        m_config->szInvestorIDSH = pShareholderAccountField->InvestorID;
        m_config->szShareholderIdSH = pShareholderAccountField->ShareholderID;
    }
    if (pShareholderAccountField->ExchangeID == TORA_TSTP_EXD_SZSE)
    {
        m_config->szInvestorIDSZ = pShareholderAccountField->InvestorID;
        m_config->szShareholderIdSZ = pShareholderAccountField->ShareholderID;
    }
}; 	

///报单录入响应
void TiHxTraderClient::OnRspOrderInsert(CTORATstpInputOrderField *pInputOrderField, CTORATstpRspInfoField *pRspInfoField, int nRequestID)
{
    std::cout << "[OnRspOrderInsert] nRequestID: " << nRequestID << std::endl;
    if (pRspInfoField->ErrorID == 0)
    {
        printf("order insert success\n");
    }
    else
    {
        printf("order insert fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
};

    
////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiHxTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations           = string(_iniFile["ti_hx_trader_client"]["locations"]);
    
    m_config->szUser                = string(_iniFile["ti_hx_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_hx_trader_client"]["pass"]);

    m_config->szUserProductInfo     = string(_iniFile["ti_hx_trader_client"]["user_product_info"]);
    m_config->szDeviceID            = string(_iniFile["ti_hx_trader_client"]["device_id"]);
    m_config->szCertSerial          = string(_iniFile["ti_hx_trader_client"]["cert_serial"]);
    m_config->szProductInfo         = string(_iniFile["ti_hx_trader_client"]["product_info"]);
    m_config->szTerminalInfo        = string(_iniFile["ti_hx_trader_client"]["terminal_info"]);

    
    if( m_config->szLocations.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

TI_OrderStatusType TiHxTraderClient::getOrderStatus(TTORATstpOrderStatusType status)
{   
    /*
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
    */
   return TI_OrderStatusType_unAccept;
};

int TiHxTraderClient::orderInsertStock(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;

    CTORATstpInputOrderField msg;
    if (!strcmp(req->szExchange, "SH"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SSE;             // 市场ID，上海
        strcpy(msg.ShareholderID, m_config->szShareholderIdSH.c_str());               // 股东号
        strcpy(msg.InvestorID, m_config->szInvestorIDSH.c_str());                     // 投资者ID
    }
    if (!strcmp(req->szExchange, "SZ"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SZSE;             // 市场ID，上海
        strcpy(msg.ShareholderID, m_config->szShareholderIdSZ.c_str());               // 账户ID
        strcpy(msg.InvestorID, m_config->szInvestorIDSZ.c_str());                     // 投资者ID
    }
    strcpy(msg.SecurityID, req->szSymbol);               // 证券代码

    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Sell:
        msg.Direction = TORA_TSTP_D_Sell;   
        break;
    case TI_TradeSideType_Buy:
        msg.Direction = TORA_TSTP_D_Buy;   
        break;
    }
    msg.OrderPriceType = TORA_TSTP_OPT_LimitPrice;      // 订单类型，限价
    msg.LimitPrice = req->nOrderPrice;                  // 委托价格 N13(4)，21.0000元
    msg.VolumeTotalOriginal = req->nOrderVol;           //
    msg.TimeCondition = TORA_TSTP_TC_GFD;
    msg.VolumeCondition = TORA_TSTP_VC_AV;

    int ret = m_client->ReqOrderInsert(&msg, req->nReqId);
    if (ret != 0)
    {   
        printf("ReqOrderInsert fail, ret[%d]\n", ret);
    }
    return nReqId;
};
int TiHxTraderClient::orderInsertEtf(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;


    CTORATstpInputOrderField msg;
    if (!strcmp(req->szExchange, "SH"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SSE;             // 市场ID，上海
        strcpy(msg.ShareholderID, m_config->szShareholderIdSH.c_str());               // 股东号
        strcpy(msg.InvestorID, m_config->szInvestorIDSH.c_str());                     // 投资者ID
    }
    if (!strcmp(req->szExchange, "SZ"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SZSE;             // 市场ID，上海
        strcpy(msg.ShareholderID, m_config->szShareholderIdSZ.c_str());               // 账户ID
        strcpy(msg.InvestorID, m_config->szInvestorIDSZ.c_str());                     // 投资者ID
    }
    strcpy(msg.SecurityID, req->szSymbol);               // 证券代码

    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Purchase:
        msg.Direction = TORA_TSTP_D_ETFPur;   
        break;
    case TI_TradeSideType_Redemption:
        msg.Direction = TORA_TSTP_D_ETFRed;   
        break;
    }
    msg.OrderPriceType = TORA_TSTP_OPT_LimitPrice;      // 订单类型，限价
    msg.LimitPrice = req->nOrderPrice;                  // 委托价格 N13(4)，21.0000元
    msg.VolumeTotalOriginal = req->nOrderVol;           //
    msg.TimeCondition = TORA_TSTP_TC_GFD;
    msg.VolumeCondition = TORA_TSTP_VC_AV;

    int ret = m_client->ReqOrderInsert(&msg, req->nReqId);
    if (ret != 0)
    {   
        printf("ReqOrderInsert fail, ret[%d]\n", ret);
    }
    return nReqId;
};

void TiHxTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    if (m_client)   //如果已经实例了就退出
    {
        return;
    }
    m_client = CTORATstpTraderApi::CreateTstpTraderApi();
    m_client->RegisterSpi(this);
    m_client->RegisterFront((char*)m_config->szLocations.c_str());
    m_client->SubscribePrivateTopic(TORA_TERT_QUICK);
    m_client->SubscribePublicTopic(TORA_TERT_RESTART);
    m_client->Init();
};

int TiHxTraderClient::orderInsert(TiReqOrderInsert* req){
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

int TiHxTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

TiRtnOrderStatus* TiHxTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
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
    

int TiHxTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    /*
    ATPClientSeqIDType seq_id = ++nReqId;;
	ATPReqFundQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);

    
    //strncpy(msg.account_id, m_config->szShareholderIdSZ.c_str(), 13);               // 账户ID

    m_client->ReqFundQuery(&msg);
    */

    return nReqId;
};

int TiHxTraderClient::QueryOrders()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    /*
    ATPClientSeqIDType seq_id = ++nReqId;;
	ATPReqOrderQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);

    m_client->ReqOrderQuery(&msg);
    */
    return nReqId;
};

int TiHxTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    /*
    ATPClientSeqIDType seq_id = ++nReqId;;
	ATPReqTradeOrderQueryMsg msg;

    strncpy(msg.cust_id, m_config->szCustomerId.c_str(), 17);                 // 客户号ID
    strncpy(msg.fund_account_id, m_config->szFundAccount.c_str(), 17);        // 资金账户ID
    strncpy(msg.account_id, m_config->szShareholderIdSH.c_str(), 13);               // 账户ID
	msg.client_seq_id = seq_id;
	strncpy(msg.password, m_config->szFundPass.c_str(),129);

    m_client->ReqTradeOrderQuery(&msg);
    */
    return nReqId;
};

int TiHxTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    /*
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
    */
    return nReqId;
};