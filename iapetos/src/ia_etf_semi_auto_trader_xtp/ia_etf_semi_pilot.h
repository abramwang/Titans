#ifndef __IA_ETF_SEMI_PILOT_H__
#define __IA_ETF_SEMI_PILOT_H__
#include "ia_etf_factor_center.h"
#include "ia_etf_semi_trader_xtp.h"
#include "ia_command_interface.h"
#include "ia_etf_purchasing_worker.h"       //申购套利任务
#include "ia_etf_redeeming_worker.h"        //赎回套利任务
#include "ia_etf_trading_signal_filter.h"   //交易信号过滤器
#include <atomic>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>
using namespace nlohmann;


using namespace std;

class IaETFSemiPilot
    : public IaETFFactorCenter, public IaCommandInterface
{
private:
    bool m_trade_switch;
    int32_t m_trade_num;
    int64_t m_init_time;
    
public:
    void OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data);
    void OnCommandRtn(const char* type, const char* command);
private:
    std::unordered_map<std::string, IaETFFactorOperator*> m_factors_map;
    std::unordered_map<std::string, int64_t> m_signal_time_map; //symbol, last signal time

    void send_signal_to_redis(const json& j);
public:
    IaETFSemiTraderXtp* m_commander;
public:
	IaETFSemiPilot(int thread_num, uv_loop_t *loop, std::string configPath);
	virtual ~IaETFSemiPilot(){};

};


#endif