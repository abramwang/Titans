#include <iostream>
#include <iniFile.h>
#include <thread>
#include <glog/logging.h>
#include "ti_encoding_tool.h"
#include "ti_df_quote_client.h"
#include "ti_quote_struct.h"
#include "datetime.h"

#include "ti_quote_formater.h"

namespace
{
static constexpr int kCacheSize = 2048;
}

TiDfQuoteClient::TiDfQuoteClient()
{

}

TiDfQuoteClient::~TiDfQuoteClient()
{
    quote_api_->Release();
}

//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////

void TiDfQuoteClient::formatQuoteUpdatetime(unsigned long long quote_update_time, int32_t &date, int32_t &time, int64_t &timestamp)
{
    date = quote_update_time / 1000000000;
    time = quote_update_time % 1000000000;
    timestamp = datetime::get_timestamp_ms(date, time);
};
//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////

// 深交所快照行情
void TiDfQuoteClient::OnLv2SnapSze(EMQSzeSnap *snap)
{
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));

    strcpy(m_snapStockCash.symbol, (char*)snap->m_head.m_symbol);
    strcpy(m_snapStockCash.exchange, "SZ");
    formatQuoteUpdatetime(snap->m_head.m_quote_update_time, 
        m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.timestamp);
    datetime::get_format_time_ms(m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp = datetime::get_now_timestamp_ms();

    m_snapStockCash.last            = snap->m_last_price/10000;
    m_snapStockCash.pre_close       = snap->m_pre_close_price/10000;
    m_snapStockCash.open            = snap->m_open_price/10000;
    m_snapStockCash.high            = snap->m_day_high_price/10000;
    m_snapStockCash.low             = snap->m_day_low_price/10000;
    m_snapStockCash.high_limit      = snap->m_upper_limit_price/10000;
    m_snapStockCash.low_limit       = snap->m_low_limit_price/10000;
    m_snapStockCash.acc_volume      = snap->m_total_quantity/100;
    m_snapStockCash.acc_turnover    = snap->m_total_value/1000000;
    m_snapStockCash.match_items     = snap->m_total_trade_num;
    m_snapStockCash.total_ask_qty   = snap->m_total_ask_quantity/100;
    m_snapStockCash.total_bid_qty   = snap->m_total_bid_quantity/100;
    m_snapStockCash.wavg_ask        = snap->m_total_ask_weighted_avg_price/10000;
    m_snapStockCash.wavg_bid        = snap->m_total_bid_weighted_avg_price/10000;
    m_snapStockCash.interest        = snap->m_open_interest/10000;
    m_snapStockCash.iopv            = snap->m_iopv/10000;

    for (size_t i = 0; i < 10; i++)
    {
        m_snapStockCash.ask_price[i] = snap->m_ask_unit[i].m_price/10000;
        m_snapStockCash.ask_volume[i] = snap->m_ask_unit[i].m_quantity/100;

        m_snapStockCash.bid_price[i] = snap->m_bid_unit[i].m_price/10000;
        m_snapStockCash.bid_volume[i] = snap->m_bid_unit[i].m_quantity/100;
    }
    

    printf("[OnL2StockSnapshotRtn] %s, %s, %d, %f, %ld, %f, %f, %f\n", 
                m_snapStockCash.symbol, m_snapStockCash.time_str, m_snapStockCash.time, 
                m_snapStockCash.last, m_snapStockCash.acc_volume, m_snapStockCash.acc_turnover, m_snapStockCash.high_limit, m_snapStockCash.low_limit);

    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }
}

// 深交所逐笔合并行情
void TiDfQuoteClient::OnLv2TickSze(EMQSzeTick *tick)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(tick);
    char cache[kCacheSize] = {0};
    if (tick->m_tick_type == EMQSzeTickType::kTickTypeOrder)
    {
        memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));
        strcpy(m_orderCash.symbol, (char*)tick->m_tick_order->m_head.m_symbol);
        strcpy(m_orderCash.exchange, "SZ");
        formatQuoteUpdatetime(tick->m_tick_order->m_head.m_quote_update_time, 
            m_orderCash.date, m_orderCash.time, m_orderCash.timestamp);
        datetime::get_format_time_ms(m_orderCash.date, m_orderCash.time, m_orderCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_orderCash.recv_time_str, TI_TIME_STR_LEN);
        m_orderCash.recv_timestamp = datetime::get_now_timestamp_ms();

        m_orderCash.channel         = tick->m_tick_order->m_head.m_channel_num;
        m_orderCash.seq             = tick->m_tick_order->m_head.m_sequence;
        m_orderCash.price           = tick->m_tick_order->m_order_price/10000;
        m_orderCash.volume          = tick->m_tick_order->m_order_quantity/100;
        m_orderCash.order_orino     = tick->m_tick_order->m_head.m_sequence;

        if(tick->m_tick_order->m_side_flag == '1'){
            m_orderCash.function_code = 'B'; 
        }
        if(tick->m_tick_order->m_side_flag == '2'){
            m_orderCash.function_code = 'S'; 
        }

        if (tick->m_tick_order->m_order_type == '2') //限价单
        {
            m_orderCash.order_type      = '0';
        }else{
            m_orderCash.order_type      = tick->m_tick_order->m_order_type;
        }

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }

        json j;
        TiQuoteFormater::FormatOrder(&m_orderCash, j);
        std::cout << "FormatOrder: "  << j.dump() << std::endl;

    }
    else if (tick->m_tick_type == EMQSzeTickType::kTickTypeExe)
    {
        memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));
        strcpy(m_matchCash.symbol, (char*)tick->m_tick_exe->m_head.m_symbol);
        strcpy(m_matchCash.exchange, "SZ");
        formatQuoteUpdatetime(tick->m_tick_exe->m_head.m_quote_update_time, 
            m_matchCash.date, m_matchCash.time, m_matchCash.timestamp);
        datetime::get_format_time_ms(m_matchCash.date, m_matchCash.time, m_matchCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_matchCash.recv_time_str, TI_TIME_STR_LEN);
        m_matchCash.recv_timestamp = datetime::get_now_timestamp_ms();

        m_matchCash.channel         = tick->m_tick_exe->m_head.m_channel_num;
        m_matchCash.seq             = tick->m_tick_exe->m_head.m_sequence;
        m_matchCash.price           = tick->m_tick_exe->m_trade_price/10000;
        m_matchCash.volume          = tick->m_tick_exe->m_trade_quantity/100;
        m_matchCash.ask_order_seq   = tick->m_tick_exe->m_trade_sell_num;
        m_matchCash.bid_order_seq   = tick->m_tick_exe->m_trade_buy_num;

        if(tick->m_tick_exe->m_trade_type == 'F')
        {
            m_matchCash.function_code = '0';
        }else{
            m_matchCash.function_code = 'C';
        }

        if (m_matchCash.ask_order_seq > m_matchCash.bid_order_seq)
        {
            m_matchCash.bs_flag = 'S';
        }else{
            m_matchCash.bs_flag = 'B';
        }
        
        if(m_cb){
            m_cb->OnL2StockMatchesRtn(&m_matchCash);
        }

        json j;
        TiQuoteFormater::FormatMatch(&m_matchCash, j);
        std::cout << "FormatMatch: " << j.dump() << std::endl;
    }
}

// 深交所指数行情
void TiDfQuoteClient::OnLv2IndexSze(EMQSzeIdx *idx)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(idx);

    char cache[1024];
    sprintf(cache, "%d,%d,%s,%llu,%d,%llu,%llu,%llu,%u,%u,%u,%u,%u,%u,%u,%d,%d,%llu,%d,%lu\n",
            idx->m_head.m_security_type, idx->m_head.m_sub_security_type, idx->m_head.m_symbol,
            idx->m_head.m_quote_update_time, idx->m_head.m_channel_num, idx->m_total_trade_num, idx->m_total_quantity,
            idx->m_total_value, idx->m_last_price, idx->m_pre_close_price, idx->m_open_price, idx->m_day_high_price,
            idx->m_day_low_price, idx->m_today_close_price, idx->m_head.m_sequence, idx->m_head.m_message_type,
            idx->m_head.m_exchange_id, idx->m_head.m_sequence_num, idx->m_head.m_md_stream_id, ts);

    std::cout << cache << std::flush;
}

// 深交所债券快照行情
void TiDfQuoteClient::OnLv2BondSnapSze(EMQSzeBondSnap *bond_snap)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(bond_snap);

    char cache[kCacheSize];
    sprintf(cache,
            "%d,%d,%s,%llu,%d,%d,%llu,%llu,%llu,%u,%u,%u,%u,%u,%u,%u,%d,%d,%u,%llu,%u,%llu,%u,%u,%d,%llu,%llu,%u,%llu,%u,%llu,%u,%llu,%u,%llu,%d,%d,%u,%llu,%u,%lu,%u,%llu\n",
            bond_snap->m_head.m_security_type, bond_snap->m_head.m_sub_security_type, bond_snap->m_head.m_symbol,
            bond_snap->m_head.m_quote_update_time, bond_snap->m_head.m_channel_num, bond_snap->m_trading_status,
            bond_snap->m_total_trade_num, bond_snap->m_total_quantity, bond_snap->m_total_value,
            bond_snap->m_pre_close_price, bond_snap->m_last_price, bond_snap->m_open_price, bond_snap->m_day_high_price,
            bond_snap->m_day_low_price, bond_snap->m_today_close_price, bond_snap->m_total_trade_weighted_avg_price,
            bond_snap->m_fluctuation_1, bond_snap->m_fluctuation_2, bond_snap->m_total_bid_weighted_avg_price,
            bond_snap->m_total_bid_quantity, bond_snap->m_total_ask_weighted_avg_price, bond_snap->m_total_ask_quantity,
            bond_snap->m_weighted_avg_price_BP, bond_snap->m_weighted_avg_pre_close_price,
            bond_snap->m_auction_trade_last_price, bond_snap->m_auction_volume_trade, bond_snap->m_auction_value_trade,
            bond_snap->m_bid_unit[0].m_price, bond_snap->m_bid_unit[0].m_quantity, bond_snap->m_ask_unit[0].m_price,
            bond_snap->m_ask_unit[0].m_quantity, bond_snap->m_bid_unit[9].m_price, bond_snap->m_bid_unit[9].m_quantity,
            bond_snap->m_ask_unit[9].m_price, bond_snap->m_ask_unit[9].m_quantity, bond_snap->m_head.m_sequence,
            bond_snap->m_head.m_message_type, bond_snap->m_head.m_exchange_id, bond_snap->m_head.m_sequence_num,
            bond_snap->m_head.m_md_stream_id, ts);

    std::cout << cache << std::flush;
}

// 深交所债券逐笔合并行情
void TiDfQuoteClient::OnLv2BondTickSze(EMQSzeBondTick *bond_tick)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(bond_tick);
    char cache[kCacheSize];

    if (bond_tick->m_tick_type == EMQSzeTickType::kTickTypeBondOrder)
    {
        sprintf(cache, "%d,%d,%s,%llu,%d,%llu,%u,%llu,%d,%d,%u,%d,%u,%u,%lu\n",
                bond_tick->m_bond_order->m_head.m_security_type, bond_tick->m_bond_order->m_head.m_sub_security_type,
                bond_tick->m_bond_order->m_head.m_symbol, bond_tick->m_bond_order->m_head.m_quote_update_time,
                bond_tick->m_bond_order->m_head.m_channel_num,

                bond_tick->m_bond_order->m_head.m_sequence_num, bond_tick->m_bond_order->m_price,
                bond_tick->m_bond_order->m_quantity, bond_tick->m_bond_order->m_side,
                bond_tick->m_bond_order->m_order_type,

                bond_tick->m_bond_exe->m_head.m_sequence, bond_tick->m_bond_exe->m_head.m_message_type,
                bond_tick->m_bond_exe->m_head.m_exchange_id, bond_tick->m_bond_exe->m_head.m_md_stream_id, ts);

        std::cout << cache << std::flush;
    }
    else if (bond_tick->m_tick_type == EMQSzeTickType::kTickTypeBondExe)
    {
        sprintf(cache, "%d,%d,%s,%llu,%d,%llu,%llu,%lld,%u,%lld,%u,%d,%u,%u,%u,%lu\n",
                bond_tick->m_bond_exe->m_head.m_security_type, bond_tick->m_bond_exe->m_head.m_sub_security_type,
                bond_tick->m_bond_exe->m_head.m_symbol, bond_tick->m_bond_exe->m_head.m_quote_update_time,
                bond_tick->m_bond_exe->m_head.m_channel_num, bond_tick->m_bond_exe->m_head.m_sequence_num,
                bond_tick->m_bond_exe->m_buy_num, bond_tick->m_bond_exe->m_sell_num, bond_tick->m_bond_exe->m_price,
                bond_tick->m_bond_exe->m_quantity, bond_tick->m_bond_exe->m_type,
                bond_tick->m_bond_exe->m_head.m_sequence, bond_tick->m_bond_exe->m_head.m_message_type,
                bond_tick->m_bond_exe->m_head.m_exchange_id, bond_tick->m_bond_exe->m_head.m_md_stream_id, ts);

        std::cout << cache << std::flush;
    }
}

// 深交所建树行情
void TiDfQuoteClient::OnLv2TreeSze(EMQSzeTree *tree)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(tree);
    char cache[kCacheSize] = {0};
    sprintf(cache,
            "%u,%u,%llu,0x%02x,0x%02x,%s,%lld,%llu,%lld,%u,%u,%u,%u,%u,%u,%u,%llu,%u,"
            "%llu,%u,%u,%llu,%llu,%u,%u,%u,%llu,%u,%llu,%u,%llu,%u,%llu,%u,%lu\n",
            tree->m_head.m_sequence, tree->m_head.m_channel_num, tree->m_head.m_sequence_num,
            tree->m_head.m_security_type, tree->m_head.m_sub_security_type, tree->m_head.m_symbol,
            tree->m_total_trade_num, tree->m_total_quantity, tree->m_total_value, tree->m_pre_close_price,
            tree->m_last_price, tree->m_open_price, tree->m_day_high_price, tree->m_day_low_price,
            tree->m_today_close_price, tree->m_total_bid_weighted_avg_price, tree->m_total_bid_quantity,
            tree->m_total_ask_weighted_avg_price, tree->m_total_ask_quantity, tree->m_upper_limit_price,
            tree->m_low_limit_price, tree->m_market_open_total_bid, tree->m_market_open_total_ask, tree->m_bid_depth,
            tree->m_ask_depth, tree->m_bid_unit[0].m_price, tree->m_bid_unit[0].m_quantity, tree->m_bid_unit[9].m_price,
            tree->m_bid_unit[9].m_quantity, tree->m_ask_unit[0].m_price, tree->m_ask_unit[0].m_quantity,
            tree->m_ask_unit[9].m_price, tree->m_ask_unit[9].m_quantity, tree->m_ch_gap_flag == 0x00 ? 0 : 1, ts);

    std::cout << cache << std::flush;
}

// 上交所快照行情
void TiDfQuoteClient::OnLv2SnapSse(EMQSseSnap *snap)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(snap);
    char cache[kCacheSize];

    sprintf(cache,
            "%d,%d,%u,%s,%d,%d,%u,%u,%u,%u,%u,%u,%d,%d,%u,%llu,%llu,%llu,%u,%llu,%u,%u,%u,%llu,%u,%llu,%u,%llu,%u,%"
            "llu,%u,%hhu,%u,%u,%u,%u,%u,%u,%u,%lu\n",
            snap->m_head.m_category_id, snap->m_head.m_msg_seq_id, snap->m_quote_update_time, snap->m_symbol,
            snap->m_security_type, snap->m_image_status, snap->m_pre_close_price, snap->m_open_price,
            snap->m_day_high_price, snap->m_day_low_price, snap->m_last_price, snap->m_today_close_price,
            snap->m_instrument_status, snap->m_trading_status, snap->m_total_trade_num, snap->m_total_quantity,
            snap->m_total_value, snap->m_total_bid_quantity, snap->m_total_bid_weighted_avg_price,
            snap->m_total_ask_quantity, snap->m_total_ask_weighted_avg_price, snap->m_yield_to_maturity,
            snap->m_bid_unit[0].m_price, snap->m_bid_unit[0].m_quantity, snap->m_ask_unit[0].m_price,
            snap->m_ask_unit[0].m_quantity, snap->m_bid_unit[9].m_price, snap->m_bid_unit[9].m_quantity,
            snap->m_ask_unit[9].m_price, snap->m_ask_unit[9].m_quantity, snap->m_head.m_sequence,
            snap->m_head.m_message_type, snap->m_head.m_message_len, snap->m_head.m_exchange_id,
            snap->m_head.m_quote_date_year, snap->m_head.m_quote_date_month, snap->m_head.m_quote_date_day,
            snap->m_head.m_send_time, snap->m_head.m_seq_lost_flag, ts);

    std::cout << cache << std::flush;
}

// 上交所逐笔合并行情
void TiDfQuoteClient::OnLv2TickSse(EMQSseTick *tick)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(tick);
    char cache[kCacheSize] = {0};
    sprintf(cache, "tick_merge,%hu,%llu,%u,%s,%c,%llu,%llu,%u,%llu,%llu,0x%2x,%u,%hhu,%lu\n", tick->m_channel_num,
            tick->m_tick_index, tick->m_tick_time, tick->m_symbol, tick->m_tick_type, tick->m_buy_num, tick->m_sell_num,
            tick->m_price, tick->m_quantity, tick->m_trade_value, tick->m_side_flag, tick->m_sequence,
            tick->m_message_type, ts);

    std::cout << cache << std::flush;
}

// 上交所指数行情
void TiDfQuoteClient::OnLv2IndexSse(EMQSseIdx *idx)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(idx);
    char cache[1024];

    sprintf(cache, "%d,%d,%u,%s,%d,%u,%u,%llu,%u,%u,%u,%llu,%u,%u,%d,%d,%d,%d,%d,%d,%d,%u,%lu\n", idx->m_head.m_category_id,
            idx->m_head.m_msg_seq_id, idx->m_quote_update_time, idx->m_symbol, idx->m_security_type,
            idx->m_pre_close_price, idx->m_open_price, idx->m_total_value, idx->m_day_high_price, idx->m_day_low_price,
            idx->m_last_price, idx->m_total_quantity, idx->m_today_close_price, idx->m_head.m_sequence,
            idx->m_head.m_message_type, idx->m_head.m_message_len, idx->m_head.m_exchange_id,
            idx->m_head.m_quote_date_year, idx->m_head.m_quote_date_month, idx->m_head.m_quote_date_day,
            idx->m_head.m_send_time, idx->m_head.m_seq_lost_flag, ts);

    std::cout << cache << std::flush;
}

// 上交所债券快照行情
void TiDfQuoteClient::OnLv2BondSnapSse(EMQSseBondSnap *bond_snap)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(bond_snap);
    char cache[kCacheSize];

    sprintf(cache,
            "%d,%d,%u,%s,%d,%d,%u,%u,%u,%u,%u,%u,%u,%u,%llu,%llu,%llu,%u,%llu,%u,%u,%llu,%llu,%u,%llu,%llu,%u,%u,%u,%u,"
            "%u,%llu,%u,%llu,%u,%llu,%u,%llu,%u,%u,%d,%d,%d,%d,%d,%d,%d,%d,%d,%lu\n",
            bond_snap->m_head.m_category_id, bond_snap->m_head.m_msg_seq_id, bond_snap->m_quote_update_time,
            bond_snap->m_symbol, bond_snap->m_security_type, bond_snap->m_sub_security_type,
            bond_snap->m_pre_close_price, bond_snap->m_open_price, bond_snap->m_day_high_price,
            bond_snap->m_day_low_price, bond_snap->m_last_price, bond_snap->m_today_close_price,
            bond_snap->m_instrument_status, bond_snap->m_total_trade_num, bond_snap->m_total_quantity,
            bond_snap->m_total_value, bond_snap->m_total_bid_quantity, bond_snap->m_total_bid_weighted_avg_price,
            bond_snap->m_total_ask_quantity, bond_snap->m_total_ask_weighted_avg_price, bond_snap->m_withdraw_bid_num,
            bond_snap->m_withdraw_bid_amount, bond_snap->m_withdraw_bid_price, bond_snap->m_withdraw_ask_num,
            bond_snap->m_withdraw_ask_amount, bond_snap->m_withdraw_ask_price, bond_snap->m_total_bid_num,
            bond_snap->m_total_ask_num, bond_snap->m_bid_trade_max_duration, bond_snap->m_ask_trade_max_duration,
            bond_snap->m_bid_unit[0].m_price, bond_snap->m_bid_unit[0].m_quantity, bond_snap->m_ask_unit[0].m_price,
            bond_snap->m_ask_unit[0].m_quantity, bond_snap->m_bid_unit[9].m_price, bond_snap->m_bid_unit[9].m_quantity,
            bond_snap->m_ask_unit[9].m_price, bond_snap->m_ask_unit[9].m_quantity, bond_snap->m_head.m_sequence,
            bond_snap->m_head.m_message_type, bond_snap->m_head.m_message_len, bond_snap->m_head.m_exchange_id,
            bond_snap->m_head.m_quote_date_year, bond_snap->m_head.m_quote_date_month,
            bond_snap->m_head.m_quote_date_day, bond_snap->m_head.m_send_time, bond_snap->m_head.m_seq_lost_flag,
            bond_snap->m_bid_depth, bond_snap->m_ask_depth, ts);

    std::cout << cache << std::flush;
}

// 上交所债券逐笔行情
void TiDfQuoteClient::OnLv2BondTickSse(EMQSseBondTick *bond_tick)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(bond_tick);
    char cache[kCacheSize];

    sprintf(cache, "%d,%d,%u,%u,%s,%d,%d,%u,%u,%llu,%llu,%u,%llu,%llu,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%lu\n",
            bond_tick->m_head.m_category_id, bond_tick->m_head.m_msg_seq_id, bond_tick->m_tick_index,
            bond_tick->m_channel_num, bond_tick->m_symbol, bond_tick->m_security_type, bond_tick->m_sub_security_type,
            bond_tick->m_tick_time, bond_tick->m_tick_type, bond_tick->m_buy_num, bond_tick->m_sell_num,
            bond_tick->m_price, bond_tick->m_quantity, bond_tick->m_trade_value, bond_tick->m_side_flag,
            bond_tick->m_instrument_status, bond_tick->m_head.m_sequence, bond_tick->m_head.m_message_type,
            bond_tick->m_head.m_message_len, bond_tick->m_head.m_exchange_id, bond_tick->m_head.m_quote_date_year,
            bond_tick->m_head.m_quote_date_month, bond_tick->m_head.m_quote_date_day, bond_tick->m_head.m_send_time,
            bond_tick->m_head.m_seq_lost_flag, ts);

    std::cout << cache << std::flush;
}

// 上交所建树行情
void TiDfQuoteClient::OnLv2TreeSse(EMQSseTree *tree)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(tree);
    char cache[kCacheSize] = {0};
    sprintf(cache,
            "%u,%u,%u,%u,%s,%u,%u,%llu,%lld,%u,%u,%u,%u,%u,%u,%u,%llu,%u,"
            "%llu,%u,%u,%u,%llu,%u,%llu,%u,%llu,%u,%llu,%lu\n",
            tree->m_head.m_sequence, tree->m_head.m_send_time, tree->m_head.m_msg_seq_id, tree->m_quote_update_time,
            tree->m_symbol, tree->m_security_type, tree->m_total_trade_num, tree->m_total_quantity, tree->m_total_value,
            tree->m_pre_close_price, tree->m_last_price, tree->m_open_price, tree->m_day_high_price,
            tree->m_day_low_price, tree->m_today_close_price, tree->m_total_bid_weighted_avg_price,
            tree->m_total_bid_quantity, tree->m_total_ask_weighted_avg_price, tree->m_total_ask_quantity,
            tree->m_bid_depth, tree->m_ask_depth, tree->m_bid_unit[0].m_price, tree->m_bid_unit[0].m_quantity,
            tree->m_bid_unit[9].m_price, tree->m_bid_unit[9].m_quantity, tree->m_ask_unit[0].m_price,
            tree->m_ask_unit[0].m_quantity, tree->m_ask_unit[9].m_price, tree->m_ask_unit[9].m_quantity, ts);

    std::cout << cache << std::flush;
}

//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////

void TiDfQuoteClient::connect()
{
    quote_api_ = EMQ::API::QuoteApiLv2::CreateQuoteApiLv2("./emq_api_log/emq.log");
    quote_api_->RegisterSpi(this);

    constexpr int kConfigNum = 12;
    EMQ::API::EMQConfigLv2 configs[kConfigNum];
    configs[0].enable = true;
    configs[0].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[0].quote_type = EMQ::API::EMQType::kSzeSnap;
    strcpy(configs[0].eth_name, "eno1");
    strcpy(configs[0].multicast_ip, "233.57.1.100");
    configs[0].multicast_port = 37100;
    configs[0].rx_cpu_id = 0;
    configs[0].handle_cpu_id = 1;
    configs[0].spsc_size = 8;
    configs[0].rx_pkt_num = 8;

    configs[1].enable = true;
    configs[1].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[1].quote_type = EMQ::API::EMQType::kSzeTick;
    strcpy(configs[1].eth_name, "eno1");
    strcpy(configs[1].multicast_ip, "233.57.1.101");
    configs[1].multicast_port = 37101;
    configs[1].rx_cpu_id = 2;
    configs[1].handle_cpu_id = 3;
    configs[1].spsc_size = 128;
    configs[1].rx_pkt_num = 128;

    configs[2].enable = true;
    configs[2].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[2].quote_type = EMQ::API::EMQType::kSzeIndex;
    strcpy(configs[2].eth_name, "eno1");
    strcpy(configs[2].multicast_ip, "233.57.1.102");
    configs[2].multicast_port = 37102;
    configs[2].rx_cpu_id = 4;
    configs[2].handle_cpu_id = 5;
    configs[2].spsc_size = 128;
    configs[2].rx_pkt_num = 128;

    configs[3].enable = true;
    configs[3].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[3].quote_type = EMQ::API::EMQType::kSzeBondSnap;
    strcpy(configs[3].eth_name, "eno1");
    strcpy(configs[3].multicast_ip, "233.57.1.107");
    configs[3].multicast_port = 37107;
    configs[3].rx_cpu_id = 4;
    configs[3].handle_cpu_id = 5;
    configs[3].spsc_size = 128;
    configs[3].rx_pkt_num = 128;

    configs[4].enable = true;
    configs[4].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[4].quote_type = EMQ::API::EMQType::kSzeBondTick;
    strcpy(configs[4].eth_name, "eno1");
    strcpy(configs[4].multicast_ip, "233.57.1.108");
    configs[4].multicast_port = 37108;
    configs[4].rx_cpu_id = 4;
    configs[4].handle_cpu_id = 5;
    configs[4].spsc_size = 128;
    configs[4].rx_pkt_num = 128;

    configs[5].enable = false;
    configs[5].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[5].quote_type = EMQ::API::EMQType::kSzeTree;
    strcpy(configs[5].eth_name, "eno1");
    strcpy(configs[5].multicast_ip, "233.57.1.101");
    configs[5].multicast_port = 37101;
    configs[5].rx_cpu_id = 4;
    configs[5].handle_cpu_id = 5;
    configs[5].spsc_size = 128;
    configs[5].rx_pkt_num = 128;

    configs[6].enable = true;
    configs[6].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[6].quote_type = EMQ::API::EMQType::kSseSnap;
    strcpy(configs[6].eth_name, "eno1");
    strcpy(configs[6].multicast_ip, "233.56.2.105");
    configs[6].multicast_port = 36105;
    configs[6].rx_cpu_id = 6;
    configs[6].handle_cpu_id = 7;
    configs[6].spsc_size = 8;
    configs[6].rx_pkt_num = 8;

    configs[7].enable = true;
    configs[7].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[7].quote_type = EMQ::API::EMQType::kSseTick;
    strcpy(configs[7].eth_name, "eno1");
    strcpy(configs[7].multicast_ip, "233.57.2.110");
    configs[7].multicast_port = 36110;
    configs[7].rx_cpu_id = 8;
    configs[7].handle_cpu_id = 9;
    configs[7].spsc_size = 8;
    configs[7].rx_pkt_num = 8;

    configs[8].enable = true;
    configs[8].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[8].quote_type = EMQ::API::EMQType::kSseIndex;
    strcpy(configs[8].eth_name, "eno1");
    strcpy(configs[8].multicast_ip, "233.56.2.102");
    configs[8].multicast_port = 36102;
    configs[8].rx_cpu_id = 10;
    configs[8].handle_cpu_id = 11;
    configs[8].spsc_size = 8;
    configs[8].rx_pkt_num = 8;

    configs[9].enable = true;
    configs[9].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[9].quote_type = EMQ::API::EMQType::kSseBondSnap;
    strcpy(configs[9].eth_name, "eno1");
    strcpy(configs[9].multicast_ip, "233.56.2.107");
    configs[9].multicast_port = 36107;
    configs[9].rx_cpu_id = 10;
    configs[9].handle_cpu_id = 11;
    configs[9].spsc_size = 8;
    configs[9].rx_pkt_num = 8;

    configs[10].enable = true;
    configs[10].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[10].quote_type = EMQ::API::EMQType::kSseBondTick;
    strcpy(configs[10].eth_name, "eno1");
    strcpy(configs[10].multicast_ip, "233.56.2.108");
    configs[10].multicast_port = 36108;
    configs[10].rx_cpu_id = 10;
    configs[10].handle_cpu_id = 11;
    configs[10].spsc_size = 8;
    configs[10].rx_pkt_num = 8;

    configs[11].enable = false;
    configs[11].mode = EMQ::API::EMQRecvMode::kNormal;
    configs[11].quote_type = EMQ::API::EMQType::kSseTree;
    strcpy(configs[11].eth_name, "eno1");
    strcpy(configs[11].multicast_ip, "233.57.1.101");
    configs[11].multicast_port = 37101;
    configs[11].rx_cpu_id = 10;
    configs[11].handle_cpu_id = 11;
    configs[11].spsc_size = 8;
    configs[11].rx_pkt_num = 8;
    quote_api_->SetChannelConfig(configs, kConfigNum);

    int x = quote_api_->Login("61.129.116.188", 9988, "510100025168", "OW4273");
    std::cout << "Login: " << x << std::endl;
    quote_api_->Start();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
