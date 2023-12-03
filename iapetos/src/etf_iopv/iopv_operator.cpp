#include "iopv_operator.h"
#include <glog/logging.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string.h>

using namespace nlohmann;

IopvOperator::IopvOperator(std::string etf, std::string config)
{
    memset(m_etf_symbol, 0, TI_SYMBOL_STR_LEN);
    memset(m_etf_exchange, 0, TI_EXCHANGE_STR_LEN);
    m_etf_quotient = 0;
    memset(&m_etf_last_snap, 0, sizeof(TiQuoteSnapshotStockField));
    m_etf_timestamp = 0;
    m_etf_last = 0;
    m_constituent_timestamp = 0;
    nIOPV = 0;

    strcpy(m_etf_symbol, etf.c_str());
    parseConfig(config);
}

IopvOperator::~IopvOperator()
{
    auto iter = m_cashMap.begin();
    for(;iter != m_cashMap.end(); iter++){
        delete iter->second;
        iter->second = NULL;
    }
}

////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

void IopvOperator::parseConfig(std::string &config){
    auto j = json::parse(config);
    
    strcpy(m_etf_exchange, j["etf_exchange"].get<std::string>().c_str());
    m_etf_quotient = j["quotient"].get<double>();

    auto _j = j["stock_weight_array"];
    for (json::iterator it = _j.begin(); it != _j.end(); ++it) {
        m_cashMap[(*it)[0].get<std::string>()] = new ETFConstituentCash((*it)[0].get<std::string>().c_str(), (*it)[1].get<std::string>().c_str(), (*it)[3].get<double>());
    }
};

/// @brief 计算 IOPV
void IopvOperator::calcIopv(){
    nIOPV = 0;
    auto iter = m_cashMap.begin();
    for(; iter != m_cashMap.end(); iter++){
        nIOPV = (nIOPV + iter->second->nFactor);
    }
    nIOPV = nIOPV / m_etf_quotient;
    
    long long _time = m_constituent_timestamp;
    double _iopv = nIOPV;

    printf(
        "calcIopv[%s][%s] [%s][%s], snapTime[%d][%lld][%f] iopv[%lld][%f]\n",
        m_etf_exchange, m_etf_symbol, 
        m_etf_last_snap.exchange, m_etf_last_snap.symbol, 
        m_etf_last_snap.time, m_etf_last_snap.timestamp, m_etf_last_snap.last,
        _time, _iopv);
};


////////////////////////////////////////////////////////////////////////
// 工具方法
////////////////////////////////////////////////////////////////////////
void IopvOperator::getSymbolSet(const char* exchange, std::set<std::string> &symbolSet){
    std::map<std::string, ETFConstituentCash*>::iterator iter = m_cashMap.begin();
    for (; iter != m_cashMap.end() ; iter ++)
    {
        if(!strcmp(exchange, iter->second->szExchange)){
            symbolSet.insert(iter->second->szSymbol);
        }
    }
    if(!strcmp(m_etf_exchange, exchange)){
        symbolSet.insert(m_etf_symbol);
    }
};

////////////////////////////////////////////////////////////////////////
// 行情回调
////////////////////////////////////////////////////////////////////////

void IopvOperator::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    auto iter = m_cashMap.find(pData->symbol);
    if(iter != m_cashMap.end()){
        iter->second->OnL2StockSnapshotRtn(pData);
    }

    if(strcmp(pData->symbol, m_etf_symbol)){
        return;
    }
    memcpy(&m_etf_last_snap, pData, sizeof(TiQuoteSnapshotStockField));
};

void IopvOperator::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    auto iter = m_cashMap.find(pData->symbol);
    if(iter != m_cashMap.end()){
        iter->second->OnL2StockMatchesRtn(pData);
    }
    //500 毫秒计算一次
    if((pData->timestamp - m_constituent_timestamp) > 500)
    {
        calcIopv();
        m_constituent_timestamp = pData->timestamp;
    }

    if(strcmp(pData->symbol, m_etf_symbol)){
        return;
    }
    if( pData->volume && pData->price ){
        m_etf_timestamp = pData->timestamp;
        m_etf_last = pData->price;
    }
};