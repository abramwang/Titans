#include "ia_etf_factor_center.h"
#include <glog/logging.h>
#include <set>
#include <iostream>
#include "datetime.h"
#include "iniFile.h"

#define DISABLE_THREAD_POOL 1
#define USE_BOOK_ENGINE 0

////////////////////////////////////////////////////////////////////////
// 构造方法
////////////////////////////////////////////////////////////////////////
IaETFFactorCenter::IaETFFactorCenter(int thread_num, uv_loop_t *loop, std::string configPath)
{
    m_config = NULL;
    m_fund_info = NULL;
    m_timestamp = 0;
    m_quoteClient = new TiQuoteIpcClient(configPath.c_str(), loop, this, this);
    m_quoteHistoryClient = new TiQuoteHistoryClientLocal(configPath.c_str(), this);
    m_output_factors_to_redis = true;
    loadConfig(configPath);

    m_cout_snap_time = 0;

    if(m_config){
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[IaETFFactorCenter] flag: " << flag;

        m_fund_info = new IaETFFundInfo(&m_redis, m_config->szConfigETFListKey, m_config->szConfigETFRealityInfoKey, m_config->szConfigETFDisclosureInfoKey);
        init_operators(thread_num);
        //m_redis.del(m_config->szStreamKey.c_str());
    }
    
    if(!m_config->bEnableHistory){
        m_quoteClient->connect();
    }
    subStock("SH");
    subStock("SZ");

    if(m_config->bEnableHistory){
        TI_QUOTE_DATA_TYPE dataTypeList[1] = {TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK};
        m_quoteHistoryClient->replay(m_config->nHistoryDateNum, dataTypeList, 1);
    }else{
        if (m_config->szIpcQuoteTopic.empty())
        {
            m_quoteClient->run(NULL);
        }else{
            m_quoteClient->run(m_config->szIpcQuoteTopic.c_str());
        }
    }

    m_timer.data = this;
    uv_timer_init(loop, &m_timer);
    uv_timer_start(&m_timer, onTimer, 1000, 500);
};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////


int IaETFFactorCenter::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["ia_etf_factor_center"]["ip"]);
    m_config->nPort       = _iniFile["ia_etf_factor_center"]["port"];
    m_config->szAuth      = string(_iniFile["ia_etf_factor_center"]["auth"]);
    
    m_config->szConfigETFListKey           = string(_iniFile["ia_etf_factor_center"]["etf_calculated_list_key"]);
    m_config->szConfigETFRealityInfoKey    = string(_iniFile["ia_etf_factor_center"]["etf_reality_info_key"]);
    m_config->szConfigETFDisclosureInfoKey = string(_iniFile["ia_etf_factor_center"]["etf_disclosure_info_key"]);
    m_config->szStreamKey                  = string(_iniFile["ia_etf_factor_center"]["stream_key"]);
    
    m_config->szUiFactorKey                = string(_iniFile["ia_etf_factor_center"]["stream_ui_factor_key"]);
    m_config->szUiCostKey                  = string(_iniFile["ia_etf_factor_center"]["stream_ui_cost_key"]);
    
    m_config->szIpcQuoteTopic              = string(_iniFile["ia_etf_factor_center"]["ipc_quote_topic"]);

    m_config->bEnableHistory    = bool(_iniFile["ia_etf_factor_center"]["enable_history"]);
    m_config->nHistoryDateNum   = int(_iniFile["ia_etf_factor_center"]["history_date_num"]);
    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szConfigETFRealityInfoKey.empty() |
        m_config->szConfigETFDisclosureInfoKey.empty())
    {
        LOG(ERROR) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void IaETFFactorCenter::init_operators(int thread_num)
{
    std::map<std::string, std::shared_ptr<IaETFFundInfo::FundInfo>>* fund_map = m_fund_info->GetFundMap();
    for (auto i = fund_map->begin(); i != fund_map->end(); i++)
    {
        IaETFFactorOperator* _operator = new IaETFFactorOperator(i->first, i->second.get(), this);
        m_operatorList.push_back(_operator);
    }
};

void IaETFFactorCenter::subStock(const char* exchangeName){
    //std::set<std::string> symbol_set;
    //m_fund_info->GetSymbolList(exchangeName, symbol_set);
    //std::cout << "IaETFFactorCenter::subStock " << symbol_set.size() << std::endl;
    std::set<std::string> _sub_symbol_set;
    std::list<IaETFFactorOperator*>::iterator iter = m_operatorList.begin();
    for(; iter != m_operatorList.end(); iter++){
        (*iter)->getSymbolSet(exchangeName, _sub_symbol_set);
    }
    char* _subSymbolArray[4000] = {0};
    int i = 0;
    std::set<std::string>::iterator _set_iter = _sub_symbol_set.begin();
    for(; _set_iter != _sub_symbol_set.end(); _set_iter++){
        _subSymbolArray[i] = (char*)_set_iter->c_str();
        i++;
    }
    printf("[%s] sub_symbol num: %ld\n", exchangeName, _sub_symbol_set.size());
    if(m_config->bEnableHistory){
        m_quoteHistoryClient->subData(exchangeName, _subSymbolArray, _sub_symbol_set.size());
    }else{
        m_quoteClient->subData(exchangeName, _subSymbolArray, _sub_symbol_set.size());
    }
};

////////////////////////////////////////////////////////////////////////
// 行情回调方法
////////////////////////////////////////////////////////////////////////

void IaETFFactorCenter::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    if ((pData->time - m_cout_snap_time) > 15000)
    {
        printf("[IaETFFactorCenter::OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
            pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
        m_cout_snap_time = pData->time;
    }
    std::list<IaETFFactorOperator*>::iterator iter = m_operatorList.begin();
    for(; iter != m_operatorList.end(); iter++){
        (*iter)->OnL2StockSnapshotRtn(pData);
    }

#if 1
    if ((pData->timestamp - m_timestamp) >= 1000)
    {
        TI_ISODateTimeType _iso_time;
        datetime::get_format_timestamp_ms(pData->timestamp, _iso_time, TI_TIME_STR_LEN);
        //std::cout << _iso_time << std::endl;
        m_timestamp = pData->timestamp;
        if(m_output_factors_to_redis){
            json j = {
                { "type", "etf_factor"},
                { "data", json::array()}
            };
            std::list<IaETFFactorOperator*>::iterator iter = m_operatorList.begin();
            for(; iter != m_operatorList.end(); iter++){
                j["data"].push_back((*iter)->m_factor_json);
#if 0
                j["data"].push_back((*iter)->m_factor_counting_process_json);
#endif
            }
            
            bool flag = m_redis.xadd(m_config->szStreamKey.c_str(), j.dump().c_str());
            
            if (!flag){
                std::cout << "try reconnect" << std::endl;
                flag = m_redis.disconnect();
                if (flag)
                {
                    flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
                    flag = m_redis.xadd(m_config->szStreamKey.c_str(), j.dump().c_str());
                }
            }
        }
    }
#endif
};

void IaETFFactorCenter::OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data){
    return;
    std::cout << "OnFactorRtn " << symbol << " " << factor_name << std::endl;
    if(!m_output_factors_to_redis)
        return;
    json j = {
        { "type", factor_name},
        { "symbol", symbol},
        { "data", *pFactor}
    };
    bool flag = m_redis.xadd(m_config->szUiFactorKey.c_str(), j.dump().c_str());
    if (!flag){
        std::cout << "try reconnect" << std::endl;
        flag = m_redis.disconnect();
        if (flag)
        {
            flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
            flag = m_redis.xadd(m_config->szUiFactorKey.c_str(), j.dump().c_str());
        }
    }        
};


void IaETFFactorCenter::OnTimer()
{
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    /*
    std::cout << "当前时间: "
            << localTime->tm_year + 1900 << "-" << localTime->tm_mon + 1 << "-" << localTime->tm_mday << " "
            << localTime->tm_hour << ":" << localTime->tm_min << ":" << localTime->tm_sec
            << std::endl;
    */
    if (localTime->tm_hour > 16 )
    {
        std::cout << "terminate" << std::endl;
        std::terminate();
    }
};

void IaETFFactorCenter::onTimer(uv_timer_t* handle)
{
    IaETFFactorCenter* pThis = (IaETFFactorCenter*)handle->data;
    pThis->OnTimer();
};
