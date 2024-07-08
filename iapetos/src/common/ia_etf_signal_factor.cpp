#include "ia_etf_signal_factor.h"
#include "datetime.h"
#include <iostream>
#include <armadillo>
#include <cmath>
#include <limits>

#include "ti_quote_formater.h"

#define __TEST__ 0

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
#if __TEST__
    if (strcmp(pData->symbol, "510300") != 0)
    {
        return;
    }
#endif
    /*
    printf("[IaEtfSignalFactor::OnL2StockSnapshotRtn] %s, %s, %d, %s, %f, %ld, %f\n", 
            pData->symbol, pData->exchange, pData->time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
    */
    m_out["symbol"] = m_etf_info_ptr->m_fundId;
    m_out["name"] = m_etf_info_ptr->m_fundName;
    m_out["company"] = m_etf_info_ptr->m_company;
    m_out["update_time"] = datetime::get_format_time_ms(pData->date, pData->time);
    m_out["last"] = get_last_price(pData);
    m_out["iopv"] = pData->iopv;    //行情的iopv
    m_info = {0};
    m_info.diff = calc_diff();
    m_out["diff"] = m_info.diff;
    calc_iopv(pData, m_info);
    format_json_profit(m_info);
    calc_corr();
    format_influx_factor(pData, m_info);
    m_out["c_iopv"] = m_info.creation_iopv;
    m_out["r_iopv"] = m_info.redemption_iopv;
};

void IaEtfSignalFactor::OnTimer()
{
    return;
};

double IaEtfSignalFactor::get_last_price(const TiQuoteSnapshotStockField* pData)
{
    if(pData->last){
        return pData->last;
    }
    if (pData->bid_price[0])
    {
        return pData->bid_price[0];
    }
    if (pData->ask_price[0])
    {
        return pData->ask_price[0];
    }
    return pData->pre_close;
};

double IaEtfSignalFactor::get_bid_price(const TiQuoteSnapshotStockField* pData)
{
    if (pData->bid_price[0])
    {
        return pData->bid_price[0];
    }
    if (pData->last)
    {
        return pData->last;
    }
    if (pData->ask_price[0])
    {
        return pData->ask_price[0];
    }
    return pData->pre_close;
};

double IaEtfSignalFactor::get_ask_price(const TiQuoteSnapshotStockField* pData)
{
    if (pData->ask_price[0])
    {
        return pData->ask_price[0];
    }
    if (pData->last)
    {
        return pData->last;
    }
    if (pData->bid_price[0])
    {
        return pData->bid_price[0];
    }
    return pData->pre_close;
};

TiQuoteSnapshotIndexField* IaEtfSignalFactor::get_future_replace_price(std::string future_symbol)
{
    std::string symbol;
    std::string exchange("SZ");
    std::string name;
    if (future_symbol.substr(0,2) == "IF")
    {
        symbol = "399300";
        name = "沪深300";
    }else if (future_symbol.substr(0,2) == "IH")
    {
        symbol = "399016";
        name = "上证50";
    }else if (future_symbol.substr(0,2) == "IC")
    {
        symbol = "399905";
        name = "中证500";
    }else if (future_symbol.substr(0,2) == "IM")
    {
        symbol = "399852";
        name = "中证1000";
    }

    return m_quote_data_cache->GetIndexSnapshot(symbol.c_str(), exchange.c_str());
};

double IaEtfSignalFactor::calc_corr()
{
    TiMinBarPtr sh_index_min_bar = NULL;
    TiMinBarPtr fund_min_bar = NULL;
    
    if(!m_quote_data_cache->getBarCache()->getMinChangePercentageBar(
        "000001", "SH", TI_BarCycType_1m, sh_index_min_bar))
    {
        return 0.0;
    };

    if(!m_quote_data_cache->getBarCache()->getMinChangePercentageBar(
        m_etf_info_ptr->m_fundId.c_str(), m_etf_info_ptr->m_exchange.c_str(), TI_BarCycType_1m, fund_min_bar))
    {
        return 0.0;
    };


    std::vector<double> sh_index_close_vec, fund_close_vec;

    if(!sh_index_min_bar->getCloseSeries(sh_index_close_vec)){
        return 0.0;
    }
    if(!fund_min_bar->getCloseSeries(fund_close_vec)){
        return 0.0;
    }

    size_t len = sh_index_close_vec.size() < fund_close_vec.size() ? sh_index_close_vec.size() : fund_close_vec.size();

    if (len < 1)
    {
        return 0.0;
    }

    while (sh_index_close_vec.size() > len)
    {
        sh_index_close_vec.erase(sh_index_close_vec.begin());
    }

    while (fund_close_vec.size() > len)
    {
        fund_close_vec.erase(fund_close_vec.begin());
    }


    arma::vec sh_index_close_series = arma::vec(sh_index_close_vec);
    arma::vec fund_close_series = arma::vec(fund_close_vec);


    arma::mat  correlation_matrix = arma::cor(sh_index_close_series, fund_close_series);
    double correlation  = correlation_matrix(0,  0);
    
    /*
    std::cout << "[calc_corr] " << len 
        << ", " << sh_index_close_vec.size() << " " << *sh_index_close_vec.begin() << " " << *sh_index_close_vec.rbegin() 
        << " " << fund_close_vec.size() << " " << *fund_close_vec.begin() << " " << *fund_close_vec.rbegin() << ", "
        << " " << sh_index_close_series.size() << " " << sh_index_close_series[0] << " " << sh_index_close_series[len -1]
        << " " << fund_close_series.size() << " " << fund_close_series[0] << " " << fund_close_series[len - 1]
        << "," << correlation << std::endl;
    */
    if (!std::isnan(correlation) && !std::isinf(correlation))
    {
        m_info.corr = correlation;
    }
    ///*
    std::cout << "[calc_corr] " << len 
        << ", " << m_etf_info_ptr->m_fundId
        << "," << correlation
        << "," << m_info.corr << std::endl;
    //*/
    return 0.0;
};

double IaEtfSignalFactor::calc_diff()
{   
    int replace_num = 0;
    double total_diff = 0.0;
    for(auto &constituent_info : m_constituent_info_vec)
    {
        TiQuoteSnapshotStockField* snap_ptr = m_quote_data_cache->GetStockSnapshot(constituent_info->m_symbol.c_str(), constituent_info->m_exchange.c_str());
        TiQuoteSnapshotFutureField* future_snap_ptr = nullptr;
        TiQuoteSnapshotIndexField* index_snap_ptr = nullptr;
        if (constituent_info->m_exchange != "SH" && constituent_info->m_exchange != "SZ")
        {
            double last = 0;
            double pre_close = 0;

            future_snap_ptr = m_quote_data_cache->GetFutureSnapshot(constituent_info->m_symbol.c_str(), "CF");
            if (future_snap_ptr)
            {
                last = future_snap_ptr->last;
                pre_close = future_snap_ptr->pre_close;
            }
            
            if (!last)
            {
                index_snap_ptr = get_future_replace_price(constituent_info->m_symbol);
                //printf("[calc_diff:index_snap_ptr] %s\n", constituent_info->m_symbol.c_str());
                if (index_snap_ptr)
                {
                    last = index_snap_ptr->last;
                    pre_close = index_snap_ptr->pre_close;
                }
            }

            double diff = constituent_info->m_reality_vol ? (constituent_info->m_reality_vol - constituent_info->m_disclosure_vol) * (last - pre_close) * 1 : 0;
            
            total_diff += diff;
        }
        
        if(snap_ptr)
        {
            double last = get_last_price(snap_ptr);
            double diff = constituent_info->m_reality_vol ? (constituent_info->m_reality_vol - constituent_info->m_disclosure_vol) * (last - snap_ptr->pre_close) : 0;
            
            bool is_replace = false;
            double replace_amount = constituent_info->m_replace_amount;
            if ((constituent_info->m_replace_flag == IA_ERT_CASH_MUST) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_SZ) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_OTHER) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_HK))
            {
                is_replace = true;
                replace_num++;
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
                    if (!constituent_info->m_reality_vol)
                    {
                        diff = replace_amount * (last - snap_ptr->pre_close)/snap_ptr->pre_close;
                    }
                }
            }
#if __TEST__
/*
            printf("[calc_diff constituent_info] %s, %s, %f, %f, %f, %f, %d, %d, %f\n", 
                constituent_info->m_symbol.c_str(), constituent_info->m_exchange.c_str(), diff,
                last, snap_ptr->pre_close, constituent_info->m_reality_vol, constituent_info->m_disclosure_vol, 
                is_replace, replace_amount);
*/
#endif
            total_diff += diff;
        }
    }
#if __TEST__
    //std::cout << "=============== " << replace_num << " ===============" << std::endl;
#endif
    return total_diff;
};

void IaEtfSignalFactor::calc_iopv(const TiQuoteSnapshotStockField* pEtfSnap, profit_info &info)
{
    for(auto &constituent_info : m_constituent_info_vec)
    {
        TiQuoteSnapshotStockField* snap_ptr = m_quote_data_cache->GetStockSnapshot(constituent_info->m_symbol.c_str(), constituent_info->m_exchange.c_str());
        if(snap_ptr)
        {
            double bid_price = get_bid_price(snap_ptr);
            double ask_price = get_ask_price(snap_ptr);
            double last_price = get_last_price(snap_ptr);

            double c_iopv = 0.0;
            double r_iopv = 0.0;
            double iopv = 0.0;

            if ((constituent_info->m_replace_flag == IA_ERT_CASH_MUST) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_SZ) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_OTHER) ||
                (constituent_info->m_replace_flag == IA_ERT_CASH_MUST_INTER_HK))
            {
                c_iopv = constituent_info->m_creation_amount ? constituent_info->m_creation_amount : constituent_info->m_disclosure_vol * ask_price;
                r_iopv = constituent_info->m_redemption_amount ? constituent_info->m_redemption_amount : constituent_info->m_disclosure_vol * bid_price;
                iopv = constituent_info->m_redemption_amount ? constituent_info->m_redemption_amount : constituent_info->m_disclosure_vol * last_price;

                info.buy_stock_replace_amount += c_iopv;
                info.sell_stock_replace_amount += r_iopv;

                info.buy_stock_replace_margin += c_iopv * constituent_info->m_cash_replaced_creation_premium_rate;
                info.sell_stock_replace_margin += r_iopv * constituent_info->m_cash_replaced_redemption_discount_rate;
            }else{
                c_iopv = ask_price * constituent_info->m_disclosure_vol;
                r_iopv = bid_price * constituent_info->m_disclosure_vol;
                iopv = last_price * constituent_info->m_disclosure_vol;

                if (m_etf_info_ptr->m_exchange == constituent_info->m_exchange) //本市场自己买
                {
                    info.buy_stock_amount_local += c_iopv;
                    info.sell_stock_amount_local += r_iopv;

                    info.buy_stock_fee_local += c_iopv * 0.00016;
                    info.sell_stock_fee_local += r_iopv * 0.00016 + r_iopv * 0.0005;
                }else{                                                          //跨市场公募代买卖
                    info.buy_stock_amount_cross += c_iopv;
                    info.sell_stock_amount_cross += r_iopv;

                    info.buy_stock_fee_cross += c_iopv * 0.0008;
                    info.sell_stock_fee_cross += r_iopv * 0.0008 + r_iopv * 0.0005;
                }
            }

            info.creation_iopv += c_iopv;
            info.redemption_iopv += r_iopv;
            info.iopv += iopv;
        }
    }

    //申购
    info.buy_stock_amount = info.buy_stock_amount_local + info.buy_stock_amount_cross;
    info.buy_stock_fee = info.buy_stock_fee_local + info.buy_stock_fee_cross;

    info.sell_etf_amount = get_bid_price(pEtfSnap) * m_etf_info_ptr->m_minUnit;
    info.sell_etf_fee = info.sell_etf_amount * 0.00008;

    info.creation_turnover = info.sell_etf_amount + info.buy_stock_amount_local;

    //赎回
    info.sell_stock_amount = info.sell_stock_amount_local + info.sell_stock_amount_cross;
    info.sell_stock_fee = info.sell_stock_fee_local + info.sell_stock_fee_cross;

    info.buy_etf_amount = get_ask_price(pEtfSnap) * m_etf_info_ptr->m_minUnit;
    info.buy_etf_fee = info.buy_etf_amount * 0.00008;

    info.redemption_turnover = info.buy_etf_amount + info.sell_stock_amount_local;

    //creation_iopv = creation_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference;
    //redemption_iopv = redemption_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference;
    info.creation_iopv = (info.creation_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference) / m_etf_info_ptr->m_minUnit;
    info.redemption_iopv = (info.redemption_iopv + m_etf_info_ptr->m_publicEstimatedCashDifference) / m_etf_info_ptr->m_minUnit;
    info.iopv = (info.iopv + m_etf_info_ptr->m_publicEstimatedCashDifference) / m_etf_info_ptr->m_minUnit;

    info.creation_profit = info.sell_etf_amount - info.creation_iopv *  m_etf_info_ptr->m_minUnit + info.diff - info.buy_stock_fee - info.sell_etf_fee; 
    info.redemption_profit = info.redemption_iopv *  m_etf_info_ptr->m_minUnit - info.buy_etf_amount + info.diff - info.sell_stock_fee - info.buy_etf_fee;
};

void IaEtfSignalFactor::format_json_profit(profit_info &info)
{
    m_out["profit"] = json::object();
    m_out["profit"]["creation"] = json::object();
    m_out["profit"]["creation"]["buy_stock_amount"] = info.buy_stock_amount;
    m_out["profit"]["creation"]["buy_stock_amount_local"] = info.buy_stock_amount_local;
    m_out["profit"]["creation"]["buy_stock_amount_cross"] = info.buy_stock_amount_cross;
    m_out["profit"]["creation"]["buy_stock_fee"] = info.buy_stock_fee;
    m_out["profit"]["creation"]["buy_stock_fee_local"] = info.buy_stock_fee_local;
    m_out["profit"]["creation"]["buy_stock_fee_cross"] = info.buy_stock_fee_cross;
    m_out["profit"]["creation"]["buy_stock_replace_amount"] = info.buy_stock_replace_amount;
    m_out["profit"]["creation"]["buy_stock_replace_margin"] = info.buy_stock_replace_margin;
    m_out["profit"]["creation"]["sell_etf_amount"] = info.sell_etf_amount;
    m_out["profit"]["creation"]["sell_etf_fee"] = info.sell_etf_fee;
    m_out["profit"]["creation_turnover"] = info.creation_turnover;
    m_out["profit"]["creation_profit"] = info.creation_profit;


    m_out["profit"]["redemption"] = json::object();
    m_out["profit"]["redemption"]["sell_stock_amount"] = info.sell_stock_amount;
    m_out["profit"]["redemption"]["sell_stock_amount_local"] = info.sell_stock_amount_local;
    m_out["profit"]["redemption"]["sell_stock_amount_cross"] = info.sell_stock_amount_cross;
    m_out["profit"]["redemption"]["sell_stock_fee"] = info.sell_stock_fee;
    m_out["profit"]["redemption"]["sell_stock_fee_local"] = info.sell_stock_fee_local;
    m_out["profit"]["redemption"]["sell_stock_fee_cross"] = info.sell_stock_fee_cross;
    m_out["profit"]["redemption"]["sell_stock_replace_amount"] = info.sell_stock_replace_amount;
    m_out["profit"]["redemption"]["sell_stock_replace_margin"] = info.sell_stock_replace_margin;
    m_out["profit"]["redemption"]["buy_etf_amount"] = info.buy_etf_amount;
    m_out["profit"]["redemption"]["buy_etf_fee"] = info.buy_etf_fee;
    m_out["profit"]["redemption_turnover"] = info.redemption_turnover;
    m_out["profit"]["redemption_profit"] = info.redemption_profit;

    m_out["profit"]["diff"] = info.diff;
    m_out["profit"]["iopv"] = info.iopv;
    m_out["profit"]["creation_iopv"] = info.creation_iopv;
    m_out["profit"]["redemption_iopv"] = info.redemption_iopv;
    
    m_out["profit"]["corr"] = info.corr;
};

void IaEtfSignalFactor::format_influx_factor(const TiQuoteSnapshotStockField* pEtfSnap, profit_info &info)
{
    m_out["influx"] = json::object();
    m_out["influx"]["tags"] = json::object();
    m_out["influx"]["tags"]["exchange"] = pEtfSnap->exchange;
    m_out["influx"]["tags"]["symbol"] = pEtfSnap->symbol;

    m_out["influx"]["fields"] = json::object();

    m_out["influx"]["fields"]["last"] = pEtfSnap->last;
    m_out["influx"]["fields"]["iopv"] = pEtfSnap->iopv;
    m_out["influx"]["fields"]["diff"] = info.diff;
    m_out["influx"]["fields"]["calc_iopv"] = info.iopv;
    m_out["influx"]["fields"]["creation_iopv"] = info.creation_iopv;
    m_out["influx"]["fields"]["redemption_iopv"] = info.redemption_iopv;

    m_out["influx"]["fields"]["last_multi"] =  pEtfSnap->last * m_etf_info_ptr->m_minUnit;
    m_out["influx"]["fields"]["iopv_multi"] = pEtfSnap->iopv * m_etf_info_ptr->m_minUnit;
    m_out["influx"]["fields"]["calc_iopv_multi"] = info.iopv * m_etf_info_ptr->m_minUnit;
    m_out["influx"]["fields"]["creation_iopv_multi"] = info.creation_iopv * m_etf_info_ptr->m_minUnit;
    m_out["influx"]["fields"]["redemption_iopv_multi"] = info.redemption_iopv * m_etf_info_ptr->m_minUnit;

    m_out["influx"]["fields"]["creation_profit"] = info.creation_profit;
    m_out["influx"]["fields"]["redemption_profit"] = info.redemption_profit;

    m_out["influx"]["fields"]["corr"] = info.corr;


    m_out["influx"]["timestamp"] = datetime::get_timestamp_ms(pEtfSnap->date, pEtfSnap->time);

    //std::cout <<  m_out["influx"] << std::endl;
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

std::shared_ptr<IaEtfInfo> IaEtfSignalFactor::GetEtfInfo()
{
    return m_etf_info_ptr;
};

std::vector<std::shared_ptr<IaEtfConstituentInfo>>* IaEtfSignalFactor::GetConstituentInfo()
{
    return &m_constituent_info_vec;
};