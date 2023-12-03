#include "ia_etf_semi_pilot.h"
#include "datetime.h"
#include <iostream>

IaETFSemiPilot::IaETFSemiPilot(int thread_num, uv_loop_t *loop, std::string configPath)
    : IaETFFactorCenter(thread_num, loop, configPath)
{
    m_output_factors_to_redis = false;
    m_commander = NULL;
    m_init_time = 0;
    m_trade_switch = false;
    m_trade_num = 0;
    //m_trade_switch = true;
}

void IaETFSemiPilot::OnFactorRtn(const char* symbol, const char* factor_name, const json* pFactor, void* data)
{
    IaETFSemiTraderXtp::Locker locker(&m_commander->m_mutex);
    if (!m_init_time)
    {
        m_init_time = datetime::get_now_timestamp_ms();
        int64_t time_num = datetime::get_time_num();
        if (time_num  > 95000000 && time_num < 155000000)   //交易时段不重置了
        {
            if(m_config)
            {
                m_redis.del(m_config->szUiCostKey.c_str());
            }
        }
    }else{
        if ((datetime::get_now_timestamp_ms() - m_init_time) < 1000 * 60 * 3) //刚启动的5分钟内不交易
        {
            return;
        }
    }
    
    if (strcmp(factor_name, "etf_factor"))
    {
        return;
    }

    bool ignore = false;
    if (!strcmp(symbol, "159681"))
    {
        ignore = true;
    }
    
    IaETFFactorOperator* pOperator = (IaETFFactorOperator*)data;
    json j;
    if (IaETFTradingSignalFilter::isPurchasingSignal(pOperator, j))
    {
        int64_t pre_signal_time = 0;
        auto iter = m_signal_time_map.find(symbol);
        if (iter != m_signal_time_map.end())
        {
            pre_signal_time = m_signal_time_map[symbol];
        }
        int64_t cur_signal_time = j["time"];
        if ((cur_signal_time - pre_signal_time) / 100000 >= 3) 
        {
            m_signal_time_map[symbol] = cur_signal_time;
            printf("purchasing: %s %s %s %f\n", symbol, factor_name, pFactor->dump().c_str(), pOperator->nPurchasingCost.load());
            send_signal_to_redis(j);

            if (!m_trade_switch && !ignore)
            {
                std::cout << j["total_occupation_fund"] << ", " << m_commander->m_cash_asset << "\n";
                if (j["total_occupation_fund"] < m_commander->m_cash_asset * 0.9)
                {
                    IaETFPurchasingWorker* worker = new IaETFPurchasingWorker(datetime::get_time_num(), pOperator, m_commander);
                    m_commander->AddWorker(std::shared_ptr<IaETFTradingWorker>(worker));
                    m_trade_num += 1;
                    if (m_trade_num >= 3)
                    {
                        m_trade_switch = true;
                    }
                }
            }
        }
    };
    
    j.clear();
    if (IaETFTradingSignalFilter::isRedeemingSignal(pOperator, j))
    {
        int64_t pre_signal_time = 0;
        auto iter = m_signal_time_map.find(symbol);
        if (iter != m_signal_time_map.end())
        {
            pre_signal_time = m_signal_time_map[symbol];
        }
        int64_t cur_signal_time = j["time"];
        //printf("[isRedeemingSignal] %s cur_signal_time: %ld, pre_signal_time: %ld, diff: %ld\n", symbol, cur_signal_time, pre_signal_time, (cur_signal_time - pre_signal_time) / 100000);
        if ((cur_signal_time - pre_signal_time) / 100000 >= 3) 
        {
            m_signal_time_map[symbol] = cur_signal_time;
            printf("redeeming: %s %s %s %f\n", symbol, factor_name, pFactor->dump().c_str(), pOperator->nRedeemingCost.load());
            send_signal_to_redis(j);

            if (!m_trade_switch && !ignore)
            {
                std::cout << j["total_occupation_fund"] << ", " << m_commander->m_cash_asset << "\n";
                if (j["total_occupation_fund"] < m_commander->m_cash_asset * 0.9)
                {
                    IaETFRedeemingWorker* worker = new IaETFRedeemingWorker(datetime::get_time_num(), pOperator, m_commander);
                    m_commander->AddWorker(std::shared_ptr<IaETFTradingWorker>(worker));
                    m_trade_num += 1;
                    if (m_trade_num >= 3)
                    {
                        m_trade_switch = true;
                    }
                }
            }
        }
        
        //int64_t symbol_id = TiQuoteTools::GetSymbolId(symbol);

        //std::cout << "purchasing: " << symbol << " " << factor_name << " " << pFactor->dump() << std::endl;
    };
    
    //std::cout << "IaETFSemiPilot::OnFactorRtn: " << symbol << " " << factor_name << " " << pFactor->dump() << std::endl;
    auto iter = m_factors_map.find(symbol);
    if (iter == m_factors_map.end())
    {
        m_factors_map[symbol] = (IaETFFactorOperator*)data;
    }
};

void IaETFSemiPilot::OnCommandRtn(const char* type, const char* command)
{
    std::cout << "OnCommandRtn: " << type << " " << command << std::endl;
    
    if(!strcmp(type, "purchasing")){
        auto j = json::parse(command);
        auto iter = m_factors_map.find(j["etf_symbol"]);
        if (iter != m_factors_map.end() && m_commander)
        {
            std::cout << "creat worker: " << j["etf_symbol"] << std::endl;
            IaETFPurchasingWorker* worker = new IaETFPurchasingWorker(datetime::get_time_num(), iter->second, m_commander);
            m_commander->AddWorker(std::shared_ptr<IaETFTradingWorker>(worker));
        }
    }

    if(!strcmp(type, "redeeming")){
        auto j = json::parse(command);
        auto iter = m_factors_map.find(j["etf_symbol"]);
        if (iter != m_factors_map.end() && m_commander)
        {
            std::cout << "creat worker: " << j["etf_symbol"] << std::endl;
            IaETFRedeemingWorker* worker = new IaETFRedeemingWorker(datetime::get_time_num(), iter->second, m_commander);
            m_commander->AddWorker(std::shared_ptr<IaETFTradingWorker>(worker));
        }
    }
};


void IaETFSemiPilot::send_signal_to_redis(const json& j)
{
    bool flag = m_redis.xadd(m_config->szUiCostKey.c_str(), j.dump().c_str());
            
    if (!flag){
        std::cout << "try reconnect" << std::endl;
        flag = m_redis.disconnect();
        if (flag)
        {
            flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
            flag = m_redis.xadd(m_config->szUiCostKey.c_str(), j.dump().c_str());
        }
    }
};