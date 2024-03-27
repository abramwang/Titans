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
    /*
    printf("[OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
            pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
    */
    m_out["symbol"] = m_etf_info_ptr->m_fundId;
    m_out["company"] = m_etf_info_ptr->m_company;
    m_out["update_time"] = pData->time_str;
    m_out["diff"] = calc_diff();
};

void IaEtfSignalFactor::OnTimer()
{
    return;
    TiQuoteSnapshotStockField* pData = m_quote_data_cache->GetSnapshot(m_etf_info_ptr->m_fundId.c_str(), m_etf_info_ptr->m_exchange.c_str());
    if (pData)
    {
        printf("[OnTimer] %s, %s, %d, %s, %f, %ld, %f\n", 
                pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
        
    }
};

double IaEtfSignalFactor::calc_diff()
{
    double total_diff = 0.0;
    for(auto &constituent_info : m_constituent_info_vec)
    {
        TiQuoteSnapshotStockField* snap_ptr = m_quote_data_cache->GetSnapshot(constituent_info->m_symbol.c_str(), constituent_info->m_exchange.c_str());
        if(snap_ptr)
        {
            double last = snap_ptr->last;
            if(!last){
                last = snap_ptr->bid_price[0];
            }
            if(!last){
                last = snap_ptr->ask_price[0];
            }
            if (!last)
            {
                last = snap_ptr->pre_close;
            }
            double diff = constituent_info->m_reality_vol ? (constituent_info->m_reality_vol - constituent_info->m_disclosure_vol) * (snap_ptr->last - snap_ptr->pre_close) : 0;
            //printf("[calc_diff] %s, %s, %f\n", snap_ptr->symbol, snap_ptr->exchange, diff);
            total_diff += diff;
        }
    }
    //printf("[calc_diff] %s total_diff: %f\n", m_etf_info_ptr->m_fundId.c_str(), total_diff);
    return total_diff;
};

void IaEtfSignalFactor::GetJsonOut(json& j)
{
    j = m_out;
};