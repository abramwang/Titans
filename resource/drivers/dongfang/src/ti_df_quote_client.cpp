#include <iostream>
#include <iniFile.h>
#include <thread>
#include <glog/logging.h>
#include "ti_encoding_tool.h"
#include "ti_df_quote_client.h"
#include "ti_quote_struct.h"
#include "datetime.h"

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

void TiDfQuoteClient::Run()
{
    quote_api_ = EMQ::API::QuoteApiLv2::CreateQuoteApiLv2("./emq_api_log/emq.log");
    quote_api_->RegisterSpi(this);

    int x = quote_api_->Login("61.129.116.188", 9988, "510100025168", "OW4273");
    std::cout << "Login: " << x << std::endl;
    quote_api_->Start();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// 深交所快照行情
void TiDfQuoteClient::OnLv2SnapSze(EMQSzeSnap *snap)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(snap);

    char cache[1024];
    sprintf(cache,
            "%d,%d,%s,%llu,%d,%#x,%llu,%llu,%llu,%u,%u,%u,%u,%u,%u,%u,%llu,%u,%llu,%u,%u,%u,%u,%u,%u,%llu,%u,%llu,%u,"
            "%llu,%u,%llu,%d,%d,%hhu,%llu,%u,%lu\n",
            snap->m_head.m_security_type, snap->m_head.m_sub_security_type, snap->m_head.m_symbol,
            snap->m_head.m_quote_update_time, snap->m_head.m_channel_num, snap->m_trading_status,
            snap->m_total_trade_num, snap->m_total_quantity, snap->m_total_value, snap->m_pre_close_price,
            snap->m_last_price, snap->m_open_price, snap->m_day_high_price, snap->m_day_low_price,
            snap->m_today_close_price, snap->m_total_bid_weighted_avg_price, snap->m_total_bid_quantity,
            snap->m_total_ask_weighted_avg_price, snap->m_total_ask_quantity, snap->m_lpv, snap->m_iopv,
            snap->m_upper_limit_price, snap->m_low_limit_price, snap->m_open_interest, snap->m_bid_unit[0].m_price,
            snap->m_bid_unit[0].m_quantity, snap->m_ask_unit[0].m_price, snap->m_ask_unit[0].m_quantity,
            snap->m_bid_unit[9].m_price, snap->m_bid_unit[9].m_quantity, snap->m_ask_unit[9].m_price,
            snap->m_ask_unit[9].m_quantity, snap->m_head.m_sequence, snap->m_head.m_message_type,
            snap->m_head.m_exchange_id, snap->m_head.m_sequence_num, snap->m_head.m_md_stream_id, ts);

    std::cout << cache << std::flush;
}

// 深交所逐笔合并行情
void TiDfQuoteClient::OnLv2TickSze(EMQSzeTick *tick)
{
    auto ts = quote_api_->GetPacketHardwareRXTs(tick);
    char cache[kCacheSize] = {0};
    if (tick->m_tick_type == EMQSzeTickType::kTickTypeOrder)
    {
        sprintf(cache, "order,0x%02x,0x%02x,%s,%llu,%u,%llu,%u,%llu,0x%02x,%c,%d,%d,%u,%u,%llu\n",
                tick->m_tick_order->m_head.m_security_type, tick->m_tick_order->m_head.m_sub_security_type,
                tick->m_tick_order->m_head.m_symbol, tick->m_tick_order->m_head.m_quote_update_time,
                tick->m_tick_order->m_head.m_channel_num, tick->m_tick_order->m_head.m_sequence_num,
                tick->m_tick_order->m_order_price, tick->m_tick_order->m_order_quantity,
                tick->m_tick_order->m_side_flag, tick->m_tick_order->m_order_type,
                tick->m_tick_order->m_head.m_sequence, tick->m_tick_order->m_head.m_message_type,
                tick->m_tick_order->m_head.m_exchange_id, tick->m_tick_order->m_head.m_md_stream_id, ts);
        std::cout << cache << std::flush;
    }
    else if (tick->m_tick_type == EMQSzeTickType::kTickTypeExe)
    {
        sprintf(cache, "exe,0x%02x,0x%02x,%s,%llu,%u,%llu,%lld,%lld,%u,%lld,%c,%u,%d,%u,%u,%lu\n",
                tick->m_tick_exe->m_head.m_security_type, tick->m_tick_exe->m_head.m_sub_security_type,
                tick->m_tick_exe->m_head.m_symbol, tick->m_tick_exe->m_head.m_quote_update_time,
                tick->m_tick_exe->m_head.m_channel_num, tick->m_tick_exe->m_head.m_sequence_num,
                tick->m_tick_exe->m_trade_buy_num, tick->m_tick_exe->m_trade_sell_num, tick->m_tick_exe->m_trade_price,
                tick->m_tick_exe->m_trade_quantity, tick->m_tick_exe->m_trade_type, tick->m_tick_exe->m_head.m_sequence,
                tick->m_tick_exe->m_head.m_message_type, tick->m_tick_exe->m_head.m_exchange_id,
                tick->m_tick_exe->m_head.m_md_stream_id, ts);
        std::cout << cache << std::flush;
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
