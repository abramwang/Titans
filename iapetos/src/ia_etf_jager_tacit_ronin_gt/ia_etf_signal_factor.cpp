#include "ia_etf_signal_factor.h"
#include "datetime.h"
#include <iostream>

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
    m_out["name"] = m_etf_info_ptr->m_fundName;
    m_out["company"] = m_etf_info_ptr->m_company;
    m_out["update_time"] = datetime::get_format_time_ms(pData->date, pData->time);
    m_out["diff"] = calc_diff();
    double creation_iopv = 0.0;
    double redemption_iopv = 0.0;
    calc_iopv(creation_iopv, redemption_iopv);
    m_out["c_iopv"] = creation_iopv;
    m_out["r_iopv"] = redemption_iopv;
    m_out["last"] = pData->last;
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
            if ((constituent_info->m_replace_flag == IA_ERT_CASH_MUST) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_SZ) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_OTHER) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_HK))
            {
                double replace_amount = constituent_info->m_replace_amount;
                if (!replace_amount)
                {
                    replace_amount = constituent_info->m_creation_amount;
                }
                if (!replace_amount)
                {
                    replace_amount = constituent_info->m_redemption_amount;
                }
                
                if (replace_amount)
                {
                    diff = replace_amount * (snap_ptr->last - snap_ptr->pre_close)/snap_ptr->pre_close;
                }
            }
            total_diff += diff;
        }
    }
    //printf("[calc_diff] %s %s total_diff: %f\n", m_etf_info_ptr->m_fundId.c_str(), m_etf_info_ptr->m_fundName.c_str(), total_diff);
    return total_diff;
};

void IaEtfSignalFactor::calc_iopv(double& creation_iopv, double& redemption_iopv)
{
    creation_iopv = 0.0;
    redemption_iopv = 0.0;
    for(auto &constituent_info : m_constituent_info_vec)
    {
        TiQuoteSnapshotStockField* snap_ptr = m_quote_data_cache->GetSnapshot(constituent_info->m_symbol.c_str(), constituent_info->m_exchange.c_str());
        if(snap_ptr)
        {
            double bid_price = snap_ptr->bid_price[0];
            double ask_price = snap_ptr->ask_price[0];
            double last = snap_ptr->last;

            if(!bid_price){
                bid_price = last;
            }
            if (!bid_price)
            {
                bid_price = ask_price;
            }
            if (!bid_price)
            {
                bid_price = snap_ptr->pre_close;
            }

            if(!ask_price){
                ask_price = last;
            }
            if (!ask_price)
            {
                ask_price = bid_price;
            }
            if (!ask_price)
            {
                ask_price = snap_ptr->pre_close;
            }

            if(!last){
                last = bid_price;
            }
            if(!last){
                last = ask_price;
            }
            if (!last)
            {
                last = snap_ptr->pre_close;
            }


            double c_iopv = ask_price * constituent_info->m_disclosure_vol;
            double r_iopv = bid_price * constituent_info->m_disclosure_vol;

            if ((constituent_info->m_replace_flag == IA_ERT_CASH_MUST) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_SZ) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_OTHER) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_HK))
            {
                c_iopv = constituent_info->m_creation_amount;
                r_iopv = constituent_info->m_redemption_amount;
            }
            creation_iopv += c_iopv;
            redemption_iopv += r_iopv;
        }else{
            if ((constituent_info->m_replace_flag == IA_ERT_CASH_MUST) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_SZ) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_OTHER) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_HK))
            {
                //creation_iopv += constituent_info->m_creation_amount;
                //redemption_iopv += constituent_info->m_redemption_amount;
            }
        }
    }

    //creation_iopv = creation_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference;
    //redemption_iopv = redemption_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference;
    creation_iopv = (creation_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference) / m_etf_info_ptr->m_minUnit;
    redemption_iopv = (redemption_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference) / m_etf_info_ptr->m_minUnit;
};

bool IaEtfSignalFactor::GetJsonOut(json& j)
{
    if (m_out.empty())
    {
        return false;
    }
    if (m_out["diff"].get<double>() == 0)
    {
        return false;
    }
    j = m_out;
    return true;
};