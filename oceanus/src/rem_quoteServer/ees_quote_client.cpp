#include "ees_quote_client.h"
#include "log.h"
#include "iniFile.h"
#include "ees_quote_response_msg.h"

using namespace EES_QUOTE_RESPONSE_MSG;

EESQuoteClient::EESQuoteClient(std::string configPath)
{
    api = NULL;
    config = NULL;
    api = CreateEESQuoteApi();
    loadConfig(configPath);
    responser = new QuoteResponser(configPath);
};

EESQuoteClient::~EESQuoteClient(){
	if(api){
        DestroyEESQuoteApi(api);
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

void EESQuoteClient::OnEqsConnected(){
	LOG_DEBUG("");
	logon();
};

void EESQuoteClient::OnEqsDisconnected(){
	LOG_DEBUG("");
}

void EESQuoteClient::OnLoginResponse(bool bSuccess, const char* pReason) {
	LOG_DEBUG("[%d,%s]", bSuccess, pReason);
    if(!bSuccess){
        return;
    }
    querySymbolList();
};

void EESQuoteClient::OnWriteTextLog(EesEqsLogLevel nlevel, const char* pLogText, int nLogLen) {
	LOG_DEBUG("[%d,%s]", nlevel, pLogText);
};

void EESQuoteClient::OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess) {
	LOG_DEBUG("[%d,%s,%d]", chInstrumentType, pSymbol, bSuccess);
};

void EESQuoteClient::OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess) {
	LOG_DEBUG("[%d,%s,%d]", chInstrumentType, pSymbol, bSuccess);
};

void EESQuoteClient::OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast) {
	LOG_DEBUG("[%d,%s,%d]", chInstrumentType, pSymbol, bLast);
    if(bLast){
        registerAllFuture();
        //registerAllOption();
        return;
    }
    // 内存缓存
    m_symbolSetMap[(int)chInstrumentType].insert(pSymbol);
    // 存入 redis
    cJSON* j = cJSON_CreateObject();
    cJSON_AddNumberToObject(j, "type",chInstrumentType);
    cJSON_AddStringToObject(j, "symbol", pSymbol);
    responser->updateSymbolInfo(j);
    cJSON_Delete(j);
};

void EESQuoteClient::OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData) {
	cJSON* j = cJSON_CreateObject();
    to_json(j, pDepthQuoteData);
    cJSON_AddNumberToObject(j, "chInstrumentType", chInstrumentType);
    char* logStr = cJSON_Print(j);
    cJSON_Minify(logStr);
    LOG_DEBUG("%s", logStr);
    responser->updateSnapshot(pDepthQuoteData->ExchangeID, pDepthQuoteData->InstrumentID, j);
    cJSON_Delete(j);
};


////////////////////////////////////////////////////////////////////////
// 工具方法
////////////////////////////////////////////////////////////////////////

int EESQuoteClient::loadConfig(std::string iniFileName){
    if(config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    config = new ConfigInfo();
    config->szBroker            = string(_iniFile["ees_quote_client"]["broker"]);
    config->nBrokerId           = _iniFile["ees_quote_client"]["broker_id"];
    config->szAccount           = string(_iniFile["ees_quote_client"]["account"]);
    config->szAccountPass       = string(_iniFile["ees_quote_client"]["account_pass"]);
    
    config->szIp       = string(_iniFile["ees_quote_client"]["ip"]);
    config->nPort      = _iniFile["ees_quote_client"]["port"];
    
    if(config->szAccount.empty() | 
        config->szAccountPass.empty() |
        config->szIp.empty() |
        !config->nPort)
    {
        LOG_ERROR("Not enough parameters in inifile");
        delete config;
        config = NULL;
        return -1;
    }
    return 0;
};

void EESQuoteClient::registerAllFuture(){
    auto futureSet = m_symbolSetMap[54];

    std::set<std::string>::iterator iter = futureSet.begin();
    for(; iter != futureSet.end(); iter++){
        registerSymbol(54, iter->c_str());
    }
};

void EESQuoteClient::registerAllOption(){
    auto optionSet = m_symbolSetMap[52];

    std::set<std::string>::iterator iter = optionSet.begin();
    for(; iter != optionSet.end(); iter++){
        registerSymbol(52, iter->c_str());
    }
};

////////////////////////////////////////////////////////////////////////
// 外部调用方法
////////////////////////////////////////////////////////////////////////

void EESQuoteClient::connect(){
    if(!api){
        LOG_DEBUG("No EESQuoteClient instance!");
        return;
    }
    if(!config){
        LOG_DEBUG("No EESQuoteClient ConfigInfo in Ini file!");
        return;
    }
	EqsTcpInfo conInfo;
	strcpy(conInfo.m_eqsIp, config->szIp.c_str());
	conInfo.m_eqsPort = config->nPort;

    int flag = api->ConnServer(conInfo, this);
    LOG_DEBUG("[%d] %s, %d", flag, config->szIp.c_str(), config->nPort);
};

void EESQuoteClient::logon(){
    if(!api){
        LOG_DEBUG("No EESQuoteClient instance!");
        return;
    }
	EqsLoginParam userInfo;
	strcpy(userInfo.m_loginId, config->szAccount.c_str());
	strcpy(userInfo.m_password, config->szAccountPass.c_str());
    LOG_DEBUG("%s, %s", config->szAccount.c_str(), config->szAccountPass.c_str());
	api->LoginToEqs(userInfo);
};

void EESQuoteClient::querySymbolList(){
    if(!api){
        LOG_DEBUG("No EESQuoteClient instance!");
        return;
    }
    LOG_DEBUG("");
    api->QuerySymbolList();
};

void EESQuoteClient::registerSymbol(int type, const char* symbol){
    if(!api){
        LOG_DEBUG("No EESQuoteClient instance!");
        return;
    }
    LOG_DEBUG("%d, %s", type, symbol);
	api->RegisterSymbol((EesEqsIntrumentType)type, symbol); 
};

void EESQuoteClient::unregisterSymbol(int type, const char* symbol){
    if(!api){
        LOG_DEBUG("No EESQuoteClient instance!");
        return;
    }
    LOG_DEBUG("%d, %s", type, symbol);
	api->UnregisterSymbol((EesEqsIntrumentType)type, symbol); 
};