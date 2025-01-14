#ifndef TI_HX_QUOTE_CLIENT_H
#define TI_HX_QUOTE_CLIENT_H

#include <string>
#include "TORATstpXMdApi.h"     //level 1 api
#include "TORATstpLev2MdApi.h"  //level 2 api
#include "ti_quote_callback.h"
#include "ti_hx_quote_l1_client.h"

using namespace TORALEV2API;

class TiHxQuoteClient: public CTORATstpLev2MdSpi, TORALEV1API::CTORATstpXMdSpi
{
public:
    typedef struct ConfigInfo
    {
        std::string szL1Host;        //level 1 host

        std::string szL2ShHost;
        std::string szL2SzHost;

        std::string szCore;                 //绑核参数

        bool        bIsMulticast;           //是否组播行情
        std::string szL2Multicast;          //组播行情地址
        std::string szL2MulticastInterface; //组播行情接收网卡地址

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
    
    TiHxQuoteL1Client* m_l1_client;

    CTORATstpLev2MdApi *m_multicast_api;
    LoginStatus* m_multicast_status;
    CTORATstpLev2MdApi *m_sh_api;
    CTORATstpLev2MdApi *m_sz_api;
    LoginStatus* m_sh_status;
    LoginStatus* m_sz_status;
    unsigned int m_trading_day;

    TiQuoteCallback* m_cb;
    TiQuoteSnapshotStockField   m_snapStockCash;
    TiQuoteSnapshotIndexField   m_snapIndexCash;
    TiQuoteOrderField           m_orderCash;
    TiQuoteMatchesField         m_matchCash;

public:
    TiHxQuoteClient(std::string configPath, TiQuoteCallback* userCb);
    ~TiHxQuoteClient();



public:
    virtual void OnFrontConnected();

    virtual void OnRspUserLogin(
        CTORATstpRspUserLoginField *pRspUserLogin, 
        CTORATstpRspInfoField *pRspInfo, 
        int nRequestID, bool bIsLast);

    virtual void OnRspSubMarketData(
        CTORATstpSpecificSecurityField *pSpecificSecurity,
        CTORATstpRspInfoField *pRspInfo, 
        int nRequestID, bool bIsLast);
    ///订阅指数行情应答
    virtual void OnRspSubIndex(CTORATstpSpecificSecurityField *pSpecificSecurity, CTORATstpRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    ///订阅逐笔成交应答(深圳非债券类、深圳可转债)
    virtual void OnRspSubTransaction(CTORATstpSpecificSecurityField *pSpecificSecurity, CTORATstpRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    ///订阅逐笔委托应答(深圳非债券类、深圳可转债)
    virtual void OnRspSubOrderDetail(CTORATstpSpecificSecurityField *pSpecificSecurity, CTORATstpRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

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
    //上海NGTS非债券逐笔数据通知
    virtual void OnRtnNGTSTick(CTORATstpLev2NGTSTickField *pTick);

	
    virtual void OnFrontDisconnected(int nReason){};

private:
    int loadConfig(std::string iniFileName);

    void WriteToCSV(const CTORATstpLev2TransactionField& data, const std::string& fileName);
    void WriteToCSV(const CTORATstpLev2OrderDetailField& data, const std::string& fileName);
    void WriteToCSV(const CTORATstpLev2NGTSTickField& data, const std::string& fileName);

public:
	void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif