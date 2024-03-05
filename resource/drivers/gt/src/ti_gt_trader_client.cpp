#include "ti_gt_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"

#include <atomic>

TiGtTraderClient::TiGtTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_client = NULL;

    nSessionId = 0;
    nOrderId = 1680000;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 100;   //跳过xtp client设置成交模式的区段
};


TiGtTraderClient::~TiGtTraderClient()
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

void TiGtTraderClient::onConnected(bool success, const char* errorMsg) 
{
    cout << "[onConnected] server connect " << (success ? string("success") : string("failure, err: ") + errorMsg) << endl;
    if (!success)
    {
        return;
    }
    std::cout << m_config->szUser.c_str() << " " << m_config->szPass.c_str() << std::endl;
    m_client->userLogin(m_config->szUser.c_str(), m_config->szPass.c_str(), nReqId++);
}

void TiGtTraderClient::onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error)
{
    cout << "[onUserLogin] login " << (error.isSuccess() ? "success" : string("failure, err: ") + error.errorMsg()) << endl;
}

void TiGtTraderClient::onRtnLoginStatus(const char* accountId, EBrokerLoginStatus status, int brokerType, const char* errorMsg)
{
    cout << "[onRtnLoginStatus] account id: " << accountId << ", type: " << brokerType << ", status: " << status << endl;

    switch (brokerType)
    {
    case AT_STOCK:
        m_config->m_strStockAccount = accountId;
        break;
    case AT_FUTURE:
        m_config->m_strFutureAccount = accountId;
        break;
    case AT_STOCK_OPTION:
        m_config->m_strOptionAccount = accountId;
        break;
    case AT_CREDIT:
        m_config->m_strCreditAccount = accountId;
        break;
    default:
        break;
    }

    m_client->reqAccountDetail(accountId, ++nReqId);
}

 // 资金账号信息
void TiGtTraderClient::onReqAccountDetail(const char* accountId, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error)
{
    cout << "[onReqAccountDetail]  资金账号 :" << data->m_strAccountID
        << "\n    账号状态:" << data->m_strStatus
        << "\n    交易日:" << data->m_strTradingDate
        << "\n    可用资金:" << data->m_dAvailable
        << "\n    净资产:" << data->m_dAssureAsset
        << "\n    总负债:" << data->m_dTotalDebit
        << "\n    期初权益:" << data->m_dPreBalance
        << endl;
}

    
////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiGtTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations           = string(_iniFile["ti_gt_trader_client"]["locations"]);
    
    m_config->szUser                = string(_iniFile["ti_gt_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_gt_trader_client"]["pass"]);
    
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


int TiGtTraderClient::orderInsertStock(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;

    return nReqId;
};
int TiGtTraderClient::orderInsertEtf(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;

    return nReqId;
};

void TiGtTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    if (m_client)   //如果已经实例了就退出
    {
        return;
    }

    m_client = XtTraderApi::createXtTraderApi(m_config->szLocations.c_str());
    if (NULL == m_client)
    {
        cout << "[init] create api failure" << endl;
        return;
    }

    m_client->setCallback(this);
    m_client->init("../config");

    m_client->join_async();
};


int TiGtTraderClient::orderInsert(TiReqOrderInsert* req){
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

int TiGtTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    TiRtnOrderStatus* order = getOrderStatus(-1, req->nOrderId);

    auto iter = m_order_map.begin();
    for (; iter != m_order_map.end(); iter++)
    {

        std::cout << iter->first << ":" << iter->second->szOrderStreamId << std::endl;
        /* code */
    }
    
    std::cout << order << std::endl;
    if(!order)
    {
        return -1;
    }

    return nReqId;
};

TiRtnOrderStatus* TiGtTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    if (m_order_map.find(order_id) != m_order_map.end())
    {
        return m_order_map[order_id].get();
    }
    auto iter = m_order_map.begin();
    for( ; iter != m_order_map.end(); iter++)
    {
        if (iter->second->nReqId == req_id)
        {
            return iter->second.get();
        }
    }
    return NULL;
};
    

int TiGtTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }


    return nReqId;
};

int TiGtTraderClient::QueryOrders()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    return nReqId;
};

int TiGtTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    

    return nReqId;
};

int TiGtTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};


int TiGtTraderClient::QueryETFFile()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

int TiGtTraderClient::QueryETFBasket()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    return nReqId;
};

