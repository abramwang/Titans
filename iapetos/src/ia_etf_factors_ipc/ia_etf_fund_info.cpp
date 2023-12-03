#include "ia_etf_fund_info.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;
#include <iostream>

IaETFFundInfo::IaETFFundInfo(RedisSyncHandle* redis, std::string etf_list_key, std::string reality_key, std::string disclosure_key)
{
    m_redis = redis;
    parseRedisETFListData(etf_list_key, reality_key, disclosure_key);
    parseRedisDisclosureInfoData(disclosure_key);
    parseRedisRealityInfoData(reality_key);
}

void IaETFFundInfo::parseRedisETFListData(std::string etf_list_key, std::string reality_key, std::string disclosure_key){
    if(!m_redis)
    {
        return;
    }
    if (!etf_list_key.empty())
    {
        m_redis->smembers(etf_list_key.c_str(), m_etfList);
        return;
    }
    if (!disclosure_key.empty())
    {
        std::vector<std::string> _disclosure_key_list;
        std::vector<std::string> _reality_key_list;
        m_redis->hkeys(disclosure_key.c_str(), _disclosure_key_list);
        m_redis->hkeys(reality_key.c_str(), _reality_key_list);

        std::set<std::string> _disclosure_key_set(_disclosure_key_list.begin(), _disclosure_key_list.end());
        std::set<std::string>::iterator iter = _disclosure_key_set.begin();
        for (size_t i = 0; i < _reality_key_list.size(); i++)
        {
            iter = _disclosure_key_set.find(_reality_key_list[i]);
            if (iter != _disclosure_key_set.end())
            {
                m_etfList.push_back(_reality_key_list[i]);
            }
        }
        std::cout << "disclosure_key_list size: " << _disclosure_key_list.size() << std::endl;
        std::cout << "reality_key_list size: " << _reality_key_list.size() << std::endl;
        std::cout << "m_etfList size: " << m_etfList.size() << std::endl;
        return;
    }
};

void IaETFFundInfo::parseRedisDisclosureInfoData(std::string disclosure_key)
{
    if(!m_redis)
    {
        return;
    }
    for (size_t i = 0; i < m_etfList.size(); i++)
    {  
        std::string result; 
        m_redis->hget(disclosure_key.c_str(), m_etfList[i].c_str(), result);
        if (!result.empty())
        {
            std::shared_ptr<FundInfo> _fund = std::shared_ptr<FundInfo>(new FundInfo);
            json j = json::parse(result);

            _fund->m_fundId = j["symbol"].get<std::string>();
            _fund->m_exchange = j["exchange"].get<std::string>();
            _fund->m_minUnit = j["unit"].get<int>();
            _fund->m_preMinUnitTurnover = j["cash_component"].get<double>();
            _fund->m_publicEstimatedCashDifference = j["estimate_amount"].get<double>();
            _fund->m_realEstimatedCashDifference = j["cash_component"].get<double>();
            _fund->m_constituentStockNum = j["component_list"].size();
            _fund->m_IOPV = j["net_value"].get<double>();
            _fund->m_sh_commission = _fund->m_exchange == "SH" ?  0.00016 : 0.0008;
            _fund->m_sz_commission = _fund->m_exchange == "SZ" ?  0.00016 : 0.0008;
            
            for (json::iterator it = j["component_list"].begin(); it != j["component_list"].end(); ++it) {
                _fund->m_stockList.push_back({
                    (*it)["symbol"].get<std::string>(),
                    (*it)["exchange"].get<std::string>(),
                    (*it)["name"].get<std::string>(),
                    (*it)["replace_type"].get<int>(),
                    (*it)["amount"].get<double>(),
                    (*it)["creation_amount"].get<double>(),
                    (*it)["redemption_amount"].get<double>(),
                    
                    (*it)["creation_premium_ratio"].get<double>(),
                    (*it)["redemption_discount_ratio"].get<double>(),

                    (*it)["disclosure_vol"].get<int>(),
                    0
                });
            }
            //std::cout << "[parseRedisDisclosureInfoData] m_fundId: " << _fund->m_fundId << "m_stockList size: " << _fund->m_stockList.size() << std::endl;
            
            m_fundInfos[_fund->m_fundId] = _fund;
        }
    }
};

void IaETFFundInfo::parseRedisRealityInfoData(std::string reality_key)
{
    if(!m_redis)
    {
        return;
    }
    for (auto iter = m_fundInfos.begin(); iter != m_fundInfos.end(); iter++)
    {
        std::string symbol = iter->first;
        std::string result;
        std::shared_ptr<FundInfo> _fund = iter->second;
        m_redis->hget(reality_key.c_str(), symbol.c_str(), result);
        /* code */
        if (!result.empty())
        {
            json j = json::parse(result);

            for (json::iterator it = j["etf_list"].begin(); it != j["etf_list"].end(); ++it) {
                bool has = false;
                for (size_t i = 0; i < _fund->m_stockList.size(); i++)
                {
                    if (_fund->m_stockList[i].m_symbol == (*it)["symbol"].get<std::string>())
                    {
                        has = true;
                        _fund->m_stockList[i].m_reality_vol = (*it)["reality_vol"].get<double>();
                    }
                }
                //con
                if (!has)
                {
                    if (strcmp((*it)["exchange"].type_name(),"number"))   //补充披露清单中没有的股票
                    {   
                        _fund->m_stockList.push_back({
                            (*it)["symbol"].get<std::string>(),
                            (*it)["exchange"].get<std::string>(),
                            (*it)["name"].get<std::string>(),
                            1,
                            0,
                            0,
                            0,

                            0,
                            0,

                            0,
                            (*it)["reality_vol"].get<double>()
                        });
                    }else{  // 期货替代
                        string symbol;
                        string exchange("SH");
                        string name;
                        string future_symbol = (*it)["symbol"].get<std::string>();
                        if (future_symbol.substr(0,2) == "IF")
                        {
                            symbol = "000300";
                            name = "沪深300";
                        }else if (future_symbol.substr(0,2) == "IH")
                        {
                            symbol = "000016";
                            name = "上证50";
                        }else if (future_symbol.substr(0,2) == "IC")
                        {
                            symbol = "000905";
                            name = "中证500";
                        }else if (future_symbol.substr(0,2) == "IM")
                        {
                            symbol = "000852";
                            name = "中证1000";
                        }

                        has = false;
                        for (size_t i = 0; i < _fund->m_stockList.size(); i++)
                        {
                            if (_fund->m_stockList[i].m_symbol == symbol && _fund->m_stockList[i].m_exchange == exchange)
                            {
                                has = true;
                                _fund->m_stockList[i].m_reality_vol += (*it)["reality_vol"].get<double>();
                            }
                        }
                        if(!has)
                        {
                            _fund->m_stockList.push_back({
                                symbol,
                                exchange,
                                name,
                                1,
                                0,
                                0,
                                0,

                                0,
                                0,

                                0,
                                (*it)["reality_vol"].get<double>()
                            });
                        }
                    };
                }
            }
            //std::cout << "[parseRedisRealityInfoData] m_fundId: " << _fund->m_fundId << "m_stockList size: " << j["etf_list"].size() << " " << _fund->m_stockList.size() << std::endl;
        }
    }
};

std::map<std::string, std::shared_ptr<IaETFFundInfo::FundInfo>>* IaETFFundInfo::GetFundMap()
{
    return &m_fundInfos;
};

void IaETFFundInfo::GetSymbolList(const char* exchange, std::set<std::string> &out)
{
    for (auto map_iter = m_fundInfos.begin(); map_iter != m_fundInfos.end(); ++map_iter)
    {
        for (auto iter = map_iter->second->m_stockList.begin(); iter != map_iter->second->m_stockList.end(); ++iter)
        {
            if(iter->m_exchange == exchange && iter->m_reality_vol){
                out.insert(iter->m_symbol);
            }
        }
    }    
};