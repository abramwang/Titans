#ifndef __TI_QUOTE_PARQUET_READER_H__
#define __TI_QUOTE_PARQUET_READER_H__

#include <arrow/api.h>
#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/arrow/writer.h>
#include <parquet/exception.h>
#include <uv.h>
#include <map>
#include <list>

#include "ti_quote_struct.h"

class TiQuoteParquetReader
{
public:
    struct ReadArrayWorkReqInfo
    { 
        std::string                   m_file_path;  //
        std::string                   m_field;      //
        std::shared_ptr<arrow::Array> m_array;      //
        TiQuoteParquetReader*         m_reader;     //
    };
    static void init_read_array_work(uv_work_t* req);
    static void after_read_array_work(uv_work_t* req, int status);
private:
    uv_loop_t m_read_loop;  //读取文件 event loop
protected:
    TI_QUOTE_DATA_TYPE m_DataType;
    std::map<std::string, std::shared_ptr<arrow::Array>> m_array_map;
    void loadAllArray(std::string file_path);
public:
    TI_QUOTE_DATA_TYPE GetDataType();
    virtual void SetQrySymbol(std::list<std::string> symbol_list){};
    virtual void SetTime(int hour, int min, int sec, int ms){}; 
    virtual int64_t GetTime();
public:
    TiQuoteParquetReader(TI_QUOTE_DATA_TYPE dataType);
    virtual ~TiQuoteParquetReader(){};

};

#endif