#include <iostream>
#include <iniFile.h>
#include <glog/logging.h>
#include <string.h>
#include "ti_hx_quote_client.h"
#include "ti_quote_struct.h"
#include "datetime.h"


using namespace std;

TiHxQuoteClient::TiHxQuoteClient(std::string configPath, TiQuoteCallback* userCb)
{
    m_config = NULL;
    m_sh_api    = NULL;
    m_sh_status = NULL;
    m_sz_api    = NULL;
    m_sz_status = NULL;
    m_trading_day = 0;

    m_cb = userCb;
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));
    memset(&m_snapIndexCash, 0, sizeof(TiQuoteSnapshotIndexField));
    memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));
    memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));

    loadConfig(configPath);
    nReqId = 0;
};

TiHxQuoteClient::~TiHxQuoteClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    if (m_sh_api)
    {
        m_sh_api->Release();
        m_sh_api = NULL;
    }
    if (m_sh_status)
    {
        delete m_sh_status;
        m_sh_status = NULL;
    }
    if (m_sz_api)
    {
        m_sz_api->Release();
        m_sh_api = NULL;
    }
    if (m_sz_status)
    {
        delete m_sz_status;
        m_sz_status = NULL;
    }
    m_cb = NULL;
    nReqId = 0;
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiHxQuoteClient::OnFrontConnected()
{
    if(!m_config){
        LOG(ERROR) << "[OnFrontConnected] Do not have config info";
        return ;
    }
    printf("TiHxQuoteClient::OnFrontConnected\n");
    CTORATstpReqUserLoginField req_user_login_field;
    memset(&req_user_login_field, 0, sizeof(req_user_login_field));
    strcpy(req_user_login_field.LogInAccount, m_config->szAccount.c_str());
    req_user_login_field.LogInAccountType = TORA_TSTP_LACT_UserID;
    // req_user_login_field.AuthMode = TORA_TSTP_SP_AM_Password;
    strcpy(req_user_login_field.Password, m_config->szPass.c_str());
    //strcpy(req_user_login_field.UserProductInfo, "lev2apidemo");
    strcpy(req_user_login_field.UserProductInfo, m_config->szProductInfo.c_str());

    if(m_sh_api && !m_sh_status){
        m_sh_status = new LoginStatus();
        m_sh_status->nReqId = ++nReqId;
        m_sh_status->bLoginSuccess = false;
        int ret = m_sh_api->ReqUserLogin(&req_user_login_field, m_sh_status->nReqId);
        if (ret != 0)
        {
            printf("ReqUserLogin SH fail, ret[%d]\n", ret);
        }
        return;
    }

    if(m_sz_api && !m_sz_status){
        m_sz_status = new LoginStatus();
        m_sz_status->nReqId = ++nReqId;
        m_sz_status->bLoginSuccess = false;
        int ret = m_sz_api->ReqUserLogin(&req_user_login_field, m_sz_status->nReqId);
        if (ret != 0)
        {
            printf("ReqUserLogin SZ fail, ret[%d]\n", ret);
        }
        return;
    }
};    

void TiHxQuoteClient::OnRspUserLogin(
    CTORATstpRspUserLoginField *pRspUserLogin,
    CTORATstpRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast)
{
    LoginStatus* _status = NULL;
    char ex_str[TI_EXCHANGE_STR_LEN] = {0};
    if(m_sh_status){
        if (m_sh_status->nReqId == nRequestID)
        {
            strcpy(ex_str, "SH");
            _status = m_sh_status;
        }
    }

    if(m_sz_status){
        if (m_sz_status->nReqId == nRequestID)
        {
            strcpy(ex_str, "SZ");
            _status = m_sz_status;
        }
    }

    if (pRspInfo->ErrorID == 0)
    {
        _status->bLoginSuccess = true;
    }

    m_trading_day = datetime::get_today();
    if(m_cb){
        m_cb->OnTradingDayRtn(m_trading_day, ex_str);
    }
    LOG(INFO) << "[OnRspUserLogin] ErrorID: "<< pRspInfo->ErrorID << " ex_str: " << ex_str << " ErrorMsg: " << pRspInfo->ErrorMsg;
};

void TiHxQuoteClient::OnRspSubMarketData(
    CTORATstpSpecificSecurityField *pSpecificSecurity,
    CTORATstpRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast)
{
    LOG(INFO) << "[OnRspSubMarketData] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << pRspInfo->ErrorMsg;
};

void TiHxQuoteClient::OnRtnMarketData(CTORATstpLev2MarketDataField *pDepthMarketData,
                                 const int FirstLevelBuyNum, const int FirstLevelBuyOrderVolumes[],
                                 const int FirstLevelSellNum, const int FirstLevelSellOrderVolumes[])
{
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));

    strcpy(m_snapStockCash.symbol, pDepthMarketData->SecurityID);
    m_snapStockCash.date            = m_trading_day;
    if(pDepthMarketData->ExchangeID == '1'){
        strcpy(m_snapStockCash.exchange, "SH");
        m_snapStockCash.time            = pDepthMarketData->DataTimeStamp * 1000;
    }
    if(pDepthMarketData->ExchangeID == '2'){
        strcpy(m_snapStockCash.exchange, "SZ");
        m_snapStockCash.time            = pDepthMarketData->DataTimeStamp;
    }
    m_snapStockCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_snapStockCash.time);
    datetime::get_format_time_ms(m_trading_day, m_snapStockCash.time, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();

    m_snapStockCash.last             = pDepthMarketData->LastPrice;
    m_snapStockCash.pre_close        = pDepthMarketData->PreClosePrice;
    m_snapStockCash.open             = pDepthMarketData->OpenPrice;
    m_snapStockCash.high             = pDepthMarketData->HighestPrice;
    m_snapStockCash.low              = pDepthMarketData->LowestPrice;
    m_snapStockCash.high_limit       = pDepthMarketData->UpperLimitPrice;
    m_snapStockCash.low_limit        = pDepthMarketData->LowerLimitPrice;
    m_snapStockCash.volume           = 0;
    m_snapStockCash.turnover         = 0;
    m_snapStockCash.acc_volume       = pDepthMarketData->TotalVolumeTrade;
    m_snapStockCash.acc_turnover     = pDepthMarketData->TotalValueTrade;
    m_snapStockCash.match_items      = pDepthMarketData->NumTrades;
    m_snapStockCash.total_ask_qty    = pDepthMarketData->TotalAskVolume;
    m_snapStockCash.total_bid_qty    = pDepthMarketData->TotalBidVolume;
    m_snapStockCash.wavg_ask         = pDepthMarketData->AvgAskPrice;
    m_snapStockCash.wavg_bid         = pDepthMarketData->AvgBidPrice;
    m_snapStockCash.interest         = 0;
    //ask
    m_snapStockCash.ask_price[0]     = pDepthMarketData->AskPrice1;
    m_snapStockCash.ask_price[1]     = pDepthMarketData->AskPrice2;
    m_snapStockCash.ask_price[2]     = pDepthMarketData->AskPrice3;
    m_snapStockCash.ask_price[3]     = pDepthMarketData->AskPrice4;
    m_snapStockCash.ask_price[4]     = pDepthMarketData->AskPrice5;
    m_snapStockCash.ask_price[5]     = pDepthMarketData->AskPrice6;
    m_snapStockCash.ask_price[6]     = pDepthMarketData->AskPrice7;
    m_snapStockCash.ask_price[7]     = pDepthMarketData->AskPrice8;
    m_snapStockCash.ask_price[8]     = pDepthMarketData->AskPrice9;
    m_snapStockCash.ask_price[9]     = pDepthMarketData->AskPrice10;

    m_snapStockCash.ask_volume[0]    = pDepthMarketData->AskVolume1;
    m_snapStockCash.ask_volume[1]    = pDepthMarketData->AskVolume2;
    m_snapStockCash.ask_volume[2]    = pDepthMarketData->AskVolume3;
    m_snapStockCash.ask_volume[3]    = pDepthMarketData->AskVolume4;
    m_snapStockCash.ask_volume[4]    = pDepthMarketData->AskVolume5;
    m_snapStockCash.ask_volume[5]    = pDepthMarketData->AskVolume6;
    m_snapStockCash.ask_volume[6]    = pDepthMarketData->AskVolume7;
    m_snapStockCash.ask_volume[7]    = pDepthMarketData->AskVolume8;
    m_snapStockCash.ask_volume[8]    = pDepthMarketData->AskVolume9;
    m_snapStockCash.ask_volume[9]    = pDepthMarketData->AskVolume10;

    m_snapStockCash.ask_order_num[0] = pDepthMarketData->Ask1NumOrders;
    m_snapStockCash.ask_order_num[1] = pDepthMarketData->Ask2NumOrders;
    m_snapStockCash.ask_order_num[2] = pDepthMarketData->Ask3NumOrders;
    m_snapStockCash.ask_order_num[3] = pDepthMarketData->Ask4NumOrders;
    m_snapStockCash.ask_order_num[4] = pDepthMarketData->Ask5NumOrders;
    m_snapStockCash.ask_order_num[5] = pDepthMarketData->Ask6NumOrders;
    m_snapStockCash.ask_order_num[6] = pDepthMarketData->Ask7NumOrders;
    m_snapStockCash.ask_order_num[7] = pDepthMarketData->Ask8NumOrders;
    m_snapStockCash.ask_order_num[8] = pDepthMarketData->Ask9NumOrders;
    m_snapStockCash.ask_order_num[9] = pDepthMarketData->Ask10NumOrders;

    //bid
    m_snapStockCash.bid_price[0]     = pDepthMarketData->BidPrice1;
    m_snapStockCash.bid_price[1]     = pDepthMarketData->BidPrice2;
    m_snapStockCash.bid_price[2]     = pDepthMarketData->BidPrice3;
    m_snapStockCash.bid_price[3]     = pDepthMarketData->BidPrice4;
    m_snapStockCash.bid_price[4]     = pDepthMarketData->BidPrice5;
    m_snapStockCash.bid_price[5]     = pDepthMarketData->BidPrice6;
    m_snapStockCash.bid_price[6]     = pDepthMarketData->BidPrice7;
    m_snapStockCash.bid_price[7]     = pDepthMarketData->BidPrice8;
    m_snapStockCash.bid_price[8]     = pDepthMarketData->BidPrice9;
    m_snapStockCash.bid_price[9]     = pDepthMarketData->BidPrice10;

    m_snapStockCash.bid_volume[0]    = pDepthMarketData->BidVolume1;
    m_snapStockCash.bid_volume[1]    = pDepthMarketData->BidVolume2;
    m_snapStockCash.bid_volume[2]    = pDepthMarketData->BidVolume3;
    m_snapStockCash.bid_volume[3]    = pDepthMarketData->BidVolume4;
    m_snapStockCash.bid_volume[4]    = pDepthMarketData->BidVolume5;
    m_snapStockCash.bid_volume[5]    = pDepthMarketData->BidVolume6;
    m_snapStockCash.bid_volume[6]    = pDepthMarketData->BidVolume7;
    m_snapStockCash.bid_volume[7]    = pDepthMarketData->BidVolume8;
    m_snapStockCash.bid_volume[8]    = pDepthMarketData->BidVolume9;
    m_snapStockCash.bid_volume[9]    = pDepthMarketData->BidVolume10;

    m_snapStockCash.bid_order_num[0] = pDepthMarketData->Bid1NumOrders;
    m_snapStockCash.bid_order_num[1] = pDepthMarketData->Bid2NumOrders;
    m_snapStockCash.bid_order_num[2] = pDepthMarketData->Bid3NumOrders;
    m_snapStockCash.bid_order_num[3] = pDepthMarketData->Bid4NumOrders;
    m_snapStockCash.bid_order_num[4] = pDepthMarketData->Bid5NumOrders;
    m_snapStockCash.bid_order_num[5] = pDepthMarketData->Bid6NumOrders;
    m_snapStockCash.bid_order_num[6] = pDepthMarketData->Bid7NumOrders;
    m_snapStockCash.bid_order_num[7] = pDepthMarketData->Bid8NumOrders;
    m_snapStockCash.bid_order_num[8] = pDepthMarketData->Bid9NumOrders;
    m_snapStockCash.bid_order_num[9] = pDepthMarketData->Bid10NumOrders;
    
    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }
};

void TiHxQuoteClient::OnRtnIndex(CTORATstpLev2IndexField *pIndex){
    memset(&m_snapIndexCash, 0, sizeof(TiQuoteSnapshotIndexField));

    strcpy(m_snapIndexCash.symbol, pIndex->SecurityID);
    m_snapIndexCash.date            = m_trading_day;
    if(pIndex->ExchangeID == '1'){
        strcpy(m_snapIndexCash.exchange, "SH");
        m_snapIndexCash.time            = pIndex->DataTimeStamp * 1000;
    }
    if(pIndex->ExchangeID == '2'){
        strcpy(m_snapIndexCash.exchange, "SZ");
        m_snapIndexCash.time            = pIndex->DataTimeStamp;
    }
    m_snapIndexCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_snapIndexCash.time);
    datetime::get_format_time_ms(m_trading_day, m_snapIndexCash.time, m_snapIndexCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapIndexCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();

    m_snapIndexCash.last            = pIndex->LastIndex;
    m_snapIndexCash.pre_close       = pIndex->PreCloseIndex;
    m_snapIndexCash.open            = pIndex->OpenIndex;
    m_snapIndexCash.high            = pIndex->HighIndex;
    m_snapIndexCash.low             = pIndex->LowIndex;
    m_snapIndexCash.volume          = pIndex->TotalVolumeTraded;
    m_snapIndexCash.turnover        = pIndex->Turnover;
    m_snapIndexCash.close           = pIndex->CloseIndex;

    if(m_cb){
        m_cb->OnL2IndexSnapshotRtn(&m_snapIndexCash);
    }
};

void TiHxQuoteClient::OnRtnOrderDetail(CTORATstpLev2OrderDetailField *pOrderDetail) 
{
    memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));
};

void TiHxQuoteClient::OnRtnTransaction(CTORATstpLev2TransactionField *pTransaction) 
{
    memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));

    strcpy(m_matchCash.symbol, pTransaction->SecurityID);
    m_matchCash.date            = m_trading_day;
    if(pTransaction->ExchangeID == '1'){
        strcpy(m_matchCash.exchange, "SH");
        m_matchCash.time            = pTransaction->TradeTime * 10;
    }
    if(pTransaction->ExchangeID == '2'){
        strcpy(m_matchCash.exchange, "SZ");
        m_matchCash.time            = pTransaction->TradeTime;
    }
    m_matchCash.timestamp           = datetime::get_timestamp_ms(m_trading_day, m_matchCash.time);
    datetime::get_format_time_ms(m_trading_day, m_matchCash.time, m_matchCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_matchCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();
    
    m_matchCash.channel         = 0;
    m_matchCash.seq             = pTransaction->MainSeq;
    m_matchCash.price           = pTransaction->TradePrice;
    m_matchCash.volume          = pTransaction->TradeVolume;
    m_matchCash.bs_flag         = pTransaction->TradeBSFlag;
    m_matchCash.function_code   = pTransaction->ExecType;
    m_matchCash.ask_order_seq   = pTransaction->SellNo;
    m_matchCash.bid_order_seq   = pTransaction->BuyNo;
    
    if(m_cb){
        m_cb->OnL2StockMatchesRtn(&m_matchCash);
    }
};

////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiHxQuoteClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szL2ShHost            = string(_iniFile["ti_hx_quote_client"]["l2_sh_host"]);
    m_config->szL2SzHost            = string(_iniFile["ti_hx_quote_client"]["l2_sz_host"]);

    m_config->szProductInfo         = string(_iniFile["ti_hx_quote_client"]["product"]);
    m_config->szAccount             = string(_iniFile["ti_hx_quote_client"]["account"]);
    m_config->szPass                = string(_iniFile["ti_hx_quote_client"]["pass"]);

    
    if( (m_config->szL2ShHost.empty() && 
        m_config->szL2SzHost.empty()) |
        m_config->szAccount.empty() |
        m_config->szPass.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void TiHxQuoteClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return ;
    }
    if(!m_config->szL2ShHost.empty() && !m_sh_api){
        m_sh_api = CTORATstpLev2MdApi::CreateTstpLev2MdApi();
        m_sh_api->RegisterSpi(this);
        m_sh_api->RegisterFront((char*)m_config->szL2ShHost.c_str());
        m_sh_api->Init();
    }
    if(!m_config->szL2SzHost.empty() && !m_sz_api){
        m_sz_api = CTORATstpLev2MdApi::CreateTstpLev2MdApi();
        m_sz_api->RegisterSpi(this);
        m_sz_api->RegisterFront((char*)m_config->szL2SzHost.c_str());
        m_sz_api->Init();
    }
};


void TiHxQuoteClient::subData(const char* exchangeName, char* codeList[], size_t len){
    int ret = 0;
    if(!strcmp(exchangeName, "SH")){
        ret = m_sh_api->SubscribeMarketData(codeList, len, TORA_TSTP_EXD_SSE);
        ret = m_sh_api->SubscribeIndex(codeList, len, TORA_TSTP_EXD_SSE);
        ret = m_sh_api->SubscribeTransaction(codeList, len, TORA_TSTP_EXD_SSE);
        if (ret != 0)
        {
            printf("SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
    }

    if(!strcmp(exchangeName, "SZ")){
        ret = m_sz_api->SubscribeMarketData(codeList, len, TORA_TSTP_EXD_SZSE);
        ret = m_sz_api->SubscribeIndex(codeList, len, TORA_TSTP_EXD_SZSE);
        ret = m_sz_api->SubscribeTransaction(codeList, len, TORA_TSTP_EXD_SZSE);
        if (ret != 0)
        {
            printf("SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
    }
    
};