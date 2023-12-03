#include "ia_etf_factors_ipc_output.h"
#include "datetime.h"
#include "iniFile.h"
#include <iostream>

IaETFFactorOutput::IaETFFactorOutput(int thread_num, uv_loop_t *loop, std::string configPath)
    : IaETFFactorCenter(thread_num, loop, configPath)
{
    m_output_factors_to_redis = false;
    m_setting = nullptr;
    m_last_output_time = 0;
    m_output_j = {
        {"update_time", ""},
        {"data", json::object()}
    };
    loadSetting(configPath);
    loadSymbols();
    resetStreamKey();
}

void IaETFFactorOutput::OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data)
{
    if (strcmp(factor_name, "etf_factor"))
    {
        return;
    }
    auto iter = m_etf_symbols.find(symbol);
    if (iter == m_etf_symbols.end())
    {
        return;
    }
    int64_t now = datetime::get_now_timestamp_ms();
    IaETFFactorOperator* op = (IaETFFactorOperator*)data;
    m_output_j["update_time"] = datetime::get_format_now_time_ms();
    m_output_j["data"][symbol] = {
        { "symbol", op->m_etf_last_snap.symbol },
        { "ask_iopv", op->nAskIOPV.load() },
        { "bid_iopv", op->nBidIOPV.load() },
        { "diff", op->nDiff.load() }
    };
    if ((now - m_last_output_time) > 1000)
    {
        sendSignalToRedis(m_output_j);
        m_last_output_time = now;
    }
};

int IaETFFactorOutput::loadSetting(std::string iniFileName)
{
    if(m_setting){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_setting = new SettingInfo();

    m_setting->szListKey    = string(_iniFile["ia_etf_factors_ipc_output"]["list"]);
    m_setting->szOutputKey    = string(_iniFile["ia_etf_factors_ipc_output"]["output_key"]);
    
    if(m_setting->szListKey.empty() |
        m_setting->szOutputKey.empty())
    {
        delete m_setting;
        m_setting = NULL;
        return -1;
    }
    return 0;
};

void IaETFFactorOutput::resetStreamKey()
{
    if (!m_setting)
    {
        return;
    }
    int64_t time_num = datetime::get_time_num();
    if (time_num  > 95000000)   //9:50后重启程序就不用重置了
    {
        return;
    }
    if(!m_setting->szOutputKey.empty())
    {
        m_redis.del(m_setting->szOutputKey.c_str());
    }
};


int IaETFFactorOutput::loadSymbols()
{
    if(!m_setting){
        return -1;
    }
    if (m_setting->szListKey.empty())
    {
        return -1;
    }
    std::vector<std::string> etf_list;
    m_redis.smembers(m_setting->szListKey.c_str(), etf_list);
    for (size_t i = 0; i < etf_list.size(); i++)
    {
        m_etf_symbols.insert(etf_list[i]);
    }
    return 0;
};

void IaETFFactorOutput::sendSignalToRedis(const json& j)
{
    bool flag = m_redis.xadd(m_setting->szOutputKey.c_str(), j.dump().c_str());
            
    if (!flag){
        flag = m_redis.disconnect();
        if (flag)
        {
            flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
            flag = m_redis.xadd(m_setting->szOutputKey.c_str(), j.dump().c_str());
        }
    }
};