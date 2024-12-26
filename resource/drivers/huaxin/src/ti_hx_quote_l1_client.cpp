#include <iostream>
#include <iniFile.h>
#include <glog/logging.h>
#include <string.h>
#include "ti_encoding_tool.h"
#include "ti_hx_quote_l1_client.h"
#include "ti_quote_struct.h"
#include "datetime.h"

using namespace std;

TiHxQuoteL1Client::TiHxQuoteL1Client(TiQuoteCallback* userCb, 
        std::string host, 
        std::string productInfo, std::string account, std::string pass)
{
    m_cb = userCb;
    m_host = host;
    m_productInfo = productInfo;
    m_account = account;
    m_pass = pass;
    m_api = TORALEV1API::CTORATstpXMdApi::CreateTstpXMdApi();
    m_api->RegisterSpi(this);
    m_api->RegisterFront(const_cast<char*>(m_host.c_str()));
    m_api->Init();
    m_status = NULL;
    m_trading_day = 0;
};

TiHxQuoteL1Client::~TiHxQuoteL1Client(){

};

void TiHxQuoteL1Client::OnFrontConnected()
{   
    printf("TiHxQuoteL1Client::OnFrontConnected\n");

    TORALEV1API::CTORATstpReqUserLoginField req_user_login_field;
    memset(&req_user_login_field, 0, sizeof(req_user_login_field));
    strcpy(req_user_login_field.LogInAccount, m_account.c_str());
    req_user_login_field.LogInAccountType = TORALEV1API::TORA_TSTP_LACT_UserID;
    // req_user_login_field.AuthMode = TORA_TSTP_SP_AM_Password;
    strcpy(req_user_login_field.Password, m_pass.c_str());
    //strcpy(req_user_login_field.UserProductInfo, "lev2apidemo");
    strcpy(req_user_login_field.UserProductInfo, m_productInfo.c_str());

    if(m_api && !m_status){
        m_status = new LoginStatus();
        m_status->nReqId = ++nReqId;
        m_status->bLoginSuccess = false;
        int ret = m_api->ReqUserLogin(&req_user_login_field, m_status->nReqId);
        printf("ReqUserLogin l1 ret[%d]\n", ret);
        if (ret != 0)
        {
            printf("ReqUserLogin l1 fail, ret[%d]\n", ret);
        }
        return;
    }

};    

void TiHxQuoteL1Client::OnRspUserLogin(
    TORALEV1API::CTORATstpRspUserLoginField *pRspUserLogin,
    TORALEV1API::CTORATstpRspInfoField *pRspInfo, 
    int nRequestID)
{
    printf("TiHxQuoteL1Client [OnRspUserLogin] ErrorID: %d ErrorMsg: %s\n", pRspInfo->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg).c_str());
    
    m_trading_day = datetime::get_today();
    if(m_cb){
        m_cb->OnTradingDayRtn(m_trading_day, "L1");
    }
    //LOG(INFO) << "[OnRspUserLogin] ErrorID: "<< pRspInfo->ErrorID << " ex_str: " << ex_str << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};

void TiHxQuoteL1Client::OnRspSubMarketData(
    TORALEV1API::CTORATstpSpecificSecurityField *pSpecificSecurity,
    TORALEV1API::CTORATstpRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast)
{
    std::cout << "[OnRspSubMarketData] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
    LOG(INFO) << "[OnRspSubMarketData] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};
