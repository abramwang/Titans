#include "ia_etf_factor_center.h"
#include <glog/logging.h>
#include <set>
#include <iostream>
#include "datetime.h"
#include "iniFile.h"

#define DISABLE_THREAD_POOL 0
#define USE_BOOK_ENGINE 0

////////////////////////////////////////////////////////////////////////
// 静态方法
////////////////////////////////////////////////////////////////////////
void IaETFFactorCenter::calc_work(uv_work_t* req)
{
    CalcWorkReqInfo* info = (CalcWorkReqInfo*)req->data;
    
    for (auto iter = info->m_operator_list.begin(); iter != info->m_operator_list.end(); iter++)
    {
        if(info->type == TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK){
            (*iter)->OnL2StockSnapshotRtn(info->m_snapshot);
        }else if(info->type == TI_QUOTE_DATA_TYPE_MATCH){
            (*iter)->OnL2StockMatchesRtn(info->m_match);
        }
    }
};
////////////////////////////////////////////////////////////////////////
// 构造方法
////////////////////////////////////////////////////////////////////////
IaETFFactorCenter::IaETFFactorCenter(int thread_num, std::string configPath)
{
    uv_loop_init(&m_loop);
    m_config = NULL;
    m_fund_info = NULL;
    m_timestamp = 0;
    m_quoteClient = new TiZtQuoteClient(configPath.c_str(), this);
    m_quoteHistoryClient = new TiQuoteHistoryClientLocal(configPath.c_str(), this);
    m_output_factors_to_redis = true;
    m_book_engine = new TiBookEngine(this, 500, 10);
    loadConfig(configPath);

    if(m_config){
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[IaETFFactorCenter] flag: " << flag;

        m_fund_info = new IaETFFundInfo(&m_redis, m_config->szConfigETFListKey, m_config->szConfigETFRealityInfoKey, m_config->szConfigETFDisclosureInfoKey);
        init_operators(thread_num);
        m_redis.del(m_config->szStreamKey.c_str());
    }
    
    if(!m_config->bEnableHistory){
        m_quoteClient->connect();
    }
    subStock("SH");
    subStock("SZ");

    if(m_config->bEnableHistory){
#if USE_BOOK_ENGINE
        TI_QUOTE_DATA_TYPE dataTypeList[3] = {TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, TI_QUOTE_DATA_TYPE_ORDER, TI_QUOTE_DATA_TYPE_MATCH};
        m_quoteHistoryClient->replay(m_config->nHistoryDateNum, dataTypeList, 3);
#else
        TI_QUOTE_DATA_TYPE dataTypeList[1] = {TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK};
        m_quoteHistoryClient->replay(m_config->nHistoryDateNum, dataTypeList, 1);
#endif
    }
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
    
    m_config->bEnableHistory    = bool(_iniFile["ia_etf_factor_center"]["enable_history"]);
    m_config->nHistoryDateNum   = int(_iniFile["ia_etf_factor_center"]["history_date_num"]);
    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szConfigETFRealityInfoKey.empty() |
        m_config->szConfigETFDisclosureInfoKey.empty() |
        m_config->szStreamKey.empty() )
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
    for (int i = 0; i < thread_num; i++)
    {
        CalcWorkReqInfo* _info = new CalcWorkReqInfo();
        _info->type = TI_QUOTE_DATA_TYPE_NONE;
        _info->m_snapshot = NULL;
        _info->m_match = NULL;
        
        uv_work_t* req = (uv_work_t*)malloc(sizeof(uv_work_t));
        memset(req, 0, sizeof(uv_work_t));
        req->data = _info;
        m_calcWorkReqList.push_back(req);
    }
    
    std::list<uv_work_t*>::iterator iter = m_calcWorkReqList.begin();
    std::map<std::string, std::shared_ptr<IaETFFundInfo::FundInfo>>* fund_map = m_fund_info->GetFundMap();
    for (auto i = fund_map->begin(); i != fund_map->end(); i++)
    {
        IaETFFactorOperator* _operator = new IaETFFactorOperator(i->first, i->second.get(), this);
        m_operatorList.push_back(_operator);

        if(iter == m_calcWorkReqList.end()){
            iter = m_calcWorkReqList.begin();
        }
        CalcWorkReqInfo* _info = (CalcWorkReqInfo*)(*iter)->data;
        _info->m_operator_list.push_back(_operator);
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

/// @brief 
/// @param day 交易日
/// @param exchange 交易所代码
/// @param exchangeName 交易所简称
void IaETFFactorCenter::OnTradingDayRtn(const unsigned int day, const char* exchangeName) {
    printf("OnTradingDayRtn: exchange[%s] trading day[%d] \n",
        exchangeName, day);
    subStock(exchangeName);
};

void IaETFFactorCenter::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){
#if USE_BOOK_ENGINE
    if(pData->time < 93000000){
        OnDepthSnapshotRtn(pData);
        return;
    }
    if(pData->time >= 145700000){
        OnDepthSnapshotRtn(pData);
        return;
    }
    if(m_book_engine){
        m_book_engine->RecvStockSnap(pData);
    }
#else
    OnDepthSnapshotRtn(pData);
#endif
};

void IaETFFactorCenter::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
#if USE_BOOK_ENGINE
    if(m_book_engine){
        m_book_engine->RecvMatch(pData);
    }
#endif
};
void IaETFFactorCenter::OnL2StockOrderRtn(const TiQuoteOrderField* pData){
#if USE_BOOK_ENGINE
    if(m_book_engine){
        m_book_engine->RecvOrder(pData);
    }
#endif
};

void IaETFFactorCenter::OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids)
{
    if (asks.size() < 10 || bids.size() < 10)
    {
        return;
    }
    strcpy(m_book_snap_cash.exchange, pBase->exchange);
    strcpy(m_book_snap_cash.symbol, pBase->symbol);
    m_book_snap_cash.date = pBase->date;
    m_book_snap_cash.time = pBase->time;
    m_book_snap_cash.timestamp = pBase->timestamp;
    m_book_snap_cash.last = pBase->last;
    m_book_snap_cash.pre_close = pBase->pre_close;
    m_book_snap_cash.open = pBase->open;
    m_book_snap_cash.high = pBase->high;
    m_book_snap_cash.low = pBase->low;
    m_book_snap_cash.high_limit = pBase->high_limit;
    m_book_snap_cash.low_limit = pBase->low_limit;
    m_book_snap_cash.acc_volume = pBase->acc_volume;
    m_book_snap_cash.acc_turnover = pBase->acc_turnover;
    m_book_snap_cash.match_items = pBase->match_items;

    
    for (size_t i = 0; i < 10; i++)
    {
        m_book_snap_cash.ask_price[i] = asks[i]->price;
        m_book_snap_cash.ask_volume[i] = asks[i]->volume;
        m_book_snap_cash.ask_order_num[i] = asks[i]->order_count;
        m_book_snap_cash.bid_price[i] = bids[i]->price;
        m_book_snap_cash.bid_volume[i] = bids[i]->volume;
        m_book_snap_cash.bid_order_num[i] = bids[i]->order_count;
    }

    OnDepthSnapshotRtn(&m_book_snap_cash);
};

void IaETFFactorCenter::OnDepthSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
#if DISABLE_THREAD_POOL
    //printf("OnL2StockSnapshotRtn: exchange[%s] symbol[%s] %f, %f, %d\n", pData->exchange, pData->symbol, pData->last, pData->ask_price[0], pData->ask_volume[0]);
    std::list<IaETFFactorOperator*>::iterator iter = m_operatorList.begin();
    for(; iter != m_operatorList.end(); iter++){
        (*iter)->OnL2StockSnapshotRtn(pData);
    }
#else
    for (auto iter = m_calcWorkReqList.begin(); iter != m_calcWorkReqList.end(); iter++)
    {
        CalcWorkReqInfo* info = (CalcWorkReqInfo*)(*iter)->data;
        info->m_snapshot = (TiQuoteSnapshotStockField*)pData;
        info->type = TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK;
        uv_queue_work(&m_loop, *iter, IaETFFactorCenter::calc_work, NULL);
    }
    uv_run(&m_loop, UV_RUN_DEFAULT);
#endif

    if ((pData->timestamp - m_timestamp) >= 1000)
    {
        TI_ISODateTimeType _iso_time;
        datetime::get_format_timestamp_ms(pData->timestamp, _iso_time, TI_TIME_STR_LEN);
        //std::cout << _iso_time << std::endl;
        m_timestamp = pData->timestamp;
#if 1   
        if(m_output_factors_to_redis){
#if 0
            if (pData->time > 112900000 && pData->time< 130100000)
            {
                json j_2 = json::array();
                IaETFFactorOperator* op = nullptr;
                std::list<IaETFFactorOperator*>::iterator iter = m_operatorList.begin();
                for(; iter != m_operatorList.end(); iter++){
                    if(!strcmp((*iter)->m_etf_last_snap.symbol, "560010")){
                        op = *iter;
                        break;
                    }
                }
                if(op)
                {
                    int index = 0;
                    for(auto iter = op->m_cashMap.begin(); iter !=op->m_cashMap.end(); iter++){
                        j_2[index] = iter->second->m_counting_process_json;
                        index++;
                    }
                    m_redis.xadd("etf_counting_process", j_2.dump().c_str());
                }
            }
#endif
            json j = {
                { "type", "etf_factor"},
                { "data", json::array()}
            };
            std::list<IaETFFactorOperator*>::iterator iter = m_operatorList.begin();
            int i = 0;
            for(; iter != m_operatorList.end(); iter++){
                j["data"][i] = (*iter)->m_factor_json;
                i++;
            }
            
            //if(!m_config->bEnableHistory){
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
            //}
        }
#endif
    }
};