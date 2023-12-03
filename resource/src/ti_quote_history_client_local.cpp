#include "ti_quote_history_client_local.h"
#include <glog/logging.h>
#include "iniFile.h"

TiQuoteHistoryClientLocal::TiQuoteHistoryClientLocal(std::string configPath, TiQuoteCallback* userCb){
    m_config = NULL;
    m_quote_reader = NULL;
    m_cb = userCb;
    loadConfig(configPath);
    if(m_config){
        m_quote_reader = std::shared_ptr<TiQuoteReader>(new TiQuoteReader(m_config->szDataRoot.c_str()));
    }
};


int TiQuoteHistoryClientLocal::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szDataRoot                  = string(_iniFile["ti_quote_history_client_local"]["data_root"]);
    
    if( m_config->szDataRoot.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};


void TiQuoteHistoryClientLocal::subData(const char* exchangeName, char* codeList[], size_t len){
    for (size_t i = 0; i < len; i++)
    {
        TiSymbolInfo symbolInfo;
        strcpy(symbolInfo.exchange, exchangeName);
        strcpy(symbolInfo.symbol, codeList[i]);
        m_symbol_vec.push_back(symbolInfo);
    }
};

void TiQuoteHistoryClientLocal::replay(int dateNum, TI_QUOTE_DATA_TYPE dataTypeList[], size_t len){
    if(!m_cb){
        LOG(INFO) << "[replay] Do not have user callback";
        return;
    }
    std::set<std::string> exchange_set;
    TiSymbolInfo* codeList = new TiSymbolInfo[m_symbol_vec.size()];
    for (size_t i = 0; i < m_symbol_vec.size(); i++)
    {
        codeList[i] = m_symbol_vec[i];
        exchange_set.insert(m_symbol_vec[i].exchange);
    }
    
    if (m_symbol_vec.empty())
    {
        m_quote_reader->ReadQuoteData(dateNum, "ALL", dataTypeList, len);
        m_cb->OnTradingDayRtn(dateNum, "SH");
        m_cb->OnTradingDayRtn(dateNum, "SZ");
    }else{
        m_quote_reader->ReadQuoteData(dateNum, codeList, m_symbol_vec.size(), dataTypeList, len);
    
        for (auto iter = exchange_set.begin(); iter != exchange_set.end(); iter++)
        {
            TI_ExchangeType exchange_type;
            strcpy(exchange_type, iter->c_str());
            m_cb->OnTradingDayRtn(dateNum, exchange_type);
        }
    }
    
    
    TiQuoteData dataCash = {0};
    while (m_quote_reader->GetNext(&dataCash))
    {
        switch (dataCash.type)
        {
        case TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK:
            m_cb->OnL2StockSnapshotRtn(&dataCash.m_snapshot_stock);
            break;
        case TI_QUOTE_DATA_TYPE_ORDER:
            m_cb->OnL2StockOrderRtn(&dataCash.m_order);
            break;
        case TI_QUOTE_DATA_TYPE_MATCH:
            m_cb->OnL2StockMatchesRtn(&dataCash.m_matches);
            break;
        default:
            break;
        }
    }
    std::cout << "replay end" << std::endl;
    delete[] codeList;
};