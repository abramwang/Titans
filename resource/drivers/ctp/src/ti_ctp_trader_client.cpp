#include "ti_ctp_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"

#include <atomic>


TiCtpTraderClient::TiCtpTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;

    loadConfig(configPath);

    m_client = CThostFtdcTraderApi::CreateFtdcTraderApi();
    m_client->RegisterSpi(this);
    m_client->SubscribePrivateTopic(THOST_TERT_RESTART);
    m_client->SubscribePublicTopic(THOST_TERT_RESTART);
    m_client->RegisterFront((char*)m_config->szLocations.c_str());

    m_client->Init();
    m_client->Join();

    nSessionId = 0;

    m_cb = userCb;

    nReqId = 100;   //跳过xtp client设置成交模式的区段
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
    
    int flag2 = m_client->ReqAuthenticate(&req, 2);
    std::cout << "ReqAuthenticate: " << flag2 << std::endl;
};

void TiCtpTraderClient::OnFrontDisconnected(int nReason)
{
    std::cout << "交易前置连接断开，原因：" << nReason << std::endl;
    // 连接断开后的处理
};

void TiCtpTraderClient::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    
    std::cout << "OnRspAuthenticate：" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << std::endl;
    
    CThostFtdcReqUserLoginField req = {0};
    strncpy(req.BrokerID, m_config->szBrokerID.c_str(), sizeof(req.BrokerID));
    strncpy(req.UserID, m_config->szUser.c_str(), sizeof(req.UserID));
    strncpy(req.Password, m_config->szPass.c_str(), sizeof(req.Password));

    int flag = m_client->ReqUserLogin(&req, 1);

    std::cout << "ReqUserLogin: " << flag << std::endl;
};


///登录请求响应
void TiCtpTraderClient::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    std::cout << "OnRspUserLogin:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << std::endl;
};

////////////////////////////////////////////////////////////////////////
// 订单回调
////////////////////////////////////////////////////////////////////////
///报单录入请求响应
void TiCtpTraderClient::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    std::cout << "OnRspOrderInsert:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << std::endl;
};
///报单操作请求响应
void TiCtpTraderClient::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    std::cout << "OnRspOrderAction:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << std::endl;
};

///错误应答
void TiCtpTraderClient::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
    std::cout << "OnRspError:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << std::endl;
};
///报单通知
void TiCtpTraderClient::OnRtnOrder(CThostFtdcOrderField *pOrder){
    std::cout << "OnRtnOrder:" << pOrder->OrderSysID << std::endl;
};
///成交通知
void TiCtpTraderClient::OnRtnTrade(CThostFtdcTradeField *pTrade){
    std::cout << "OnRtnTrade:" << pTrade->TradeID << std::endl;
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

    return -1;
};

int TiCtpTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

TiRtnOrderStatus* TiCtpTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
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