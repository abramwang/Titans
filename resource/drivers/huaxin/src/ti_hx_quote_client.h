#ifndef TI_HX_QUOTE_CLIENT_H
#define TI_HX_QUOTE_CLIENT_H

#include <string>
#include "TORATstpLev2MdApi.h"
#include "ti_quote_callback.h"

using namespace TORALEV2API;

class TiHxQuoteClient: public CTORATstpLev2MdSpi
{
public:
    typedef struct ConfigInfo
    {
        std::string szL2ShHost;
        std::string szL2SzHost;

        std::string szProductInfo;
        std::string szAccount;
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
    CTORATstpLev2MdApi *m_sh_api;
    CTORATstpLev2MdApi *m_sz_api;
    LoginStatus* m_sh_status;
    LoginStatus* m_sz_status;
    unsigned int m_trading_day;

    TiQuoteCallback* m_cb;
    TiQuoteSnapshotStockField   m_snapStockCash;
    TiQuoteSnapshotIndexField   m_snapIndexCash;
    TiQuoteOrderField           m_orderCash;
    TiQuoteMatchesField           m_matchCash;

public:
    TiHxQuoteClient(std::string configPath, TiQuoteCallback* userCb);
    ~TiHxQuoteClient();

    virtual void OnFrontConnected();

    virtual void OnRspUserLogin(
        CTORATstpRspUserLoginField *pRspUserLogin, 
        CTORATstpRspInfoField *pRspInfo, 
        int nRequestID, bool bIsLast);

    virtual void OnRspSubMarketData(
        CTORATstpSpecificSecurityField *pSpecificSecurity,
        CTORATstpRspInfoField *pRspInfo, 
        int nRequestID, bool bIsLast);

    ///行情通知(上海非债券类、深圳非债券类、深圳可转债)
    virtual void OnRtnMarketData(CTORATstpLev2MarketDataField *pDepthMarketData,
                                 const int FirstLevelBuyNum, const int FirstLevelBuyOrderVolumes[],
                                 const int FirstLevelSellNum, const int FirstLevelSellOrderVolumes[]);
	//指数行情通知
	virtual void OnRtnIndex(CTORATstpLev2IndexField *pIndex);
	//逐笔委托通知(深圳非债券类、深圳可转债)
	virtual void OnRtnOrderDetail(CTORATstpLev2OrderDetailField *pOrderDetail);
    //逐笔成交通知(深圳非债券类、深圳可转债)
	virtual void OnRtnTransaction(CTORATstpLev2TransactionField *pTransaction);
	
    virtual void OnFrontDisconnected(int nReason){};

private:
    int loadConfig(std::string iniFileName);
public:
	void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif

