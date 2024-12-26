#ifndef TI_HX_QUOTE_L1_CLIENT_H
#define TI_HX_QUOTE_L1_CLIENT_H

#include <string>
#include <memory>
#include <unordered_map>
#include "TORATstpXMdApi.h"     //level 1 api
#include "ti_quote_callback.h"
#include "ti_quote_tools.h"

class TiHxQuoteL1Client: TORALEV1API::CTORATstpXMdSpi
{
public:
    typedef struct LoginStatus
    {
        int nReqId;
        bool bLoginSuccess;
    } LoginStatus;

private:
    std::string m_host;
    std::string m_productInfo;
    std::string m_account;
    std::string m_pass;


    int nReqId;
    TORALEV1API::CTORATstpXMdApi *m_api;
    LoginStatus* m_status;

    unsigned int m_trading_day;

    TiQuoteCallback* m_cb;

    TiQuoteSnapshotStockField   m_snapStockCash;
    std::unordered_map<int64_t, std::shared_ptr<TiQuoteSnapshotStockField>> m_snapshot_map;
public:
    TiHxQuoteL1Client(TiQuoteCallback* userCb, 
        std::string host, 
        std::string productInfo, std::string account, std::string pass);
    virtual ~TiHxQuoteL1Client();



public:
    virtual void OnFrontConnected();

    virtual void OnRspUserLogin(
        TORALEV1API::CTORATstpRspUserLoginField *pRspUserLogin, 
        TORALEV1API::CTORATstpRspInfoField *pRspInfo, 
        int nRequestID);

    virtual void OnRspSubMarketData(
        TORALEV1API::CTORATstpSpecificSecurityField *pSpecificSecurity,
        TORALEV1API::CTORATstpRspInfoField *pRspInfo);
    
    virtual void OnRtnMarketData(TORALEV1API::CTORATstpMarketDataField *pMarketDataField);

public:
    void subData(const char* exchangeName, char* codeList[], size_t len);
    TiQuoteSnapshotStockField* GetStockSnapshot(const char* symbol, const char* exchange);

};

#endif //TI_HX_QUOTE_L1_CLIENT_H