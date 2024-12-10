#ifndef TI_CTP_TRADER_CLIENT_H
#define TI_CTP_TRADER_CLIENT_H

#include <string>
#include <iostream>
#include "ThostFtdcTraderApi.h"
#include "ti_trader_client.h"
#include "ti_trader_callback.h"

class TiCtpTraderClient: 
    public CThostFtdcTraderSpi, public TiTraderClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szLocations;

        std::string szBrokerID;
        std::string szAppID;    
        std::string szAuthCode;

        std::string szUser;     //投资者账户
        std::string szPass;     //投资者密码

    } ConfigInfo;
    
private:
    int nReqId;
    ConfigInfo* m_config;
    unsigned int m_trading_day;

    CThostFtdcTraderApi * m_client;
    std::map<int32_t, int32_t> m_report_sync;
    unsigned long long nSessionId;

    json m_json_rsp;
    std::unordered_map<int64_t, std::shared_ptr<TiRtnOrderStatus>> m_order_req_map;     //req_id, order_status
    std::unordered_map<int64_t, std::shared_ptr<TiRtnOrderStatus>> m_order_map;         //order_id, order_status
    std::multimap<int64_t, std::shared_ptr<TiRtnOrderMatch>> m_matches_map;             //order_id, m_matches_map

    TiTraderCallback* m_cb;
public:
    TiCtpTraderClient(std::string configPath, TiTraderCallback* userCb);
    virtual ~TiCtpTraderClient();

public:
    virtual void OnFrontConnected();
    virtual void OnFrontDisconnected(int nReason);

    virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

public:
	///报单录入请求响应
	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///报单操作请求响应
	virtual void OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	///报单通知
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	///成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);
	///报单录入错误回报
	virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo);
	///报单操作错误回报
	virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo);

private:
    int loadConfig(std::string iniFileName);
    
public:
	void connect();
    int orderInsertBatch(std::vector<TiReqOrderInsert> &req_vec, std::string account_id){};
    int orderInsert(TiReqOrderInsert* req);
    int orderDelete(TiReqOrderDelete* req);
    TiRtnOrderStatus* getOrderStatus(int64_t req_id, int64_t order_id);
    
    
    int QueryAsset();
    int QueryOrders();
    int QueryMatches();
    int QueryPositions();
    
};

#endif

