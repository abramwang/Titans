#include "ees_trader_client.h"
#include "log.h"
#include "iniFile.h"
#include "ees_trader_response_msg.h"
#include <iostream>

using namespace EES_TRADER_RESPONSE_MSG;

EESTraderClient::EESTraderClient(std::string configPath)
{
    api = NULL;
    config = NULL;
    responser = NULL;
    api = CreateEESTraderApi();
    loadConfig(configPath);
    responser = new TradeResponser(configPath);
};

EESTraderClient::~EESTraderClient(){
    if(api){
        DestroyEESTraderApi(api);
        api = NULL;
    }
    if(config){
        delete config;
        config = NULL;
    }
    if(responser){
        delete responser;
        responser = NULL;
    }
};


////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void EESTraderClient::OnConnection(ERR_NO errNo, const char* pErrStr ){
    LOG_DEBUG("[%d] %s", errNo, pErrStr);
    logon();
};

void EESTraderClient::OnDisConnection(ERR_NO errNo, const char* pErrStr ){
    LOG_DEBUG("[%d] %s", errNo, pErrStr);
};

void EESTraderClient::OnUserLogon(EES_LogonResponse* pLogon){
    LOG_DEBUG("[%d] m_UserId:%d", pLogon->m_Result, pLogon->m_UserId);
};

void EESTraderClient::OnRspChangePassword(EES_ChangePasswordResult nResult){

};

void EESTraderClient::OnQueryUserAccount(EES_AccountInfo * pAccoutnInfo, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccoutnInfo);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryUserAccount", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pAccoutnPosition);
    cJSON_AddNumberToObject(j, "nReqId", nReqId);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryAccountPosition", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryAccountOptionPosition(const char* pAccount, EES_AccountOptionPosition* pAccoutnOptionPosition, int nReqId, bool bFinish) {
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pAccoutnOptionPosition);
    cJSON_AddNumberToObject(j, "nReqId", nReqId);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryAccountOptionPosition", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryAccountBP(const char* pAccount, EES_AccountBP* pAccoutnBP, int nReqId ){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pAccoutnBP);
    cJSON_AddNumberToObject(j, "nReqId", nReqId);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnQueryAccountBP", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnAccountBPReport(const char* pAccount, EES_AccountBP* pAccoutnBP) {
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pAccoutnBP);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnAccountBPReport", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnQuerySymbol(EES_SymbolField* pSymbol, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pSymbol);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQuerySymbol", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryAccountTradeMargin(const char* pAccount, EES_AccountMargin* pSymbolMargin, bool bFinish ){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pSymbolMargin);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryAccountTradeMargin", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryAccountTradeFee(const char* pAccount, EES_AccountFee* pSymbolFee, bool bFinish ){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pSymbolFee);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryAccountTradeFee", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnOrderAccept(EES_OrderAcceptField* pAccept){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccept);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnOrderAccept", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccept);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnOrderMarketAccept", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnOrderReject(EES_OrderRejectField* pReject){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pReject);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnOrderReject", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnOrderMarketReject(EES_OrderMarketRejectField* pReject) {
    cJSON* j = cJSON_CreateObject();
    to_json(j, pReject);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnOrderMarketReject", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnOrderExecution(EES_OrderExecutionField* pExec){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pExec);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnOrderExecution", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnOrderCxled(EES_OrderCxled* pCxled){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pCxled);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnOrderCxled", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnCxlOrderReject(EES_CxlOrderRej* pReject){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pReject);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnCxlOrderReject", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pQueryOrder);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryTradeOrder", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pAccount, pQueryOrderExec);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryTradeOrderExec", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnPostOrder(EES_PostOrder* pPostOrder){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pPostOrder);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnPostOrder", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnPostOrderExecution(EES_PostOrderExecution* pPostOrderExecution){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pPostOrderExecution);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnPostOrderExecution", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryMarketSession(EES_ExchangeMarketSession* pMarketSession, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pMarketSession);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryMarketSession", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnMarketSessionStatReport(EES_MarketSessionId MarketSessionId, bool ConnectionGood){
    cJSON* j = cJSON_CreateObject();
    cJSON_AddNumberToObject(j, "m_MarketSessionId", MarketSessionId);
    cJSON_AddNumberToObject(j, "m_ConnectionGood", ConnectionGood);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", ConnectionGood, logStr);
    responser->response("OnMarketSessionStatReport", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnSymbolStatusReport(EES_SymbolStatus* pSymbolStatus){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pSymbolStatus);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnSymbolStatusReport", j);
    cJSON_Delete(j);
};

void EESTraderClient::OnQuerySymbolStatus(EES_SymbolStatus* pSymbolStatus, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pSymbolStatus);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQuerySymbolStatus", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQueryMarketMBLData(EES_MarketMBLData* pMarketMBLData, bool bFinish){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pMarketMBLData);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("[%d] %s", bFinish, logStr);
    if(!bFinish){
        responser->response("OnQueryMarketMBLData", j);
    }
    cJSON_Delete(j);
};

void EESTraderClient::OnQuerySessionID(EES_SessionID* pSessionID){
    cJSON* j = cJSON_CreateObject();
    to_json(j, pSessionID);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->response("OnQuerySessionID", j);
    cJSON_Delete(j);
};


////////////////////////////////////////////////////////////////////////
// 工具方法
////////////////////////////////////////////////////////////////////////

int EESTraderClient::loadConfig(std::string iniFileName){
    if(config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    config = new ConfigInfo();
    config->szBlock         = string(_iniFile["ees_trader_client"]["broker"]);
    config->nBrokerId       = _iniFile["ees_trader_client"]["block"];
    config->szUser          = string(_iniFile["ees_trader_client"]["user"]);
    config->szUserPass      = string(_iniFile["ees_trader_client"]["user_pass"]);
    config->szAppId         = string(_iniFile["ees_trader_client"]["app_id"]);
    config->szAuthCode      = string(_iniFile["ees_trader_client"]["auth_code"]);
    
    config->szTradeIp       = string(_iniFile["ees_trader_client"]["trade_ip"]);
    config->nTradePort      = _iniFile["ees_trader_client"]["trade_port"];
    config->szQueryIp        = string(_iniFile["ees_trader_client"]["query_ip"]);
    config->nQueryPort      = _iniFile["ees_trader_client"]["query_port"];
    
    if(config->szUser.empty() | 
        config->szUserPass.empty() |
        config->szAppId.empty() |
        config->szAuthCode.empty() |
        config->szTradeIp.empty() |
        !config->nTradePort |
        config->szQueryIp.empty() |
        !config->nQueryPort )
    {
        LOG_ERROR("Not enough parameters in inifile");
        delete config;
        config = NULL;
        return -1;
    }
    return 0;
};

void EESTraderClient::connect(){
    if(!api){
        LOG_DEBUG("No EESTraderApi instance!");
        return;
    }
    if(!config){
        LOG_DEBUG("No EESTraderApi ConfigInfo in Ini file!");
        return;
    }
    api->SetLoggerSwitch(true);
    api->SetAutoReconnect(true);
    int flag = api->ConnServer(
        config->szTradeIp.c_str(), 
        config->nTradePort, 
        this, 
        config->szQueryIp.c_str(), 
        config->nQueryPort);
    LOG_DEBUG("[%d] %s, %d, %s, %d", flag, 
        config->szTradeIp.c_str(), 
        config->nTradePort,
        config->szQueryIp.c_str(), 
        config->nQueryPort);
};

void EESTraderClient::logon(){
    if(!api){
        LOG_DEBUG("No EESTraderApi instance!");
        return;
    }
    if(!config){
        LOG_DEBUG("No EESTraderApi ConfigInfo in Ini file!");
        return;
    }
    int flag = api->UserLogon(
        config->szUser.c_str(), 
        config->szUserPass.c_str(), 
        config->szAppId.c_str(), 
        config->szAuthCode.c_str());
    LOG_DEBUG("[%d] %s, %s, %s, %s", flag, 
        config->szUser.c_str(), 
        config->szUserPass.c_str(), 
        config->szAppId.c_str(), 
        config->szAuthCode.c_str());
};

void EESTraderClient::getMaxToken(OUT EES_ClientToken* orderToken){
    int flag = api->GetMaxToken(orderToken);
    LOG_DEBUG("[%d] %d", flag, orderToken);
};

void EESTraderClient::querySymbolList(){
    int flag = api->QuerySymbolList();
    LOG_DEBUG("[%d]", flag);
};

void EESTraderClient::queryUserAccount(){
    int flag = api->QueryUserAccount();
    LOG_DEBUG("[%d]", flag);
};

void EESTraderClient::queryAccountPosition(const char* accountId, int nReqId){
    int flag = api->QueryAccountPosition(accountId, nReqId);
    LOG_DEBUG("[%d] %s (%d)", flag, accountId, nReqId);
};

void EESTraderClient::queryAccountBP(const char* accountId, int nReqId){
    int flag = api->QueryAccountBP(accountId, nReqId);
    LOG_DEBUG("[%d] %s (%d)", flag, accountId, nReqId);
};

void EESTraderClient::queryAccountTradeMargin(const char* accountId){
    int flag = api->QueryAccountTradeMargin(accountId);
    LOG_DEBUG("[%d] %s", flag, accountId);
};

void EESTraderClient::queryAccountTradeFee(const char* accountId){
    int flag = api->QueryAccountTradeFee(accountId);
    LOG_DEBUG("[%d] %s", flag, accountId);
};

void EESTraderClient::enterOrder(EES_EnterOrderField* pOrder){
    int flag = api->EnterOrder(pOrder);
    LOG_DEBUG("[%d]", flag);
};

void EESTraderClient::cancelOrder(EES_CancelOrder* pOrder){
    int flag = api->CancelOrder(pOrder);
    LOG_DEBUG("[%d]", flag);
};

void EESTraderClient::queryAccountOrder(const char* accountId){
    int flag = api->QueryAccountOrder(accountId);
    LOG_DEBUG("[%d]", flag);
};

void EESTraderClient::queryAccountOrderExecution(const char* accountId){
    int flag = api->QueryAccountOrderExecution(accountId);
    LOG_DEBUG("[%d]", flag);
};

void EESTraderClient::queryMarketMBLData(int nRequestId, EES_ExchangeID exchangeID, int nSide){
    int flag = api->QueryMarketMBLData(nRequestId, exchangeID, nSide);
    LOG_DEBUG("[%d] %d, %c, %d", flag,  nRequestId, exchangeID, nSide);
};
