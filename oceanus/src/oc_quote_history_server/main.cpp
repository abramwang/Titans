#include "ti_quote_reader.h"
#include "datetime.h"
#include <iostream>

#include <stdlib.h> 
#include <thread>

#if 0
void read_match(){
    char* file = "/opt/ti_data/ti_market_data/ti.matches.SH.20230307.parquet";
    //read_whole_file(file);
    //read_single_column(file);

    long long _start = datetime::get_now_timestamp_ms();
    //read_whole_file(file);
    
    char* field_list[6] = {"date", "time", "price", "volume", "symbol", "timestamp"};
    
    TiQuoteMatchReader reader(file, field_list, 0);
    char* symbol_list[2] = {"600000", "600004"};
    //reader.SetQrySymbol(symbol_list, 2);
    reader.SetTime(10, 01, 03, 210);
    TiQuoteMatchesField data = {0};
    while(reader.GetNext(&data)){
        printf("%s, %d, %d, %c, %c, %f, %d, %d, %d, %d, %d, %ld, %s, %s \n", 
            data.symbol, data.date, data.time, data.function_code, data.bs_flag,
            data.price, data.volume, data.ask_order_seq, data.bid_order_seq,
            data.channel, data.seq, data.timestamp, data.time_str, data.exchange
        );
        break;
    };
    //printf("reader time: %ld;\n", reader.Time());
    //ti_quote_read_matches(file, read_matches_row_fn);
    long long _end = datetime::get_now_timestamp_ms();
    std::cout << "used time: " << _end - _start << std::endl;
}

void read_order(){
    char* file = "/opt/ti_data/ti_market_data/ti.order.SH.20230307.parquet";
    //read_whole_file(file);
    //read_single_column(file);

    long long _start = datetime::get_now_timestamp_ms();
    //read_whole_file(file);
    
    char* field_list[6] = {"date", "time", "price", "volume", "symbol", "timestamp"};
    
    TiQuoteOrderReader reader(file, field_list, 0);
    char* symbol_list[2] = {"600000", "600004"};
    //reader.SetQrySymbol(symbol_list, 2);
    reader.SetTime(10, 01, 03, 210);
    TiQuoteOrderField data = {0};
    while(reader.GetNext(&data)){
        printf("%s, %d, %d, %c, %c, %f, %d, %d, %d, %ld, %s, %s \n", 
            data.symbol, data.date, data.time, data.function_code, data.order_type,
            data.price, data.volume,
            data.channel, data.seq, data.timestamp, data.time_str, data.exchange
        );
        break;
    };
    //printf("reader time: %ld;\n", reader.Time());
    //ti_quote_read_matches(file, read_matches_row_fn);
    long long _end = datetime::get_now_timestamp_ms();
    std::cout << "used time: " << _end - _start << std::endl;
}


void read_snapshot(){
    char* file = "/opt/ti_data/ti_market_data/ti.snapshot.SH.20211018.parquet";
    //read_whole_file(file);
    //read_single_column(file);

    long long _start = datetime::get_now_timestamp_ms();
    //read_whole_file(file);
    
    char* field_list[10] = {"date", "time", "last", "volume", "bid_price", "bid_volume", "ask_price", "ask_volume", "symbol", "timestamp"};
    
    TiQuoteSnapshotReader reader(file, field_list, 0);
    std::list<std::string> symbol_list;
    symbol_list.push_back("600000");
    symbol_list.push_back("600004");
    reader.SetQrySymbol(symbol_list);
    reader.SetTime(10, 01, 03, 210);
    TiQuoteSnapshotStockField data = {0};
    while(reader.GetNext(&data)){
#if 1
        printf("%s, %d, %d, \
%f, %f, %f, %f, %f, %ld, %ld, \
%ld, %s, %s \n", 
            data.symbol, data.date, data.time,
            data.open, data.high, data.low, data.last, data.pre_close, data.acc_turnover, data.acc_volume,
            data.timestamp, data.time_str, data.exchange
        );
#else
        printf("%s, %d, %d, %f, %ld, \
%f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, \
%f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, \
%ld, %s, %s \n", 
            data.symbol, data.date, data.time,
            data.last, data.volume,
            data.bid_price[0], data.bid_volume[0], 
            data.bid_price[1], data.bid_volume[1], 
            data.bid_price[2], data.bid_volume[2], 
            data.bid_price[3], data.bid_volume[3], 
            data.bid_price[4], data.bid_volume[4], 
            data.bid_price[5], data.bid_volume[5], 
            data.bid_price[6], data.bid_volume[6], 
            data.bid_price[7], data.bid_volume[7], 
            data.bid_price[8], data.bid_volume[8], 
            data.bid_price[9], data.bid_volume[9], 
            data.ask_price[0], data.ask_volume[0], 
            data.ask_price[1], data.ask_volume[1], 
            data.ask_price[2], data.ask_volume[2], 
            data.ask_price[3], data.ask_volume[3], 
            data.ask_price[4], data.ask_volume[4], 
            data.ask_price[5], data.ask_volume[5], 
            data.ask_price[6], data.ask_volume[6], 
            data.ask_price[7], data.ask_volume[7], 
            data.ask_price[8], data.ask_volume[8], 
            data.ask_price[9], data.ask_volume[9], 
            data.timestamp, data.time_str, data.exchange
        );
#endif
        //break;
    };
    //printf("reader time: %ld;\n", reader.Time());
    //ti_quote_read_matches(file, read_matches_row_fn);
    long long _end = datetime::get_now_timestamp_ms();
    std::cout << "used time: " << _end - _start << std::endl;
}

#endif

void set_thread_pool_num(){
    char value[32] = {0};
    int kernel_num = std::thread::hardware_concurrency();
    kernel_num *= 0.6;
    if(kernel_num > 12){
        sprintf(value, "%d", 12);
    }else{
        sprintf(value, "%d", kernel_num);
    }
    //sprintf(value, "%d", kernel_num);
    setenv("UV_THREADPOOL_SIZE", value, 0);
    char *p; 
    if((p = getenv("UV_THREADPOOL_SIZE")))
        printf("UV_THREADPOOL_SIZE = %s\n",p); 
}

int main(int argc, char** argv) {
    set_thread_pool_num();
    
    //read_snapshot();
    //read_match();
    //read_order();
    //return 0;
    
    long long _start = datetime::get_now_timestamp_ms();

    TiQuoteReader reader("/opt/ti_data/ti_market_data");
    TiSymbolInfo symbol_list[4] = {
        {"SH", "600000"},
        {"SH", "600004"},
        {"SZ", "000001"},
        {"SZ", "000002"}
    };
    TI_QUOTE_DATA_TYPE quote_type_list[3] = {
        TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK, TI_QUOTE_DATA_TYPE_ORDER, TI_QUOTE_DATA_TYPE_MATCH
    };
    reader.ReadQuoteData(20230307, symbol_list, 4, quote_type_list, 3);

    TiQuoteReader::QuoteData dataCash = {0};

    long long _end = datetime::get_now_timestamp_ms();
    std::cout << "used time: " << _end - _start << std::endl;
    
    //std::cout << reader.GetNext(&dataCash) << std::endl;

    while (reader.GetNext(&dataCash))
    {
        continue;
        printf("[%c] ",dataCash.type);
        switch (dataCash.type)
        {
        case TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK:{
    #if 1
        printf("%s, %d, %d, \
%f, %f, %f, %f, %f, %ld, %ld, \
%ld, %s, %s \n", 
            dataCash.m_snapshot_stock.symbol, dataCash.m_snapshot_stock.date, dataCash.m_snapshot_stock.time,
            dataCash.m_snapshot_stock.open, dataCash.m_snapshot_stock.high, dataCash.m_snapshot_stock.low, dataCash.m_snapshot_stock.last, dataCash.m_snapshot_stock.pre_close, dataCash.m_snapshot_stock.acc_turnover, dataCash.m_snapshot_stock.acc_volume,
            dataCash.m_snapshot_stock.timestamp, dataCash.m_snapshot_stock.time_str, dataCash.m_snapshot_stock.exchange
        );
    #else
            printf("%s, %d, %d, %f, %ld, \
%f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, \
%f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, %f, %d, \
%ld, %s, %s \n", 
                dataCash.m_snapshot_stock.symbol, dataCash.m_snapshot_stock.date, dataCash.m_snapshot_stock.time,
                dataCash.m_snapshot_stock.last, dataCash.m_snapshot_stock.volume,
                dataCash.m_snapshot_stock.bid_price[0], dataCash.m_snapshot_stock.bid_volume[0], 
                dataCash.m_snapshot_stock.bid_price[1], dataCash.m_snapshot_stock.bid_volume[1], 
                dataCash.m_snapshot_stock.bid_price[2], dataCash.m_snapshot_stock.bid_volume[2], 
                dataCash.m_snapshot_stock.bid_price[3], dataCash.m_snapshot_stock.bid_volume[3], 
                dataCash.m_snapshot_stock.bid_price[4], dataCash.m_snapshot_stock.bid_volume[4], 
                dataCash.m_snapshot_stock.bid_price[5], dataCash.m_snapshot_stock.bid_volume[5], 
                dataCash.m_snapshot_stock.bid_price[6], dataCash.m_snapshot_stock.bid_volume[6], 
                dataCash.m_snapshot_stock.bid_price[7], dataCash.m_snapshot_stock.bid_volume[7], 
                dataCash.m_snapshot_stock.bid_price[8], dataCash.m_snapshot_stock.bid_volume[8], 
                dataCash.m_snapshot_stock.bid_price[9], dataCash.m_snapshot_stock.bid_volume[9], 
                dataCash.m_snapshot_stock.ask_price[0], dataCash.m_snapshot_stock.ask_volume[0], 
                dataCash.m_snapshot_stock.ask_price[1], dataCash.m_snapshot_stock.ask_volume[1], 
                dataCash.m_snapshot_stock.ask_price[2], dataCash.m_snapshot_stock.ask_volume[2], 
                dataCash.m_snapshot_stock.ask_price[3], dataCash.m_snapshot_stock.ask_volume[3], 
                dataCash.m_snapshot_stock.ask_price[4], dataCash.m_snapshot_stock.ask_volume[4], 
                dataCash.m_snapshot_stock.ask_price[5], dataCash.m_snapshot_stock.ask_volume[5], 
                dataCash.m_snapshot_stock.ask_price[6], dataCash.m_snapshot_stock.ask_volume[6], 
                dataCash.m_snapshot_stock.ask_price[7], dataCash.m_snapshot_stock.ask_volume[7], 
                dataCash.m_snapshot_stock.ask_price[8], dataCash.m_snapshot_stock.ask_volume[8], 
                dataCash.m_snapshot_stock.ask_price[9], dataCash.m_snapshot_stock.ask_volume[9], 
                dataCash.m_snapshot_stock.timestamp, dataCash.m_snapshot_stock.time_str, dataCash.m_snapshot_stock.exchange
            );
#endif
            break;
        }
        case TI_QUOTE_DATA_TYPE_MATCH:{
            printf("%s, %d, %d, %c, %c, %f, %d, %d, %d, %d, %d, %ld, %s, %s \n", 
                dataCash.m_matches.symbol, dataCash.m_matches.date, dataCash.m_matches.time, dataCash.m_matches.function_code, dataCash.m_matches.bs_flag,
                dataCash.m_matches.price, dataCash.m_matches.volume, dataCash.m_matches.ask_order_seq, dataCash.m_matches.bid_order_seq,
                dataCash.m_matches.channel, dataCash.m_matches.seq, dataCash.m_matches.timestamp, dataCash.m_matches.time_str, dataCash.m_matches.exchange
            );
            break;
        }
        case TI_QUOTE_DATA_TYPE_ORDER:{
            printf("%s, %d, %d, %c, %c, %f, %d, %d, %d, %ld, %s, %s \n", 
                dataCash.m_order.symbol, dataCash.m_order.date, dataCash.m_order.time, dataCash.m_order.function_code, dataCash.m_order.order_type,
                dataCash.m_order.price, dataCash.m_order.volume,
                dataCash.m_order.channel, dataCash.m_order.seq, dataCash.m_order.timestamp, dataCash.m_order.time_str, dataCash.m_order.exchange
            );
            break;
        }
        default:
            break;
        }
    }
    
    
    long long _print_end = datetime::get_now_timestamp_ms();
    std::cout << "print used time: " << _print_end - _start << std::endl;
}