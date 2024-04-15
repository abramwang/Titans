#include "ti_ctp_quote_client.h"
#include <cmath>
#include <limits>
#include <string.h>
#include <glog/logging.h>
#include <algorithm>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"
#include "ti_cf_contract_tool.h"
#include "ti_number_tool.h"

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
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    
    CThostFtdcReqUserLoginField req = {0};

    strcpy(req.BrokerID, m_config->szBrokerId.c_str());
    strcpy(req.UserID, m_config->szUser.c_str());
    strcpy(req.Password, m_config->szPass.c_str());

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

    std::vector<std::string> contracts_vec;
    //TiCFContractTool::getContractCodes("IF");
    TiCFContractTool::getContractCodes("IF", contracts_vec);
    TiCFContractTool::getContractCodes("IM", contracts_vec);
    TiCFContractTool::getContractCodes("IC", contracts_vec);
    TiCFContractTool::getContractCodes("IH", contracts_vec);
    //return;

    bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
    if (!bResult)
    {
        std::cout << "=====账户登录成功=====" << std::endl;
        std::cout << "交易日： " << pRspUserLogin->TradingDay << std::endl;
        std::cout << "登录时间： " << pRspUserLogin->LoginTime << std::endl;
        std::cout << "经纪商： " << pRspUserLogin->BrokerID << std::endl;
        std::cout << "帐户名： " << pRspUserLogin->UserID << std::endl;
        // 开始订阅行情

        int instrumentNum = contracts_vec.size();   
    
        char **pInstrumentID = new char*[instrumentNum];
        for (int i = 0; i < instrumentNum; i++)
        {
            pInstrumentID[i] = new char[10];
            strcpy(pInstrumentID[i], contracts_vec[i].c_str());
        }

        int rt = m_api->SubscribeMarketData(pInstrumentID, 1);
        if (!rt)
            std::cout << ">>>>>>发送订阅行情请求成功" << std::endl;
        else
            std::cerr << "--->>>发送订阅行情请求失败" << std::endl;

        for (int i = 0; i < instrumentNum; i++)
        {
            delete[] pInstrumentID[i];
        }
        delete[] pInstrumentID;
    }
    else
    {
        std::cerr << "返回错误--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << std::endl;
    }
}

void TiCtpQuoteClient::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    std::cout << "错误回报：" << TiEncodingTool::GbkToUtf8(pRspInfo->ErrorMsg).c_str() << std::endl;
    // 错误回报的处理
}

void TiCtpQuoteClient::OnRspQryMulticastInstrument(CThostFtdcMulticastInstrumentField *pMulticastInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

    std::cout << "OnRspQryMulticastInstrument:" << bIsLast << std::endl;
};

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
    
    std::cout << std::numeric_limits<double>::max() << " " << std::numeric_limits<double>::infinity() << std::endl;

    m_snapCash.last = pDepthMarketData->LastPrice;

    m_snapCash.pre_settlement_close = TiNumberTool::checkSpecialValue(pDepthMarketData->PreSettlementPrice);
    m_snapCash.pre_open_interest = TiNumberTool::checkSpecialValue(pDepthMarketData->PreOpenInterest);
    m_snapCash.pre_close = TiNumberTool::checkSpecialValue(pDepthMarketData->PreClosePrice);
    m_snapCash.open = TiNumberTool::checkSpecialValue(pDepthMarketData->OpenPrice);
    m_snapCash.high = TiNumberTool::checkSpecialValue(pDepthMarketData->HighestPrice);
    m_snapCash.low = TiNumberTool::checkSpecialValue(pDepthMarketData->LowestPrice);
    m_snapCash.close = TiNumberTool::checkSpecialValue(pDepthMarketData->ClosePrice);
    m_snapCash.settlement = TiNumberTool::checkSpecialValue(pDepthMarketData->SettlementPrice);
    m_snapCash.high_limit = TiNumberTool::checkSpecialValue(pDepthMarketData->UpperLimitPrice);
    m_snapCash.low_limit = TiNumberTool::checkSpecialValue(pDepthMarketData->LowerLimitPrice);
    m_snapCash.volume = TiNumberTool::checkSpecialValue(pDepthMarketData->Volume);
    m_snapCash.turnover = TiNumberTool::checkSpecialValue(pDepthMarketData->Turnover);
    m_snapCash.acc_volume = TiNumberTool::checkSpecialValue(pDepthMarketData->Volume);
    m_snapCash.acc_turnover = TiNumberTool::checkSpecialValue(pDepthMarketData->Turnover);
    m_snapCash.open_interest = TiNumberTool::checkSpecialValue(pDepthMarketData->OpenInterest);
    m_snapCash.pre_delta = TiNumberTool::checkSpecialValue(pDepthMarketData->PreDelta);
    m_snapCash.curr_delta = TiNumberTool::checkSpecialValue(pDepthMarketData->CurrDelta);
    m_snapCash.ave_price = TiNumberTool::checkSpecialValue(pDepthMarketData->AveragePrice);

   
    m_snapCash.ask_price[0] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskPrice1);
    m_snapCash.ask_price[1] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskPrice2);
    m_snapCash.ask_price[2] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskPrice3);
    m_snapCash.ask_price[3] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskPrice4);
    m_snapCash.ask_price[4] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskPrice5);

    m_snapCash.ask_volume[0] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskVolume1);
    m_snapCash.ask_volume[1] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskVolume2);
    m_snapCash.ask_volume[2] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskVolume3);
    m_snapCash.ask_volume[3] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskVolume4);
    m_snapCash.ask_volume[4] = TiNumberTool::checkSpecialValue(pDepthMarketData->AskVolume5);

    m_snapCash.bid_price[0] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidPrice1);
    m_snapCash.bid_price[1] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidPrice2);
    m_snapCash.bid_price[2] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidPrice3);
    m_snapCash.bid_price[3] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidPrice4);
    m_snapCash.bid_price[4] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidPrice5);

    m_snapCash.bid_volume[0] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidVolume1);
    m_snapCash.bid_volume[1] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidVolume2);
    m_snapCash.bid_volume[2] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidVolume3);
    m_snapCash.bid_volume[3] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidVolume4);
    m_snapCash.bid_volume[4] = TiNumberTool::checkSpecialValue(pDepthMarketData->BidVolume5);


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

    m_api->RegisterFront((char*)m_config->szLocation.c_str());
    std::string api_version = m_api->GetApiVersion();

    std::cout << "API版本：" << api_version << std::endl;
    
    m_api->Init();
    //m_api->Join();
    std::cout << "TiCtpQuoteClient::connect over" << std::endl;
};

void TiCtpQuoteClient::subData(const char* exchangeName, char* codeList[], size_t len){

};