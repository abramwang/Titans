#ifndef TI_HX_QUOTE_L1_CLIENT_H
#define TI_HX_QUOTE_L1_CLIENT_H

#include <string>
#include "TORATstpXMdApi.h"     //level 1 api
#include "ti_quote_callback.h"

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
    TiQuoteSnapshotIndexField   m_snapIndexCash;
    TiQuoteOrderField           m_orderCash;
    TiQuoteMatchesField         m_matchCash;

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
        TORALEV1API::CTORATstpRspInfoField *pRspInfo, 
        int nRequestID, bool bIsLast);

};

#endif //TI_HX_QUOTE_L1_CLIENT_H