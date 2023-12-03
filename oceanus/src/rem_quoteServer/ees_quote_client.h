#ifndef EES_QUOTE_CLIENT_H
#define EES_QUOTE_CLIENT_H

#include <string>
#include <map>
#include <set>
#include <EESQuoteApi.h>
#include "quote_responser.h"

class EESQuoteClient : public EESQuoteEvent
{
public:
	typedef struct ConfigInfo
    {
        std::string	szBroker;
        int 		nBrokerId;

        std::string szAccount;
        std::string szAccountPass;

        std::string szIp;
        int 		nPort;
    } ConfigInfo;
private:
	EESQuoteApi* api;
    ConfigInfo* config;
	QuoteResponser* responser;

	std::map<int, std::set<std::string>> m_symbolSetMap; 
public:
	EESQuoteClient(std::string configPath);
	virtual ~EESQuoteClient();
	
	void OnEqsConnected();
	void OnEqsDisconnected();

	void OnLoginResponse(bool bSuccess, const char* pReason);
	void OnWriteTextLog(EesEqsLogLevel nlevel, const char* pLogText, int nLogLen);
	void OnSymbolRegisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
	void OnSymbolUnregisterResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bSuccess);
	void OnSymbolListResponse(EesEqsIntrumentType chInstrumentType, const char* pSymbol, bool bLast);

	void OnQuoteUpdated(EesEqsIntrumentType chInstrumentType, EESMarketDepthQuoteData* pDepthQuoteData);
private:
    int loadConfig(std::string iniFileName);
	void registerAllFuture();
	void registerAllOption();
public:
	void connect();
	void logon();
	void querySymbolList();
	void registerSymbol(int type, const char* symbol);
	void unregisterSymbol(int type, const char* symbol);
};

#endif