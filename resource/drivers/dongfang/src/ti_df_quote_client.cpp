#include <iostream>
#include <iniFile.h>
#include <thread>
#include <glog/logging.h>
#include "ti_encoding_tool.h"
#include "ti_df_quote_client.h"
#include "ti_quote_struct.h"
#include "datetime.h"

#include "ti_quote_formater.h"

#define Enable_Df_DataOutput 0

TiDfQuoteClient::TiDfQuoteClient()
{
    m_trading_day = datetime::get_today();
    m_quote_api = NULL;
    m_quoteL1Client = new TiDfQuoteL1Client();

    m_quoteL1Client->Run();
}

TiDfQuoteClient::~TiDfQuoteClient()
{
    m_quote_api->Release();
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
    
#if Enable_Df_DataOutput
    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapIndexCash, j);
    std::cout << "FormatSnapshot: " << j.dump() << std::endl;
#endif

    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }
}

// 深交所逐笔合并行情
void TiDfQuoteClient::OnLv2TickSze(EMQSzeTick *tick)
{
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

#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatOrder(&m_orderCash, j);
        std::cout << "FormatOrder: "  << j.dump() << std::endl;
#endif

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }
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

#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatMatch(&m_matchCash, j);
        std::cout << "FormatMatch: " << j.dump() << std::endl;
#endif

        if(m_cb){
            m_cb->OnL2StockMatchesRtn(&m_matchCash);
        }
    }
}

// 深交所指数行情
void TiDfQuoteClient::OnLv2IndexSze(EMQSzeIdx *idx)
{
    memset(&m_snapIndexCash, 0, sizeof(TiQuoteSnapshotIndexField));
    strcpy(m_snapIndexCash.symbol, (char*)idx->m_head.m_symbol);
    strcpy(m_snapIndexCash.exchange, "SZ");
    formatQuoteUpdatetime(idx->m_head.m_quote_update_time, 
        m_snapIndexCash.date, m_snapIndexCash.time, m_snapIndexCash.timestamp);
    datetime::get_format_time_ms(m_snapIndexCash.date, m_snapIndexCash.time, m_snapIndexCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapIndexCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapIndexCash.recv_timestamp = datetime::get_now_timestamp_ms();

    m_snapIndexCash.last            = idx->m_last_price/10000;
    m_snapIndexCash.pre_close       = idx->m_pre_close_price/10000;
    m_snapIndexCash.open            = idx->m_open_price/10000;
    m_snapIndexCash.high            = idx->m_day_high_price/10000;
    m_snapIndexCash.low             = idx->m_day_low_price/10000;
    m_snapIndexCash.volume          = idx->m_total_quantity/100;
    m_snapIndexCash.turnover        = idx->m_total_value/1000000;
    m_snapIndexCash.close           = idx->m_today_close_price/10000;

#if Enable_Df_DataOutput
    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapIndexCash, j);
    std::cout << "FormatSnapshot: " << j.dump() << std::endl;
#endif

    if(m_cb){
        m_cb->OnL2IndexSnapshotRtn(&m_snapIndexCash);
    }
}

// 深交所债券快照行情
void TiDfQuoteClient::OnLv2BondSnapSze(EMQSzeBondSnap *bond_snap)
{
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));
    strcpy(m_snapStockCash.symbol, (char*)bond_snap->m_head.m_symbol);
    strcpy(m_snapStockCash.exchange, "SZ");
    formatQuoteUpdatetime(bond_snap->m_head.m_quote_update_time, 
        m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.timestamp);
    datetime::get_format_time_ms(m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp = datetime::get_now_timestamp_ms();

    m_snapStockCash.last            = bond_snap->m_last_price/10000;
    m_snapStockCash.pre_close       = bond_snap->m_pre_close_price/10000;
    m_snapStockCash.open            = bond_snap->m_open_price/10000;
    m_snapStockCash.high            = bond_snap->m_day_high_price/10000;
    m_snapStockCash.low             = bond_snap->m_day_low_price/10000;
    m_snapStockCash.acc_volume      = bond_snap->m_total_quantity/100;
    m_snapStockCash.acc_turnover    = bond_snap->m_total_value/1000000;
    m_snapStockCash.match_items     = bond_snap->m_total_trade_num;
    m_snapStockCash.total_ask_qty   = bond_snap->m_total_ask_quantity/100;
    m_snapStockCash.total_bid_qty   = bond_snap->m_total_bid_quantity/100;
    m_snapStockCash.wavg_ask        = bond_snap->m_total_ask_weighted_avg_price/10000;
    m_snapStockCash.wavg_bid        = bond_snap->m_total_bid_weighted_avg_price/10000;
    
    for (size_t i = 0; i < 10; i++)
    {
        m_snapStockCash.ask_price[i] = bond_snap->m_ask_unit[i].m_price/10000;
        m_snapStockCash.ask_volume[i] = bond_snap->m_ask_unit[i].m_quantity/100;

        m_snapStockCash.bid_price[i] = bond_snap->m_bid_unit[i].m_price/10000;
        m_snapStockCash.bid_volume[i] = bond_snap->m_bid_unit[i].m_quantity/100;
    }

#if Enable_Df_DataOutput
    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapStockCash, j);
    std::cout << "FormatSnapshot: " << j.dump() << std::endl;
#endif

    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }

}

// 深交所债券逐笔合并行情
void TiDfQuoteClient::OnLv2BondTickSze(EMQSzeBondTick *bond_tick)
{
    if (bond_tick->m_tick_type == EMQSzeTickType::kTickTypeBondOrder)
    {
        memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));
        strcpy(m_orderCash.symbol, (char*)bond_tick->m_bond_order->m_head.m_symbol);
        strcpy(m_orderCash.exchange, "SZ");
        formatQuoteUpdatetime(bond_tick->m_bond_order->m_head.m_quote_update_time, 
            m_orderCash.date, m_orderCash.time, m_orderCash.timestamp);
        datetime::get_format_time_ms(m_orderCash.date, m_orderCash.time, m_orderCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_orderCash.recv_time_str, TI_TIME_STR_LEN);
        m_orderCash.recv_timestamp = datetime::get_now_timestamp_ms();

        m_orderCash.channel         = bond_tick->m_bond_order->m_head.m_channel_num;
        m_orderCash.seq             = bond_tick->m_bond_order->m_head.m_sequence;
        m_orderCash.price           = bond_tick->m_bond_order->m_price/10000;
        m_orderCash.volume          = bond_tick->m_bond_order->m_quantity/100;
        m_orderCash.order_orino     = bond_tick->m_bond_order->m_head.m_sequence;

        if(bond_tick->m_bond_order->m_side == '1'){
            m_orderCash.function_code = 'B'; 
        }
        if(bond_tick->m_bond_order->m_side == '2'){
            m_orderCash.function_code = 'S'; 
        }

        if (bond_tick->m_bond_order->m_order_type == '2') //限价单
        {
            m_orderCash.order_type      = '0';
        }else{
            m_orderCash.order_type      = bond_tick->m_bond_order->m_order_type;
        }

#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatOrder(&m_orderCash, j);
        std::cout << "FormatOrder: "  << j.dump() << std::endl;
#endif
        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }
    }
    else if (bond_tick->m_tick_type == EMQSzeTickType::kTickTypeBondExe)
    {
        memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));
        strcpy(m_matchCash.symbol, (char*)bond_tick->m_bond_exe->m_head.m_symbol);
        strcpy(m_matchCash.exchange, "SZ");
        formatQuoteUpdatetime(bond_tick->m_bond_exe->m_head.m_quote_update_time, 
            m_matchCash.date, m_matchCash.time, m_matchCash.timestamp);
        datetime::get_format_time_ms(m_matchCash.date, m_matchCash.time, m_matchCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_matchCash.recv_time_str, TI_TIME_STR_LEN);
        m_matchCash.recv_timestamp = datetime::get_now_timestamp_ms();

        m_matchCash.channel         = bond_tick->m_bond_exe->m_head.m_channel_num;
        m_matchCash.seq             = bond_tick->m_bond_exe->m_head.m_sequence;
        m_matchCash.price           = bond_tick->m_bond_exe->m_price/10000;
        m_matchCash.volume          = bond_tick->m_bond_exe->m_quantity/100;
        m_matchCash.ask_order_seq   = bond_tick->m_bond_exe->m_sell_num;
        m_matchCash.bid_order_seq   = bond_tick->m_bond_exe->m_buy_num;

        if(bond_tick->m_bond_exe->m_type == 'F')
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

#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatMatch(&m_matchCash, j);
        std::cout << "FormatMatch: " << j.dump() << std::endl;
#endif

        if(m_cb){
            m_cb->OnL2StockMatchesRtn(&m_matchCash);
        }

    }
}

// 深交所建树行情
void TiDfQuoteClient::OnLv2TreeSze(EMQSzeTree *tree)
{
    return;
}

// 上交所快照行情
void TiDfQuoteClient::OnLv2SnapSse(EMQSseSnap *snap)
{
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));

    strcpy(m_snapStockCash.symbol, (char*)snap->m_symbol);
    strcpy(m_snapStockCash.exchange, "SH");
    m_snapStockCash.date            = m_trading_day;
    m_snapStockCash.time            = snap->m_quote_update_time*1000;
    m_snapStockCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_snapStockCash.time);
    datetime::get_format_time_ms(m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp = datetime::get_now_timestamp_ms();

    m_snapStockCash.last            = snap->m_last_price/10000;
    m_snapStockCash.pre_close       = snap->m_pre_close_price/10000;
    m_snapStockCash.open            = snap->m_open_price/10000;
    m_snapStockCash.high            = snap->m_day_high_price/10000;
    m_snapStockCash.low             = snap->m_day_low_price/10000;
    //m_snapStockCash.high_limit      = snap->m_upper_limit_price/10000;
    //m_snapStockCash.low_limit       = snap->m_low_limit_price/10000;
    m_snapStockCash.acc_volume      = snap->m_total_quantity/100;
    m_snapStockCash.acc_turnover    = snap->m_total_value/1000000;
    m_snapStockCash.match_items     = snap->m_total_trade_num;
    m_snapStockCash.total_ask_qty   = snap->m_total_ask_quantity/100;
    m_snapStockCash.total_bid_qty   = snap->m_total_bid_quantity/100;
    m_snapStockCash.wavg_ask        = snap->m_total_ask_weighted_avg_price/10000;
    m_snapStockCash.wavg_bid        = snap->m_total_bid_weighted_avg_price/10000;

    for (size_t i = 0; i < 10; i++)
    {
        m_snapStockCash.ask_price[i] = snap->m_ask_unit[i].m_price/10000;
        m_snapStockCash.ask_volume[i] = snap->m_ask_unit[i].m_quantity/100;

        m_snapStockCash.bid_price[i] = snap->m_bid_unit[i].m_price/10000;
        m_snapStockCash.bid_volume[i] = snap->m_bid_unit[i].m_quantity/100;
    }

#if Enable_Df_DataOutput
    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapStockCash, j);
    std::cout << "FormatSnapshot: " << j.dump() << std::endl;
#endif

    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }
}

// 上交所逐笔合并行情
void TiDfQuoteClient::OnLv2TickSse(EMQSseTick *tick)
{
    if (tick->m_tick_type == 'A' || tick->m_tick_type == 'D')
    {
        memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));
        strcpy(m_orderCash.symbol, (char*)tick->m_symbol);
        strcpy(m_orderCash.exchange, "SH");
        m_orderCash.date            = m_trading_day;
        m_orderCash.time            = tick->m_tick_time*10;
        m_orderCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_orderCash.time);
        datetime::get_format_time_ms(m_orderCash.date, m_orderCash.time, m_orderCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_orderCash.recv_time_str, TI_TIME_STR_LEN);
        m_orderCash.recv_timestamp = datetime::get_now_timestamp_ms();

        m_orderCash.channel         = tick->m_channel_num;
        m_orderCash.seq             = tick->m_sequence;
        m_orderCash.price           = tick->m_price/10000;
        m_orderCash.volume          = tick->m_quantity/100;
        m_orderCash.order_orino     = tick->m_sequence;

        if (tick->m_side_flag == 0x00)
        {
            m_orderCash.function_code = 'B';
        }
        if (tick->m_side_flag == 0x01)
        {
            m_orderCash.function_code = 'S';
        }
        m_orderCash.order_type      = tick->m_tick_type;
        
#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatOrder(&m_orderCash, j);
        std::cout << "FormatOrder: "  << j.dump() << std::endl;
#endif

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }

    }

    if (tick->m_tick_type == 'T')
    {
        memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));
        strcpy(m_matchCash.symbol, (char*)tick->m_symbol);
        strcpy(m_matchCash.exchange, "SH");
        m_matchCash.date            = m_trading_day;
        m_matchCash.time            = tick->m_tick_time*10;
        m_matchCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_matchCash.time);
        datetime::get_format_time_ms(m_matchCash.date, m_matchCash.time, m_matchCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_matchCash.recv_time_str, TI_TIME_STR_LEN);
        m_matchCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        m_matchCash.channel         = tick->m_channel_num;
        m_matchCash.seq             = tick->m_sequence;
        m_matchCash.price           = tick->m_price/10000;
        m_matchCash.volume          = tick->m_quantity/100;
        m_matchCash.ask_order_seq   = tick->m_sell_num;
        m_matchCash.bid_order_seq   = tick->m_buy_num;
        
        m_matchCash.function_code   = '0';

        if (tick->m_side_flag == 0x00)
        {
            m_matchCash.bs_flag = 'B';
        }
        if (tick->m_side_flag == 0x01)
        {
            m_matchCash.bs_flag = 'S';
        }

#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatMatch(&m_matchCash, j);
        std::cout << "FormatMatch: " << j.dump() << std::endl;
#endif

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }
        
    }
}

// 上交所指数行情
void TiDfQuoteClient::OnLv2IndexSse(EMQSseIdx *idx)
{
    memset(&m_snapIndexCash, 0, sizeof(TiQuoteSnapshotIndexField));
    strcpy(m_snapIndexCash.symbol, (char*)idx->m_symbol);
    strcpy(m_snapIndexCash.exchange, "SH");
    m_snapIndexCash.date            = m_trading_day;
    m_snapIndexCash.time            = idx->m_quote_update_time*1000;
    m_snapIndexCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_snapIndexCash.time);
    datetime::get_format_time_ms(m_snapIndexCash.date, m_snapIndexCash.time, m_snapIndexCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapIndexCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapIndexCash.recv_timestamp  = datetime::get_now_timestamp_ms();
    
    m_snapIndexCash.last            = idx->m_last_price/10000;
    m_snapIndexCash.pre_close       = idx->m_pre_close_price/10000;
    m_snapIndexCash.open            = idx->m_open_price/10000;
    m_snapIndexCash.high            = idx->m_day_high_price/10000;
    m_snapIndexCash.low             = idx->m_day_low_price/10000;
    m_snapIndexCash.volume          = idx->m_total_quantity/100;
    m_snapIndexCash.turnover        = idx->m_total_value/1000000;
    m_snapIndexCash.close           = idx->m_today_close_price/10000;

#if Enable_Df_DataOutput
    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapIndexCash, j);
    std::cout << "FormatSnapshot: " << j.dump() << std::endl;
#endif

    if(m_cb){
        m_cb->OnL2IndexSnapshotRtn(&m_snapIndexCash);
    }
}

// 上交所债券快照行情
void TiDfQuoteClient::OnLv2BondSnapSse(EMQSseBondSnap *bond_snap)
{
    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));

    strcpy(m_snapStockCash.symbol, (char*)bond_snap->m_symbol);
    strcpy(m_snapStockCash.exchange, "SH");
    m_snapStockCash.date            = m_trading_day;
    m_snapStockCash.time            = bond_snap->m_quote_update_time;
    m_snapStockCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_snapStockCash.time);
    datetime::get_format_time_ms(m_snapStockCash.date, m_snapStockCash.time, m_snapStockCash.time_str, TI_TIME_STR_LEN);
    datetime::get_format_now_time_us(m_snapStockCash.recv_time_str, TI_TIME_STR_LEN);
    m_snapStockCash.recv_timestamp  = datetime::get_now_timestamp_ms();

    m_snapStockCash.last            = bond_snap->m_last_price/10000;
    m_snapStockCash.pre_close       = bond_snap->m_pre_close_price/10000;
    m_snapStockCash.open            = bond_snap->m_open_price/10000;
    m_snapStockCash.high            = bond_snap->m_day_high_price/10000;
    m_snapStockCash.low             = bond_snap->m_day_low_price/10000;
    m_snapStockCash.acc_volume      = bond_snap->m_total_quantity/100;
    m_snapStockCash.acc_turnover    = bond_snap->m_total_value/1000000;
    m_snapStockCash.match_items     = bond_snap->m_total_trade_num;
    m_snapStockCash.total_ask_qty   = bond_snap->m_total_ask_quantity/100;
    m_snapStockCash.total_bid_qty   = bond_snap->m_total_bid_quantity/100;
    m_snapStockCash.wavg_ask        = bond_snap->m_total_ask_weighted_avg_price/10000;
    m_snapStockCash.wavg_bid        = bond_snap->m_total_bid_weighted_avg_price/10000;

    for (size_t i = 0; i < 10; i++)
    {
        m_snapStockCash.ask_price[i] = bond_snap->m_ask_unit[i].m_price/10000;
        m_snapStockCash.ask_volume[i] = bond_snap->m_ask_unit[i].m_quantity/100;

        m_snapStockCash.bid_price[i] = bond_snap->m_bid_unit[i].m_price/10000;
        m_snapStockCash.bid_volume[i] = bond_snap->m_bid_unit[i].m_quantity/100;
    }

#if Enable_Df_DataOutput
    json j;
    TiQuoteFormater::FormatSnapshot(&m_snapStockCash, j);
    std::cout << "FormatSnapshot: " << j.dump() << std::endl;
#endif

    if(m_cb){
        m_cb->OnL2StockSnapshotRtn(&m_snapStockCash);
    }
}

// 上交所债券逐笔行情
void TiDfQuoteClient::OnLv2BondTickSse(EMQSseBondTick *bond_tick)
{
    if (bond_tick->m_tick_type == 'A' || bond_tick->m_tick_type == 'D')
    {
        memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));
        strcpy(m_orderCash.symbol, (char*)bond_tick->m_symbol);
        strcpy(m_orderCash.exchange, "SH");
        m_orderCash.date            = m_trading_day;
        m_orderCash.time            = bond_tick->m_tick_time;
        m_orderCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_orderCash.time);
        datetime::get_format_time_ms(m_orderCash.date, m_orderCash.time, m_orderCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_orderCash.recv_time_str, TI_TIME_STR_LEN);
        m_orderCash.recv_timestamp = datetime::get_now_timestamp_ms();

        m_orderCash.channel         = bond_tick->m_channel_num;
        m_orderCash.seq             = bond_tick->m_tick_index;
        m_orderCash.price           = bond_tick->m_price/10000;
        m_orderCash.volume          = bond_tick->m_quantity/100;
        m_orderCash.order_orino     = bond_tick->m_tick_index;

        if (bond_tick->m_side_flag == 0x00)
        {
            m_orderCash.function_code = 'B';
        }
        if (bond_tick->m_side_flag == 0x01)
        {
            m_orderCash.function_code = 'S';
        }
        m_orderCash.order_type      = bond_tick->m_tick_type;

#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatOrder(&m_orderCash, j);
        std::cout << "FormatOrder: "  << j.dump() << std::endl;
#endif

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }

    }

    if (bond_tick->m_tick_type == 'T')
    {
        memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));
        strcpy(m_matchCash.symbol, (char*)bond_tick->m_symbol);
        strcpy(m_matchCash.exchange, "SH");
        m_matchCash.date            = m_trading_day;
        m_matchCash.time            = bond_tick->m_tick_time;
        m_matchCash.timestamp       = datetime::get_timestamp_ms(m_trading_day, m_matchCash.time);
        datetime::get_format_time_ms(m_matchCash.date, m_matchCash.time, m_matchCash.time_str, TI_TIME_STR_LEN);
        datetime::get_format_now_time_us(m_matchCash.recv_time_str, TI_TIME_STR_LEN);
        m_matchCash.recv_timestamp  = datetime::get_now_timestamp_ms();

        m_matchCash.channel         = bond_tick->m_channel_num;
        m_matchCash.seq             = bond_tick->m_tick_index;
        m_matchCash.price           = bond_tick->m_price/10000;
        m_matchCash.volume          = bond_tick->m_quantity/100;
        m_matchCash.ask_order_seq   = bond_tick->m_sell_num;
        m_matchCash.bid_order_seq   = bond_tick->m_buy_num;
        
        m_matchCash.function_code   = '0';

        if (bond_tick->m_side_flag == 0x00)
        {
            m_matchCash.bs_flag = 'B';
        }
        if (bond_tick->m_side_flag == 0x01)
        {
            m_matchCash.bs_flag = 'S';
        }

#if Enable_Df_DataOutput
        json j;
        TiQuoteFormater::FormatMatch(&m_matchCash, j);
        std::cout << "FormatMatch: " << j.dump() << std::endl;
#endif

        if(m_cb){
            m_cb->OnL2StockOrderRtn(&m_orderCash);
        }
        
    }
}

// 上交所建树行情
void TiDfQuoteClient::OnLv2TreeSse(EMQSseTree *tree)
{
    return;
}

//////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////

void TiDfQuoteClient::connect()
{
    m_quote_api = EMQ::API::QuoteApiLv2::CreateQuoteApiLv2("./emq_api_log/emq.log");
    m_quote_api->RegisterSpi(this);

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
    strcpy(configs[7].multicast_ip, "233.56.2.110");
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
    m_quote_api->SetChannelConfig(configs, kConfigNum);

    int x = m_quote_api->Login("61.129.116.188", 9988, "510100025168", "OW4273");
    std::cout << "Login: " << x << std::endl;
    m_quote_api->Start();
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
