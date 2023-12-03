#include "ti_quote_formater.h"
#include "datetime.h"

void TiQuoteFormater::FormatSnapshot(const TiQuoteSnapshotFutureField* pData, json& j)
{
    j = {
        {"exchange" , pData->exchange},
        {"symbol" , pData->symbol},
        {"date" , pData->date},
        {"time" , pData->time},
        {"datetime" , datetime::get_format_timestamp_ms(pData->timestamp)},
        {"time_str" , pData->time_str},
        {"recv_time_str" , pData->recv_time_str},
        {"recv_timestamp" , pData->recv_timestamp},
        {"last" , pData->last},
        {"pre_settlement_close" , pData->pre_settlement_close},
        {"pre_open_interest" , pData->pre_open_interest},
        {"pre_close" , pData->pre_close},
        {"open" , pData->open},
        {"high" , pData->high},
        {"low" , pData->low},
        {"close" , pData->close},
        {"settlement" , pData->settlement},
        {"high_limit" , pData->high_limit},
        {"low_limit" , pData->low_limit},
        {"volume" , pData->volume},
        {"turnover" , pData->turnover},
        {"acc_volume" , pData->acc_volume},
        {"acc_turnover" , pData->acc_turnover},
        {"open_interest" , pData->open_interest},
        {"pre_delta" , pData->pre_delta},
        {"curr_delta" , pData->curr_delta},
        {"ave_price" , pData->ave_price},
        {"ask_price" , json::array()},
        {"ask_volume" , json::array()},
        {"bid_price" , json::array()},
        {"bid_volume" , json::array()}
    };
    
    for (size_t i = 0; i < TI_FUTURE_ARRAY_LEN; i++)
    {
        j["ask_price"].push_back(pData->ask_price[i]);
        j["ask_volume"].push_back(pData->ask_volume[i]);
        j["bid_price"].push_back(pData->bid_price[i]);
        j["bid_volume"].push_back(pData->bid_volume[i]);
    }
};

void TiQuoteFormater::FormatSnapshot(const TiQuoteSnapshotStockField* pData, json& j)
{
    j = {
        {"exchange" , pData->exchange},
        {"symbol" , pData->symbol},
        {"date" , pData->date},
        {"time" , pData->time},
        {"datetime" , datetime::get_format_timestamp_ms(pData->timestamp)},     
        {"time_str" , pData->time_str},
        {"recv_time_str" , pData->recv_time_str},
        {"recv_timestamp" , pData->recv_timestamp},
        {"last", pData->last},
        {"pre_close", pData->pre_close},
        {"open", pData->open},
        {"high", pData->high},
        {"low", pData->low},
        {"high_limit", pData->high_limit},
        {"low_limit", pData->low_limit},
        {"volume", pData->volume},
        {"turnover", pData->turnover},
        {"acc_volume", pData->acc_volume},
        {"acc_turnover", pData->acc_turnover},
        {"match_items", pData->match_items},
        {"total_ask_qty", pData->total_ask_qty},
        {"total_bid_qty", pData->total_bid_qty},
        {"wavg_ask", pData->wavg_ask},
        {"wavg_bid", pData->wavg_bid},
        {"interest", pData->interest},
        {"ask_price", json::array()},
        {"ask_volume", json::array()},
        {"ask_order_num", json::array()},
        {"bid_price", json::array()},
        {"bid_volume", json::array()},
        {"bid_order_num",json::array()}
    };
    for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
    {
        j["ask_price"].push_back(pData->ask_price[i]);
        j["ask_volume"].push_back(pData->ask_volume[i]);
        j["ask_order_num"].push_back(pData->ask_order_num[i]);
        j["bid_price"].push_back(pData->bid_price[i]);
        j["bid_volume"].push_back(pData->bid_volume[i]);
        j["bid_order_num"].push_back(pData->bid_order_num[i]);
    }
};
void TiQuoteFormater::FormatMatch(const TiQuoteMatchesField* pData, json& j)
{
    j = {
        {"exchange" , pData->exchange},
        {"symbol" , pData->symbol},
        {"date" , pData->date},
        {"time" , pData->time},
        {"datetime" , datetime::get_format_timestamp_ms(pData->timestamp)},     
        {"time_str" , pData->time_str},
        {"recv_time_str" , pData->recv_time_str},
        {"recv_timestamp" , pData->recv_timestamp},
        {"channel" , pData->channel},
        {"seq" , pData->seq},
        {"price" , pData->price},
        {"volume" , pData->volume},
        {"bs_flag" , pData->bs_flag},
        {"function_code" , pData->function_code},
        {"ask_order_seq" , pData->ask_order_seq},
        {"bid_order_seq" , pData->bid_order_seq}
    };
};

void TiQuoteFormater::FormatOrder(const TiQuoteOrderField* pData, json& j)
{
    j = {
        {"exchange" , pData->exchange},
        {"symbol" , pData->symbol},
        {"date" , pData->date},
        {"time" , pData->time},
        {"datetime" , datetime::get_format_timestamp_ms(pData->timestamp)},     
        {"time_str" , pData->time_str},
        {"recv_time_str" , pData->recv_time_str},
        {"recv_timestamp" , pData->recv_timestamp},
        {"channel" , pData->channel},
        {"seq" , pData->seq},
        {"price" , pData->price},
        {"volume" , pData->volume},
        {"order_type" , pData->order_type},
        {"function_code" , pData->function_code}
    };
};