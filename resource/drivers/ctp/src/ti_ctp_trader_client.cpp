#include "ti_ctp_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"

#include <atomic>


TiCtpTraderClient::TiCtpTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;

    m_client = CThostFtdcTraderApi::CreateFtdcTraderApi();
    m_client->RegisterSpi(this);
    m_client->SubscribePrivateTopic(THOST_TERT_RESTART);
    m_client->SubscribePublicTopic(THOST_TERT_RESTART);
    m_client->RegisterFront("tcp://124.93.32.65:31254");

    m_client->Init();
    //m_client->Join();

    nSessionId = 0;

    m_cb = userCb;

    loadConfig(configPath);
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
    // 连接成功后可以进行交易操作

    //return;
    ///*
    CThostFtdcReqAuthenticateField req2 = {0};
    strcpy(req2.BrokerID, "1022");
    strcpy(req2.UserID, "00206");
    strcpy(req2.UserProductInfo, "client_sntz_1.0");
    strcpy(req2.AppID, "client_sntz_1.0");
    strcpy(req2.AuthCode, "S78K2HXU9KWLTWF6");

    
    int flag2 = m_client->ReqAuthenticate(&req2, 2);
    
    std::cout << "ReqAuthenticate: " << flag2 << std::endl;
    //*/
};

void TiCtpTraderClient::OnFrontDisconnected(int nReason)
{
    std::cout << "交易前置连接断开，原因：" << nReason << std::endl;
    // 连接断开后的处理
};

void TiCtpTraderClient::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    
    std::cout << "OnRspAuthenticate：" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << std::endl;

    
    CThostFtdcReqUserLoginField req = {0};
    strcpy(req.BrokerID, "1022");
    strcpy(req.UserID, "00206");
    strcpy(req.Password, "testdce");

    int flag = m_client->ReqUserLogin(&req, 1);

    std::cout << "ReqUserLogin: " << flag << std::endl;

};


///登录请求响应
void TiCtpTraderClient::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    std::cout << "OnRspUserLogin:" << nRequestID << " ErrorID:" << pRspInfo->ErrorID << std::endl;
};


void TiCtpTraderClient::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::cout << "错误回报：" << pRspInfo->ErrorMsg << std::endl;
    // 错误回报的处理
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
    m_config->szLocations   = string(_iniFile["ti_ctp_trader_client"]["locations"]);
    m_config->szLocations2   = string(_iniFile["ti_ctp_trader_client"]["locations2"]);
    
    m_config->szUser                = string(_iniFile["ti_ctp_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_ctp_trader_client"]["pass"]);

    m_config->szSoftwareName        = string(_iniFile["ti_ctp_trader_client"]["software_name"]);
    m_config->szSoftwareVersion     = string(_iniFile["ti_ctp_trader_client"]["software_version"]);

    m_config->szBranchCode     = string(_iniFile["ti_ctp_trader_client"]["branch_code"]);
    m_config->szCustomerId     = string(_iniFile["ti_ctp_trader_client"]["customer_id"]);
    m_config->szFundAccount     = string(_iniFile["ti_ctp_trader_client"]["fund_account"]);
    m_config->szFundPass     = string(_iniFile["ti_ctp_trader_client"]["fund_pass"]);
    
    if( m_config->szLocations.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty() |
        m_config->szBranchCode.empty() |
        m_config->szCustomerId.empty() |
        m_config->szFundAccount.empty() |
        m_config->szFundPass.empty() )
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