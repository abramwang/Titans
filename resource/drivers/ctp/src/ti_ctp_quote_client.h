#ifndef TI_CTP_QUOTE_CLIENT_H
#define TI_CTP_QUOTE_CLIENT_H

#include <string>
#include <iostream>
#include "ThostFtdcMdApi.h"
#include "ti_quote_client.h"
#include "ti_quote_callback.h"


class TiCtpQuoteClient: 
    public CThostFtdcMdSpi, public TiQuoteClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szLocation;
        
        std::string szBrokerId;
        std::string szUser;
        std::string szPass;
    } ConfigInfo;
    
    typedef struct LoginStatus
    {
        int nReqId;
        bool bLoginSuccess;
    } LoginStatus;

private:
    int nReqId;
    ConfigInfo* m_config;
    unsigned int m_trading_day;

    CThostFtdcMdApi*   m_api;

    TiQuoteCallback* m_cb;

    
    TiQuoteSnapshotFutureField   m_snapCash;
public:
    TiCtpQuoteClient(std::string configPath, TiQuoteCallback* userCb);
    virtual ~TiCtpQuoteClient();

public:
    virtual void OnFrontConnected();

    virtual void OnFrontDisconnected(int nReason);

    virtual void OnRspUserLogin(
        CThostFtdcRspUserLoginField *pRspUserLogin, 
        CThostFtdcRspInfoField *pRspInfo, 
        int nRequestID, 
        bool bIsLast);

    virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅询价应答
	virtual void OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅询价应答
	virtual void OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	///询价通知
	virtual void OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp);
private:
    int loadConfig(std::string iniFileName);
public:
	void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif

