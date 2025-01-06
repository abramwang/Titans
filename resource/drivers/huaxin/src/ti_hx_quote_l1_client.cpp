#include <iostream>
#include <iniFile.h>
#include <glog/logging.h>
#include <string.h>
#include "ti_encoding_tool.h"
#include "ti_quote_formater.h"
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
    LOG(INFO) << "[OnRspUserLogin] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};

void TiHxQuoteL1Client::OnRspSubMarketData(
    TORALEV1API::CTORATstpSpecificSecurityField *pSpecificSecurity,
    TORALEV1API::CTORATstpRspInfoField *pRspInfo)
{
    std::cout << "[OnRspSubMarketData] SecurityID: "<< pSpecificSecurity->SecurityID << " ExchangeID: " << pSpecificSecurity->ExchangeID;
    std::cout << " ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg) << std::endl;
    LOG(INFO) << "[OnRspSubMarketData] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};

void TiHxQuoteL1Client::OnRtnMarketData(TORALEV1API::CTORATstpMarketDataField *pMarketDataField)
{
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));

    strcpy(m_snapStockCash.symbol, pMarketDataField->SecurityID);
    m_snapStockCash.date            = m_trading_day;
    m_snapStockCash.time            = atoi(pMarketDataField->UpdateTime);
    
    if(pMarketDataField->ExchangeID == '1'){
        strcpy(m_snapStockCash.exchange, "SH");
    }
    if(pMarketDataField->ExchangeID == '2'){
        strcpy(m_snapStockCash.exchange, "SZ");
    }
    m_snapStockCash.timestamp = datetime::get_timestamp_ms(pMarketDataField->TradingDay, pMarketDataField->UpdateTime);
    datetime::get_format_timestamp_ms(m_snapStockCash.timestamp, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();

    m_snapStockCash.last             = pMarketDataField->LastPrice;
    m_snapStockCash.pre_close        = pMarketDataField->PreClosePrice;
    m_snapStockCash.open             = pMarketDataField->OpenPrice;
    m_snapStockCash.high             = pMarketDataField->HighestPrice;
    m_snapStockCash.low              = pMarketDataField->LowestPrice;
    m_snapStockCash.high_limit       = pMarketDataField->UpperLimitPrice;
    m_snapStockCash.low_limit        = pMarketDataField->LowerLimitPrice;
    m_snapStockCash.acc_volume       = pMarketDataField->Volume;
    m_snapStockCash.acc_turnover     = pMarketDataField->Turnover;
    m_snapStockCash.iopv             = pMarketDataField->IOPV;
    //ask
    m_snapStockCash.ask_price[0]     = pMarketDataField->AskPrice1;
    m_snapStockCash.ask_price[1]     = pMarketDataField->AskPrice2;
    m_snapStockCash.ask_price[2]     = pMarketDataField->AskPrice3;
    m_snapStockCash.ask_price[3]     = pMarketDataField->AskPrice4;
    m_snapStockCash.ask_price[4]     = pMarketDataField->AskPrice5;

    m_snapStockCash.ask_volume[0]    = pMarketDataField->AskVolume1;
    m_snapStockCash.ask_volume[1]    = pMarketDataField->AskVolume2;
    m_snapStockCash.ask_volume[2]    = pMarketDataField->AskVolume3;
    m_snapStockCash.ask_volume[3]    = pMarketDataField->AskVolume4;
    m_snapStockCash.ask_volume[4]    = pMarketDataField->AskVolume5;

    //bid
    m_snapStockCash.bid_price[0]     = pMarketDataField->BidPrice1;
    m_snapStockCash.bid_price[1]     = pMarketDataField->BidPrice2;
    m_snapStockCash.bid_price[2]     = pMarketDataField->BidPrice3;
    m_snapStockCash.bid_price[3]     = pMarketDataField->BidPrice4;
    m_snapStockCash.bid_price[4]     = pMarketDataField->BidPrice5;

    m_snapStockCash.bid_volume[0]    = pMarketDataField->BidVolume1;
    m_snapStockCash.bid_volume[1]    = pMarketDataField->BidVolume2;
    m_snapStockCash.bid_volume[2]    = pMarketDataField->BidVolume3;
    m_snapStockCash.bid_volume[3]    = pMarketDataField->BidVolume4;
    m_snapStockCash.bid_volume[4]    = pMarketDataField->BidVolume5;

    /*
    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapStockCash, j);

    std::cout << "[OnRtnMarketData JSON] " << j.dump() << std::endl;
    */

    TiQuoteSnapshotStockField* pSnap = GetStockSnapshot(m_snapStockCash.symbol, m_snapStockCash.exchange);
    if(pSnap){
        memcpy(pSnap, &m_snapStockCash, sizeof(TiQuoteSnapshotStockField));
    }else{
        m_snapshot_map[TiQuoteTools::GetSymbolID(m_snapStockCash.exchange, m_snapStockCash.symbol)] = std::make_unique<TiQuoteSnapshotStockField>(m_snapStockCash);
    }

    /*
    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }
    */
};

void TiHxQuoteL1Client::subData(const char* exchangeName, char* codeList[], size_t len)
{
    int ret = 0;
    if(!strcmp(exchangeName, "SH")){
        ret = m_api->SubscribeMarketData(codeList, len, TORALEV1API::TORA_TSTP_EXD_SSE);
        //ret = m_api->SubscribeSimplifyMarketData(codeList, len, TORALEV1API::TORA_TSTP_EXD_SSE);
        printf("[L1]SubscribeMarketData exchange[%s], ret[%d]\n", exchangeName, ret);
        if (ret != 0)
        {
            printf("[L1]SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
    }

    if(!strcmp(exchangeName, "SZ")){
        ret = m_api->SubscribeMarketData(codeList, len, TORALEV1API::TORA_TSTP_EXD_SZSE);
        //ret = m_api->SubscribeSimplifyMarketData(codeList, len, TORALEV1API::TORA_TSTP_EXD_SSE);
        printf("[L1]SubscribeMarketData exchange[%s], ret[%d]\n", exchangeName, ret);
        if (ret != 0)
        {
            printf("[L1]SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
    }
};

TiQuoteSnapshotStockField* TiHxQuoteL1Client::GetStockSnapshot(const char* symbol, const char* exchange)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    auto it = m_snapshot_map.find(id);
    if (it != m_snapshot_map.end())
    {
        return it->second.get();
    }
    return nullptr;
};