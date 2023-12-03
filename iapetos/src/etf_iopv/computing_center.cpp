#include "computing_center.h"
#include <glog/logging.h>
#include "iniFile.h"

ComputingCenter::ComputingCenter(std::string configPath)
{
    m_quoteClient = new TiHxQuoteClient(configPath.c_str(), this);
    m_config = NULL;
    loadConfig(configPath);
    if(m_config){
        bool flag = connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[ComputingCenter] flag: " << flag;
    }
    loadIOPVConfig();
    m_quoteClient->connect();
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////


int ComputingCenter::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["computing_center"]["ip"]);
    m_config->nPort       = _iniFile["computing_center"]["port"];
    m_config->szAuth      = string(_iniFile["computing_center"]["auth"]);
    
    m_config->szConfigKey     = string(_iniFile["computing_center"]["config_key"]);
    m_config->szStreamKey     = string(_iniFile["computing_center"]["stream_key"]);
    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szConfigKey.empty() |
        m_config->szStreamKey.empty() )
    {
        LOG(ERROR) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

int ComputingCenter::loadIOPVConfig(){
    if(!m_config){
        return -1;
    }
    std::map<std::string, std::string> _configMap;
    if (hgetall(m_config->szConfigKey.c_str(), _configMap)){
        std::map<std::string, std::string>::iterator iter = _configMap.begin();
        for (; iter !=  _configMap.end(); iter++)
        {
            IopvOperator* _operator = new IopvOperator(iter->first, iter->second);
            m_operatorList.push_back(_operator);
        }
    };
    return 0;
};

void ComputingCenter::subStock(const char* exchangeName){
    std::set<std::string> _sub_symbol_set;
    std::list<IopvOperator*>::iterator iter = m_operatorList.begin();
    for(; iter != m_operatorList.end(); iter++){
        (*iter)->getSymbolSet(exchangeName, _sub_symbol_set);
    }
    char* _subSymbolArray[1000] = {0};
    int i = 0;
    std::set<std::string>::iterator _set_iter = _sub_symbol_set.begin();
    for(; _set_iter != _sub_symbol_set.end(); _set_iter++){
        _subSymbolArray[i] = (char*)_set_iter->c_str();
        i++;
    }
    printf("[%s] sub_symbol num: %ld\n", exchangeName, _sub_symbol_set.size());
    m_quoteClient->subData(exchangeName, _subSymbolArray, _sub_symbol_set.size());
};

////////////////////////////////////////////////////////////////////////
// 行情回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param day 交易日
/// @param exchange 交易所代码
/// @param exchangeName 交易所简称
void ComputingCenter::OnTradingDayRtn(const unsigned int day, const char* exchangeName) {
    printf("OnTradingDayRtn: exchange[%s] trading day[%d] \n",
        exchangeName, day);
    subStock(exchangeName);
};

void ComputingCenter::OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){
    std::list<IopvOperator*>::iterator iter = m_operatorList.begin();
    for(; iter != m_operatorList.end(); iter++){
        (*iter)->OnL2IndexSnapshotRtn(pData);
    }
};

void ComputingCenter::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
    std::list<IopvOperator*>::iterator iter = m_operatorList.begin();
    for(; iter != m_operatorList.end(); iter++){
        (*iter)->OnL2StockSnapshotRtn(pData);
    }
};

void ComputingCenter::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    std::list<IopvOperator*>::iterator iter = m_operatorList.begin();
    for(; iter != m_operatorList.end(); iter++){
        (*iter)->OnL2StockMatchesRtn(pData);
    }
};