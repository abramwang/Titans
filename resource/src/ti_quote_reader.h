#ifndef __TI_QUOTE_READER_H__
#define __TI_QUOTE_READER_H__

#include "ti_quote_snapshot_reader.h"
#include "ti_quote_match_reader.h"
#include "ti_quote_order_reader.h"
#include <string.h>
#include <list>
#include <vector>
#include <uv.h>

class TiQuoteReader
{
public:
    struct ReadWorkReqInfo
    { 
        TI_QUOTE_DATA_TYPE     type;                //
        char                   szParquetPath[256];  //
        std::list<std::string> m_symbol_list;       //
        TiQuoteParquetReader*  m_file_reader;       //
        TiQuoteReader*         m_quote_reader;      //
    };
   
    static void init_reader_work(uv_work_t* req);
    static void after_reader_work(uv_work_t* req, int status);
private:
    uv_loop_t m_loop;  //读取文件 event loop
    std::string szRootPath;
protected:
    std::list<std::shared_ptr<TiQuoteParquetReader>> m_readers;
public:
    void ReadQuoteData(int dayNum, TI_ExchangeType ex, TI_QUOTE_DATA_TYPE datatype_list[], size_t datatype_len);
    void ReadQuoteData(int dayNum, TiSymbolInfo symbol_list[], size_t symbol_len, TI_QUOTE_DATA_TYPE datatype_list[], size_t datatype_len);
    bool GetNext(TiQuoteData* pData); 
public:
    TiQuoteReader(const char* rootPath);
    virtual ~TiQuoteReader(){};
};


#endif