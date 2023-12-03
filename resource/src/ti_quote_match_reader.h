#ifndef __TI_QUOTE_MATCH_READER_H__
#define __TI_QUOTE_MATCH_READER_H__

#include <set>
#include <list>
#include "ti_quote_parquet_reader.h"
#include "ti_quote_struct.h"

class TiQuoteMatchReader : 
    public TiQuoteParquetReader
{
private:
    std::unique_ptr<parquet::arrow::FileReader> m_reader;
    std::shared_ptr<arrow::Schema> m_file_schema;
    std::set<std::string> m_field_set;
    TiQuoteMatchesField m_data;
    int64_t nIndex;

    std::set<std::string> m_symbol_set; //筛选的symbol

    std::shared_ptr<arrow::Int64Array>     m_date_array;           //
    std::shared_ptr<arrow::Int64Array>     m_time_array;           //必须要有
    std::shared_ptr<arrow::StringArray>    m_function_code_array;  //
    std::shared_ptr<arrow::StringArray>    m_bs_flag_array;        //
    std::shared_ptr<arrow::DoubleArray>    m_price_array;          //
    std::shared_ptr<arrow::Int64Array>     m_volume_array;         //
    std::shared_ptr<arrow::Int64Array>     m_ask_order_seq_array;  //
    std::shared_ptr<arrow::Int64Array>     m_bid_order_seq_array;  //
    std::shared_ptr<arrow::Int64Array>     m_channel_array;        //
    std::shared_ptr<arrow::Int64Array>     m_seq_array;            //
    std::shared_ptr<arrow::StringArray>    m_symbol_array;         //
    std::shared_ptr<arrow::TimestampArray> m_timestamp_array;      //
    std::shared_ptr<arrow::StringArray>    m_exchange_array;       //
private:
    void initField(char* field_list[], size_t len);
    void loadArray(std::string file_path);
public:
    void SetQrySymbol(std::list<std::string> symbol_list);
    void SetTime(int hour, int min, int sec, int ms);   // 小时，分，秒，毫秒
    int64_t GetTime();
    bool GetNext(TiQuoteMatchesField* out);             // true: has next, false: is last;

    TiQuoteMatchReader(const char* path, char* field_list[], size_t len);
    virtual ~TiQuoteMatchReader(){};
};

#endif