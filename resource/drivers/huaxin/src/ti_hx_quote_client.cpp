#include <fstream>
#include <string>
#include <iomanip> // for std::setprecision
#include <iostream>
#include <iniFile.h>
#include <glog/logging.h>
#include "ti_encoding_tool.h"
#include "ti_hx_quote_client.h"
#include "ti_quote_struct.h"
#include "datetime.h"

using namespace std;

#define Enable_Hx_DataOutput 0

TiHxQuoteClient::TiHxQuoteClient(std::string configPath, TiQuoteCallback* userCb)
{
    m_config = NULL;

    m_l1_client = NULL;

    m_multicast_api = NULL;
    m_multicast_status = NULL;

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
    if (m_multicast_api)
    {
        m_multicast_api->Release();
        m_multicast_api = NULL;
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

    if(m_multicast_api && !m_multicast_status){
        m_multicast_status = new LoginStatus();
        m_multicast_status->nReqId = ++nReqId;
        m_multicast_status->bLoginSuccess = false;
        int ret = m_multicast_api->ReqUserLogin(&req_user_login_field, m_multicast_status->nReqId);
        if (ret != 0)
        {
            printf("ReqUserLogin multicast fail, ret[%d]\n", ret);
        }
        return;
    }

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

    if(m_multicast_status){
        if (m_multicast_status->nReqId == nRequestID)
        {
            strcpy(ex_str, "MULTICAST");
            _status = m_multicast_status;
        }
    }

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

    LOG(INFO) << "[OnRspUserLogin] ErrorID: "<< pRspInfo->ErrorID << " ex_str: " << ex_str << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};

void TiHxQuoteClient::OnRspSubMarketData(
    CTORATstpSpecificSecurityField *pSpecificSecurity,
    CTORATstpRspInfoField *pRspInfo, 
    int nRequestID, bool bIsLast)
{
    std::cout << "[OnRspSubMarketData] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
    LOG(INFO) << "[OnRspSubMarketData] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};

void TiHxQuoteClient::OnRspSubIndex(CTORATstpSpecificSecurityField *pSpecificSecurity, CTORATstpRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::cout << "[OnRspSubIndex] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
    LOG(INFO) << "[OnRspSubIndex] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};

void TiHxQuoteClient::OnRspSubTransaction(CTORATstpSpecificSecurityField *pSpecificSecurity, CTORATstpRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::cout << "[OnRspSubTransaction] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
    LOG(INFO) << "[OnRspSubTransaction] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
};

void TiHxQuoteClient::OnRspSubOrderDetail(CTORATstpSpecificSecurityField *pSpecificSecurity, CTORATstpRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::cout << "[OnRspSubOrderDetail] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
    LOG(INFO) << "[OnRspSubOrderDetail] ErrorID: "<< pRspInfo->ErrorID << " ErrorMsg: " << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg);
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
    //datetime::get_format_time_ms(m_trading_day, m_snapStockCash.time, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    //datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();

    TiQuoteSnapshotStockField* pL1Snap = m_l1_client->GetStockSnapshot(m_snapStockCash.symbol, m_snapStockCash.exchange);

    m_snapStockCash.last             = pDepthMarketData->LastPrice;
    m_snapStockCash.pre_close        = pDepthMarketData->PreClosePrice;
    m_snapStockCash.open             = pDepthMarketData->OpenPrice;
    m_snapStockCash.high             = pDepthMarketData->HighestPrice;
    m_snapStockCash.low              = pDepthMarketData->LowestPrice;
    m_snapStockCash.high_limit       = pDepthMarketData->UpperLimitPrice;
    if (!m_snapStockCash.high_limit && pL1Snap)
    {
        m_snapStockCash.high_limit = pL1Snap->high_limit;
    }
    m_snapStockCash.low_limit        = pDepthMarketData->LowerLimitPrice;
    if (!m_snapStockCash.low_limit && pL1Snap)
    {
        m_snapStockCash.low_limit = pL1Snap->low_limit;
    }
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
    //datetime::get_format_time_ms(m_trading_day, m_snapIndexCash.time, m_snapIndexCash.time_str, TI_TIME_STR_LEN);
    //datetime::get_format_now_time_us(m_snapIndexCash.recv_time_str, TI_TIME_STR_LEN);
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
#if Enable_Hx_DataOutput
    try {
        WriteToCSV(*pOrderDetail, "order_detail_data.csv");
    } catch (const std::exception& e) {
        // 捕获异常并打印错误
        std::cerr << "Error writing to CSV: " << e.what() << std::endl;
    }
#endif
    memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));

    strcpy(m_orderCash.symbol, pOrderDetail->SecurityID);
    m_orderCash.date            = m_trading_day;
    if(pOrderDetail->ExchangeID == '1'){
        strcpy(m_orderCash.exchange, "SH");
        m_orderCash.time            = pOrderDetail->OrderTime * 10;
    }
    if(pOrderDetail->ExchangeID == '2'){
        strcpy(m_orderCash.exchange, "SZ");
        m_orderCash.time            = pOrderDetail->OrderTime;
    }
    m_orderCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_orderCash.time);
    //datetime::get_format_time_ms(m_trading_day, m_orderCash.time, m_orderCash.time_str, TI_TIME_STR_LEN);
    //datetime::get_format_now_time_us(m_orderCash.recv_time_str, TI_TIME_STR_LEN);
    m_orderCash.recv_timestamp  = datetime::get_now_timestamp_ms();

    m_orderCash.channel         = pOrderDetail->MainSeq;
    m_orderCash.seq             = pOrderDetail->SubSeq;
    m_orderCash.price           = pOrderDetail->Price;
    m_orderCash.volume          = pOrderDetail->Volume;

    if(pOrderDetail->Side == '1'){
        m_orderCash.function_code = 'B'; 
    }
    if(pOrderDetail->Side == '2'){
        m_orderCash.function_code = 'S'; 
    }

    if (pOrderDetail->OrderType == '2') //限价单
    {
        m_orderCash.order_type      = '0';
    }else{
        m_orderCash.order_type      = pOrderDetail->OrderType;
    }
    
    m_orderCash.order_orino     = pOrderDetail->OrderNO;

    if(m_cb){
        m_cb->OnL2StockOrderRtn(&m_orderCash);
    }
};

void TiHxQuoteClient::OnRtnTransaction(CTORATstpLev2TransactionField *pTransaction) 
{
#if Enable_Hx_DataOutput
    try {
        WriteToCSV(*pTransaction, "transaction_data.csv");
    } catch (const std::exception& e) {
        // 捕获异常并打印错误
        std::cerr << "Error writing to CSV: " << e.what() << std::endl;
    }
#endif
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
    //datetime::get_format_time_ms(m_trading_day, m_matchCash.time, m_matchCash.time_str, TI_TIME_STR_LEN);
    //datetime::get_format_now_time_us(m_matchCash.recv_time_str, TI_TIME_STR_LEN);
    m_matchCash.recv_timestamp  = datetime::get_now_timestamp_ms();
    
    m_matchCash.channel         = pTransaction->MainSeq;
    m_matchCash.seq             = pTransaction->SubSeq;
    m_matchCash.price           = pTransaction->TradePrice;
    m_matchCash.volume          = pTransaction->TradeVolume;
    m_matchCash.bs_flag         = pTransaction->TradeBSFlag;

    if (pTransaction->ExecType == '2')
    {
        m_matchCash.function_code = 'C';
    }else{
        m_matchCash.function_code = '0';
    }

    m_matchCash.ask_order_seq   = pTransaction->SellNo;
    m_matchCash.bid_order_seq   = pTransaction->BuyNo;

    if (m_matchCash.ask_order_seq > m_matchCash.bid_order_seq)
    {
        m_matchCash.bs_flag = 'S';
    }else{
        m_matchCash.bs_flag = 'B';
    }
    
    if(m_cb){
        m_cb->OnL2StockMatchesRtn(&m_matchCash);
    }
};

void TiHxQuoteClient::OnRtnNGTSTick(CTORATstpLev2NGTSTickField *pTick)
{
#if Enable_Hx_DataOutput
    try {
        WriteToCSV(*pTick, "ngts_tick_data.csv");
    } catch (const std::exception& e) {
        // 捕获异常并打印错误
        std::cerr << "Error writing to CSV: " << e.what() << std::endl;
    }
#endif
    if (pTick->TickType == TORA_TSTP_LTT_Trade)
    {
        memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));
    
        strcpy(m_matchCash.symbol, pTick->SecurityID);
        m_matchCash.date            = m_trading_day;
        if(pTick->ExchangeID == '1'){
            strcpy(m_matchCash.exchange, "SH");
            m_matchCash.time            = pTick->TickTime * 10;
        }

        m_matchCash.timestamp         = datetime::get_timestamp_ms(m_trading_day, m_matchCash.time);
        //datetime::get_format_time_ms(m_trading_day, m_matchCash.time, m_matchCash.time_str, TI_TIME_STR_LEN);
        //datetime::get_format_now_time_us(m_matchCash.recv_time_str, TI_TIME_STR_LEN);
        m_matchCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        m_matchCash.channel         = pTick->MainSeq;
        m_matchCash.seq             = pTick->SubSeq;
        m_matchCash.price           = pTick->Price;
        m_matchCash.volume          = pTick->Volume;
        m_matchCash.ask_order_seq   = pTick->SellNo;
        m_matchCash.bid_order_seq   = pTick->BuyNo;

        m_matchCash.bs_flag         = pTick->TradeBSFlag;
        m_matchCash.function_code   = '0';

        if(m_cb){
            m_cb->OnL2StockMatchesRtn(&m_matchCash);
        }
    }

    if (pTick->TickType == TORA_TSTP_LTT_Add || pTick->TickType == TORA_TSTP_LTT_Delete)
    {
        memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));

        strcpy(m_orderCash.symbol, pTick->SecurityID);
        m_orderCash.date            = m_trading_day;
        if(pTick->ExchangeID == '1'){
            strcpy(m_orderCash.exchange, "SH");
            m_orderCash.time            = pTick->TickTime * 10;
        }

        m_orderCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_orderCash.time);
        //datetime::get_format_time_ms(m_trading_day, m_orderCash.time, m_orderCash.time_str, TI_TIME_STR_LEN);
        //datetime::get_format_now_time_us(m_orderCash.recv_time_str, TI_TIME_STR_LEN);
        m_orderCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        m_orderCash.channel         = pTick->MainSeq;
        m_orderCash.seq             = pTick->SubSeq;
        m_orderCash.price           = pTick->Price;
        m_orderCash.volume          = pTick->Volume;
        m_orderCash.order_orino     = pTick->MainSeq;
        if(pTick->Side == '1'){
            m_orderCash.function_code = 'B'; 
        }
        if(pTick->Side == '2'){
            m_orderCash.function_code = 'S'; 
        }
        m_orderCash.order_type      = pTick->TickType;

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }
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
    m_config->szL1Host              = string(_iniFile["ti_hx_quote_client"]["l1_host"]);

    m_config->szL2ShHost            = string(_iniFile["ti_hx_quote_client"]["l2_sh_host"]);
    m_config->szL2SzHost            = string(_iniFile["ti_hx_quote_client"]["l2_sz_host"]);

    m_config->bIsMulticast          = bool(_iniFile["ti_hx_quote_client"]["is_multicast"]);
    m_config->szL2Multicast         = string(_iniFile["ti_hx_quote_client"]["l2_multicast"]);
    m_config->szL2MulticastInterface= string(_iniFile["ti_hx_quote_client"]["l2_multicast_interface"]);

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

void TiHxQuoteClient::WriteToCSV(const CTORATstpLev2TransactionField& data, const std::string& fileName)
{
    // 打开文件（如果文件不存在，会创建新文件）
    std::ofstream file;
    file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    // 如果是新文件，写入标题行
    static bool isHeaderWritten = false;
    if (!isHeaderWritten) {
        file << "ExchangeID,SecurityID,TradeTime,TradePrice,TradeVolume,ExecType,MainSeq,SubSeq,BuyNo,SellNo,Info1,Info2,Info3,TradeBSFlag,BizIndex" << std::endl;
        isHeaderWritten = true;
    }

    // 写入数据行
    file << data.ExchangeID << ","
         << data.SecurityID << ","
         << data.TradeTime << ","
         << std::fixed << std::setprecision(2) << data.TradePrice << ","
         << data.TradeVolume << ","
         << static_cast<char>(data.ExecType) << ","
         << data.MainSeq << ","
         << data.SubSeq << ","
         << data.BuyNo << ","
         << data.SellNo << ","
         << data.Info1 << ","
         << data.Info2 << ","
         << data.Info3 << ","
         << static_cast<char>(data.TradeBSFlag) << ","
         << data.BizIndex << std::endl;

    file.close();
};

void TiHxQuoteClient::WriteToCSV(const CTORATstpLev2OrderDetailField& data, const std::string& fileName) {
    // 打开文件（如果文件不存在，会创建新文件）
    std::ofstream file;
    file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    // 如果是新文件，写入标题行
    static bool isHeaderWritten = false;
    if (!isHeaderWritten) {
        file << "ExchangeID,SecurityID,OrderTime,Price,Volume,Side,OrderType,MainSeq,SubSeq,Info1,Info2,Info3,OrderNO,OrderStatus,BizIndex" << std::endl;
        isHeaderWritten = true;
    }

    // 写入数据行
    file << data.ExchangeID << ","
         << data.SecurityID << ","
         << data.OrderTime << ","
         << std::fixed << std::setprecision(2) << data.Price << ","
         << data.Volume << ","
         << static_cast<char>(data.Side) << ","
         << static_cast<char>(data.OrderType) << ","
         << data.MainSeq << ","
         << data.SubSeq << ","
         << data.Info1 << ","
         << data.Info2 << ","
         << data.Info3 << ","
         << data.OrderNO << ","
         << data.OrderStatus << ","
         << data.BizIndex << std::endl;

    file.close();
}

void TiHxQuoteClient::WriteToCSV(const CTORATstpLev2NGTSTickField& data, const std::string& fileName) {
    // 打开文件（如果文件不存在，会创建新文件）
    std::ofstream file;
    file.open(fileName, std::ios::out | std::ios::app); // 追加模式打开

    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }

    // 如果是新文件，写入标题行
    static bool isHeaderWritten = false;
    if (!isHeaderWritten) {
        file << "ExchangeID,SecurityID,MainSeq,SubSeq,TickTime,TickType,BuyNo,SellNo,Price,Volume,TradeMoney,Side,TradeBSFlag,MDSecurityStat,Info1,Info2,Info3" << std::endl;
        isHeaderWritten = true;
    }

    // 写入数据行
    file << data.ExchangeID << ","
         << data.SecurityID << ","
         << data.MainSeq << ","
         << data.SubSeq << ","
         << data.TickTime << ","
         << static_cast<char>(data.TickType) << ","
         << data.BuyNo << ","
         << data.SellNo << ","
         << std::fixed << std::setprecision(2) << data.Price << ","
         << data.Volume << ","
         << std::fixed << std::setprecision(2) << data.TradeMoney << ","
         << static_cast<char>(data.Side) << ","
         << static_cast<char>(data.TradeBSFlag) << ","
         << data.MDSecurityStat << ","
         << data.Info1 << ","
         << data.Info2 << ","
         << data.Info3 << std::endl;

    file.close();
}



////////////////////////////////////////////////////////////////////////
// 公有方法
////////////////////////////////////////////////////////////////////////
void TiHxQuoteClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return ;
    }

    if(!m_config->szL1Host.empty() && !m_l1_client){
        m_l1_client = new TiHxQuoteL1Client(m_cb,
            m_config->szL1Host.c_str(),
            m_config->szProductInfo.c_str(), m_config->szAccount.c_str(), m_config->szPass.c_str());
    }

    if(m_config->bIsMulticast && !m_multicast_api){
        m_multicast_api = CTORATstpLev2MdApi::CreateTstpLev2MdApi(TORA_TSTP_MST_MCAST);
        m_multicast_api->RegisterSpi(this);    
        m_multicast_api->RegisterMulticast((char*)m_config->szL2Multicast.c_str(), 
            (char*)m_config->szL2MulticastInterface.c_str(),
            NULL);
        TTORATstpMKSubTypesType subType;
        memset(&subType, 0, sizeof(subType));
        strcpy(subType, "MITO");
        m_multicast_api->DeclareMKSubTypes(subType);
        m_multicast_api->Init();
        return;
    }

    if(!m_config->szL2ShHost.empty() && !m_sh_api){
        m_sh_api = CTORATstpLev2MdApi::CreateTstpLev2MdApi();
        m_sh_api->RegisterSpi(this); 
        m_sh_api->RegisterFront((char*)m_config->szL2SzHost.c_str());
        m_sh_api->Init();
    }

    if(!m_config->szL2SzHost.empty() && !m_sz_api)
    {
        m_sz_api = CTORATstpLev2MdApi::CreateTstpLev2MdApi();
        m_sz_api->RegisterSpi(this); 
        m_sz_api->RegisterFront((char*)m_config->szL2SzHost.c_str());
        m_sz_api->Init();
    }
};


void TiHxQuoteClient::subData(const char* exchangeName, char* codeList[], size_t len){
    int ret = 0;

    if(m_l1_client){
        m_l1_client->subData(exchangeName, codeList, len);
    }

    if(m_multicast_api)
    {
        if(!strcmp(exchangeName, "SH")){
            ret = m_multicast_api->SubscribeMarketData(codeList, len, TORA_TSTP_EXD_SSE);
            if (ret != 0)
            {
                printf("[L2 multicast SH]SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            ret = m_multicast_api->SubscribeIndex(codeList, len, TORA_TSTP_EXD_SSE);
            if (ret != 0)
            {
                printf("[L2 multicast SH]SubscribeIndex fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            ret = m_multicast_api->SubscribeTransaction(codeList, len, TORA_TSTP_EXD_SSE);
            if (ret != 0)
            {
                printf("[L2 multicast SH]SubscribeTransaction fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            ret = m_multicast_api->SubscribeOrderDetail(codeList, len, TORA_TSTP_EXD_SSE);
            if (ret != 0)
            {
                printf("[L2 multicast SH]SubscribeOrderDetail fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            ret = m_multicast_api->SubscribeNGTSTick(codeList, len, TORA_TSTP_EXD_SSE);
            if (ret != 0)
            {
                printf("[L2 multicast SH]SubscribeNGTSTick fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            return;
        }
        if(!strcmp(exchangeName, "SZ")){
            ret = m_multicast_api->SubscribeMarketData(codeList, len, TORA_TSTP_EXD_SZSE);
            if (ret != 0)
            {
                printf("[L2 multicast SZ]SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            ret = m_multicast_api->SubscribeIndex(codeList, len, TORA_TSTP_EXD_SZSE);
            if (ret != 0)
            {
                printf("[L2 multicast SZ]SubscribeIndex fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            ret = m_multicast_api->SubscribeTransaction(codeList, len, TORA_TSTP_EXD_SZSE);
            if (ret != 0)
            {
                printf("[L2 multicast SZ]SubscribeTransaction fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            ret = m_multicast_api->SubscribeOrderDetail(codeList, len, TORA_TSTP_EXD_SZSE);
            if (ret != 0)
            {
                printf("[L2 multicast SZ]SubscribeOrderDetail fail, exchange[%s], ret[%d]\n", exchangeName, ret);
            }
            return;
        }
        return;
    }

    if(!strcmp(exchangeName, "SH")){
        ret = m_sh_api->SubscribeMarketData(codeList, len, TORA_TSTP_EXD_SSE);
        if (ret != 0)
        {
            printf("[L2 SH]SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
        ret = m_sh_api->SubscribeIndex(codeList, len, TORA_TSTP_EXD_SSE);
        if (ret != 0)
        {
            printf("[L2 SH]SubscribeIndex fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
        ret = m_sh_api->SubscribeTransaction(codeList, len, TORA_TSTP_EXD_SSE);
        if (ret != 0)
        {
            printf("[L2 SH]SubscribeTransaction fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
        ret = m_sh_api->SubscribeOrderDetail(codeList, len, TORA_TSTP_EXD_SSE);
        if (ret != 0)
        {
            printf("[L2 SH]SubscribeOrderDetail fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
    }

    if(!strcmp(exchangeName, "SZ")){
        ret = m_sz_api->SubscribeMarketData(codeList, len, TORA_TSTP_EXD_SZSE);
        if (ret != 0)
        {
            printf("[L2 SZ]SubscribeMarketData fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
        ret = m_sz_api->SubscribeIndex(codeList, len, TORA_TSTP_EXD_SZSE);
        if (ret != 0)
        {
            printf("[L2 SZ]SubscribeIndex fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
        ret = m_sz_api->SubscribeTransaction(codeList, len, TORA_TSTP_EXD_SZSE);
        if (ret != 0)
        {
            printf("[L2 SZ]SubscribeTransaction fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
        ret = m_sz_api->SubscribeOrderDetail(codeList, len, TORA_TSTP_EXD_SZSE);
        if (ret != 0)
        {
            printf("[L2 SZ]SubscribeOrderDetail fail, exchange[%s], ret[%d]\n", exchangeName, ret);
        }
    }
    
};