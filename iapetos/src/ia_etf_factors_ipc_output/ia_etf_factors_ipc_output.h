#ifndef __IA_ETF_FACTOR_OUTPUT_H__
#define __IA_ETF_FACTOR_OUTPUT_H__
#include "ia_etf_factor_center.h"
#include "ia_etf_factor_operator.h"

#include <atomic>
#include <unordered_map>

#include <nlohmann/json.hpp>
using namespace nlohmann;

using namespace std;

class IaETFFactorOutput
    : public IaETFFactorCenter
{
public:
    typedef struct SettingInfo
    {
        std::string szListKey;
        std::string szOutputKey;
    } SettingInfo;

public:
    void OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data);
private:
    SettingInfo* m_setting;
    std::unordered_set<std::string> m_etf_symbols;
    int64_t m_last_output_time;
    json m_output_j;

    int loadSetting(std::string iniFileName);
    void resetStreamKey();
    int loadSymbols();
    void sendSignalToRedis(const json& j);

public:
	IaETFFactorOutput(int thread_num, uv_loop_t *loop, std::string configPath);
	virtual ~IaETFFactorOutput(){};

};


#endif