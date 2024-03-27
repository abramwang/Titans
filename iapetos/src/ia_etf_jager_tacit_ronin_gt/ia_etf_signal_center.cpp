#include "ia_etf_signal_center.h"

IaEtfSignalCenter::IaEtfSignalCenter(IaEtfUserSetting* etf_user_setting, IaEtfQuoteDataCache* quote_data_cache)
{
    m_etf_user_setting = etf_user_setting;
    m_quote_data_cache = quote_data_cache;

    init_etf_signal_factor();
}

IaEtfSignalCenter::~IaEtfSignalCenter()
{
}


void IaEtfSignalCenter::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    if (std::string(pData->symbol) == "000001" && std::string(pData->exchange) == "SZ")
    {
        printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
    }
};

void IaEtfSignalCenter::OnTimer()
{
    auto iter = m_etf_signal_factor_map.begin();
    for (; iter != m_etf_signal_factor_map.end(); ++iter)
    {
        iter->second->OnTimer();
    }
};

void IaEtfSignalCenter::init_etf_signal_factor()
{
    std::cout << "init_etf_signal_factor" << std::endl;
    std::vector<std::string> monitor_etf_symbol_vec;
    m_etf_user_setting->GetMonitorEtfSymbol(monitor_etf_symbol_vec);

    for (auto &fund_symbol : monitor_etf_symbol_vec)
    {
        std::shared_ptr<IaEtfInfo> etf_info_ptr;
        std::vector<std::shared_ptr<IaEtfConstituentInfo>> constituent_info_vec;
        m_etf_user_setting->GetEtfInfo(fund_symbol, etf_info_ptr, constituent_info_vec);

        std::shared_ptr<IaEtfSignalFactor> etf_signal_factor = std::make_shared<IaEtfSignalFactor>(etf_info_ptr, constituent_info_vec, m_quote_data_cache);
        //m_etf_signal_factor_map[fund_symbol] = etf_signal_factor;
        break;
    }
};