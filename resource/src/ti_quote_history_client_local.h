#ifndef TI_QUOTE_HISTORY_CLIENT_LOCAL_H
#define TI_QUOTE_HISTORY_CLIENT_LOCAL_H

#include <string>
#include <vector>
#include "ti_quote_callback.h"
#include "ti_quote_client.h"
#include "ti_quote_reader.h"

class TiQuoteHistoryClientLocal:
    public TiQuoteClient
{
    typedef struct ConfigInfo
    {
        std::string szDataRoot;
    } ConfigInfo;
private:
    ConfigInfo* m_config;
    std::shared_ptr<TiQuoteReader> m_quote_reader;   //
    std::vector<TiSymbolInfo>      m_symbol_vec;    //
    TiQuoteCallback* m_cb;
private:
    int loadConfig(std::string iniFileName);
public:
	void connect(){};
    void subData(const char* exchangeName, char* codeList[], size_t len);
    void replay(int dateNum, TI_QUOTE_DATA_TYPE dataTypeList[], size_t len);
public:
    TiQuoteHistoryClientLocal(std::string configPath, TiQuoteCallback* userCb);
    virtual ~TiQuoteHistoryClientLocal(){};
};

#endif 