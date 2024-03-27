#include "ia_etf_signal_factor.h"

IaEtfSignalFactor::IaEtfSignalFactor(std::shared_ptr<IaEtfInfo> etf_info_ptr, std::vector<std::shared_ptr<IaEtfConstituentInfo>> constituent_info_vec, IaEtfQuoteDataCache* etf_quote_data_cache)
{
    m_etf_info_ptr = etf_info_ptr;
    m_constituent_info_vec = constituent_info_vec;
    m_quote_data_cache = etf_quote_data_cache;    
};

IaEtfSignalFactor::~IaEtfSignalFactor()
{
};

void IaEtfSignalFactor::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
            pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
};

void IaEtfSignalFactor::OnTimer()
{
    TiQuoteSnapshotStockField* pData = m_quote_data_cache->GetSnapshot(m_etf_info_ptr->m_fundId.c_str(), m_etf_info_ptr->m_exchange.c_str());
    if (pData)
    {
        printf("[OnTimer] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
    }
};

double IaEtfSignalFactor::calc_diff()
{
    return 0.0;
};