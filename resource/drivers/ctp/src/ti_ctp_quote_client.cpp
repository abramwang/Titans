#include "ti_ctp_quote_client.h"
#include <string.h>
#include <glog/logging.h>
#include <algorithm>
#include "iniFile.h"
#include "datetime.h"

TiCtpQuoteClient::TiCtpQuoteClient(std::string configPath, TiQuoteCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_api = CThostFtdcMdApi::CreateFtdcMdApi();
	m_api->RegisterSpi(this);

    m_cb = userCb;
    memset(&m_snapCash, 0, sizeof(m_snapCash));

    loadConfig(configPath);
    nReqId = 0;
};


TiCtpQuoteClient::~TiCtpQuoteClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    m_cb = NULL;
    nReqId = 0;
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////
void TiCtpQuoteClient::OnFrontConnected()
{
    std::cout << "行情前置已连接" << std::endl;
    // 连接成功后可以进行行情操作
    
    CThostFtdcReqUserLoginField req = {0};

    strcpy(req.BrokerID, "9999");
    strcpy(req.UserID, "207960");
    strcpy(req.Password, "ikdlKJFHE%#^8");

    m_api->ReqUserLogin(&req, nReqId++);
}

void TiCtpQuoteClient::OnFrontDisconnected(int nReason)
{
    std::cout << "行情前置连接断开，原因：" << nReason << std::endl;
    // 连接断开后的处理
}

void TiCtpQuoteClient::OnRspUserLogin(
    CThostFtdcRspUserLoginField *pRspUserLogin, 
    CThostFtdcRspInfoField *pRspInfo, 
    int nRequestID, 
    bool bIsLast)
{
    char *g_pInstrumentID[] = {"IM2310", "IH2310", "IF2310", "IC2310", 
        "RB2310",  "JM2310" , "CF401"}; // 行情合约代码列表，中、上、大、郑交易所各选一种
    int instrumentNum = 7;   

    bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
    if (!bResult)
    {
        std::cout << "=====账户登录成功=====" << std::endl;
        std::cout << "交易日： " << pRspUserLogin->TradingDay << std::endl;
        std::cout << "登录时间： " << pRspUserLogin->LoginTime << std::endl;
        std::cout << "经纪商： " << pRspUserLogin->BrokerID << std::endl;
        std::cout << "帐户名： " << pRspUserLogin->UserID << std::endl;
        // 开始订阅行情
        int rt = m_api->SubscribeMarketData(g_pInstrumentID, instrumentNum);
        if (!rt)
            std::cout << ">>>>>>发送订阅行情请求成功" << std::endl;
        else
            std::cerr << "--->>>发送订阅行情请求失败" << std::endl;
    }
    else
        std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
}

void TiCtpQuoteClient::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::cout << "错误回报：" << pRspInfo->ErrorMsg << std::endl;
    // 错误回报的处理
}

///订阅行情应答
void TiCtpQuoteClient::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{

};

///取消订阅行情应答
void TiCtpQuoteClient::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{

};

///订阅询价应答
void TiCtpQuoteClient::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{

};

///取消订阅询价应答
void TiCtpQuoteClient::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) 
{

};

///深度行情通知
void TiCtpQuoteClient::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData) 
{
    memset(&m_snapCash, 0, sizeof(m_snapCash));
    
    strcpy(m_snapCash.exchange, "CF");
    strcpy(m_snapCash.symbol, pDepthMarketData->InstrumentID);
    m_snapCash.date = atoi(pDepthMarketData->TradingDay);
    std::string time_str = pDepthMarketData->UpdateTime;
    char charToRemove = ':'; // 要去除的特定字符
    time_str.erase(std::remove_if(time_str.begin(), time_str.end(), 
            [charToRemove](char c) { return c == charToRemove; }), time_str.end());
    m_snapCash.time = atoi(time_str.c_str())* 1000 + pDepthMarketData->UpdateMillisec;
    m_snapCash.timestamp = datetime::get_timestamp_ms(m_snapCash.date, m_snapCash.time);
    
    m_snapCash.last = pDepthMarketData->LastPrice;
    m_snapCash.pre_settlement_close = pDepthMarketData->PreSettlementPrice;
    m_snapCash.pre_open_interest = pDepthMarketData->PreOpenInterest;
    m_snapCash.pre_close = pDepthMarketData->PreClosePrice;
    m_snapCash.open = pDepthMarketData->OpenPrice;
    m_snapCash.high = pDepthMarketData->HighestPrice;
    m_snapCash.low = pDepthMarketData->LowestPrice;
    m_snapCash.close = pDepthMarketData->ClosePrice;
    m_snapCash.settlement = pDepthMarketData->SettlementPrice;
    m_snapCash.high_limit = pDepthMarketData->UpperLimitPrice;
    m_snapCash.low_limit = pDepthMarketData->LowerLimitPrice;
    //m_snapCash.volume = pDepthMarketData->Volume;
    //m_snapCash.turnover = pDepthMarketData->Turnover;
    m_snapCash.acc_volume = pDepthMarketData->Volume;
    m_snapCash.acc_turnover = pDepthMarketData->Turnover;
    m_snapCash.open_interest = pDepthMarketData->OpenInterest;
    m_snapCash.pre_delta = pDepthMarketData->PreDelta;
    m_snapCash.curr_delta = pDepthMarketData->CurrDelta;
    m_snapCash.ave_price = pDepthMarketData->AveragePrice;

    m_snapCash.ask_price[0] = pDepthMarketData->AskPrice1;
    //m_snapCash.ask_price[1] = pDepthMarketData->AskPrice2;
    //m_snapCash.ask_price[2] = pDepthMarketData->AskPrice3;
    //m_snapCash.ask_price[3] = pDepthMarketData->AskPrice4;
    //m_snapCash.ask_price[4] = pDepthMarketData->AskPrice5;
    m_snapCash.ask_volume[0] = pDepthMarketData->AskVolume1;
    //m_snapCash.ask_volume[1] = pDepthMarketData->AskVolume2;
    //m_snapCash.ask_volume[2] = pDepthMarketData->AskVolume3;
    //m_snapCash.ask_volume[3] = pDepthMarketData->AskVolume4;
    //m_snapCash.ask_volume[4] = pDepthMarketData->AskVolume5;
    m_snapCash.bid_price[0] = pDepthMarketData->BidPrice1;
    //m_snapCash.bid_price[1] = pDepthMarketData->BidPrice2;
    //m_snapCash.bid_price[2] = pDepthMarketData->BidPrice3;
    //m_snapCash.bid_price[3] = pDepthMarketData->BidPrice4;
    //m_snapCash.bid_price[4] = pDepthMarketData->BidPrice5;
    m_snapCash.bid_volume[0] = pDepthMarketData->BidVolume1;
    //m_snapCash.bid_volume[1] = pDepthMarketData->BidVolume2;
    //m_snapCash.bid_volume[2] = pDepthMarketData->BidVolume3;
    //m_snapCash.bid_volume[3] = pDepthMarketData->BidVolume4;
    //m_snapCash.bid_volume[4] = pDepthMarketData->BidVolume5;

    if (m_cb)
    {
        m_cb->OnL2FutureSnapshotRtn(&m_snapCash);
    }
};

///询价通知
void TiCtpQuoteClient::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{

};


////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiCtpQuoteClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocation            = string(_iniFile["ti_ctp_quote_client"]["location"]);
    m_config->szBrokerId            = string(_iniFile["ti_ctp_quote_client"]["broker_id"]);
    m_config->szUser                = string(_iniFile["ti_ctp_quote_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_ctp_quote_client"]["pass"]);

    
    if( m_config->szLocation.empty() |
        m_config->szBrokerId.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void TiCtpQuoteClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }

    m_api->RegisterFront("tcp://180.168.146.187:10211");
    std::string api_version = m_api->GetApiVersion();

    std::cout << "API版本：" << api_version << std::endl;
    
    m_api->Init();
};

void TiCtpQuoteClient::subData(const char* exchangeName, char* codeList[], size_t len){

};