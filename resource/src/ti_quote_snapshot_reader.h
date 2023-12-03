#ifndef __TI_QUOTE_SNAPSHOT_READER_H__
#define __TI_QUOTE_SNAPSHOT_READER_H__

#include <set>
#include <list>
#include "ti_quote_parquet_reader.h"
#include "ti_quote_struct.h"

class TiQuoteSnapshotReader : 
    public TiQuoteParquetReader
{
private:
    std::unique_ptr<parquet::arrow::FileReader> m_reader;
    std::shared_ptr<arrow::Schema> m_file_schema;
    std::set<std::string> m_field_set;
    TiQuoteMatchesField m_data;
    int64_t nIndex;

    std::set<std::string> m_symbol_set; //筛选的symbol

    std::shared_ptr<arrow::Int64Array>     m_date_array;                               //
    std::shared_ptr<arrow::Int64Array>     m_time_array;                               //必须要有
    std::shared_ptr<arrow::DoubleArray>    m_last_array;                               //
    std::shared_ptr<arrow::Int64Array>     m_volume_array;                             //
    std::shared_ptr<arrow::Int64Array>     m_turnover_array;                           //
    std::shared_ptr<arrow::Int64Array>     m_match_items_array;                        //
    std::shared_ptr<arrow::Int64Array>     m_interest_array;                           //
    std::shared_ptr<arrow::Int64Array>     m_acc_volume_array;                         //
    std::shared_ptr<arrow::Int64Array>     m_acc_turnover_array;                       //
    std::shared_ptr<arrow::DoubleArray>    m_high_array;                               //
    std::shared_ptr<arrow::DoubleArray>    m_low_array;                                //
    std::shared_ptr<arrow::DoubleArray>    m_open_array;                               //
    std::shared_ptr<arrow::DoubleArray>    m_pre_close_array;                          //
    std::shared_ptr<arrow::DoubleArray>    m_ask_price_array[TI_STOCK_ARRAY_LEN];      //
    std::shared_ptr<arrow::DoubleArray>    m_bid_price_array[TI_STOCK_ARRAY_LEN];      //
    std::shared_ptr<arrow::Int64Array>     m_ask_order_num_array[TI_STOCK_ARRAY_LEN];  //
    std::shared_ptr<arrow::Int64Array>     m_bid_order_num_array[TI_STOCK_ARRAY_LEN];  //
    std::shared_ptr<arrow::Int64Array>     m_ask_volume_array[TI_STOCK_ARRAY_LEN];     //
    std::shared_ptr<arrow::Int64Array>     m_bid_volume_array[TI_STOCK_ARRAY_LEN];     //
    std::shared_ptr<arrow::DoubleArray>    m_wavg_ask_array;                           //
    std::shared_ptr<arrow::DoubleArray>    m_wavg_bid_array;                           //
    std::shared_ptr<arrow::Int64Array>     m_total_ask_qty_array;                      //
    std::shared_ptr<arrow::Int64Array>     m_total_bid_qty_array;                      //
    std::shared_ptr<arrow::StringArray>    m_symbol_array;                             //
    std::shared_ptr<arrow::TimestampArray> m_timestamp_array;                          //
    std::shared_ptr<arrow::StringArray>    m_exchange_array;                           //
private:
    void initField(char* field_list[], size_t len);
    void loadArray(std::string file_path);
public:
    void SetQrySymbol(std::list<std::string> symbol_list);
    void SetTime(int hour, int min, int sec, int ms);           // 小时，分，秒，毫秒
    int64_t GetTime();
    bool GetNext(TiQuoteSnapshotStockField* out);               // true: has next, false: is last;

    TiQuoteSnapshotReader(const char* path, char* field_list[], size_t len);
    virtual ~TiQuoteSnapshotReader(){};
};

#endif