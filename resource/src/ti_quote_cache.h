#ifndef TI_QUOTE_CACHE_H
#define TI_QUOTE_CACHE_H

#include "ti_quote_struct.h"

const int TI_QUOTE_CACHE_MAX_LEN     = 0xFFFF;
typedef char TI_QuoteCacheBufferType[TI_QUOTE_CACHE_MAX_LEN];

class TiQuoteCache
{
public:
#pragma pack(1)
    struct DataPtr
    {
        TI_QUOTE_DATA_TYPE dataType;
        size_t len;
        void* data;
        DataPtr *next;
    };
#pragma pack()
public:
    void* m_buffer;
    size_t m_len;
    size_t m_used;
    DataPtr* m_dataPtr;

public:
    bool try_getNext(DataPtr* &dataPtr);

    bool try_addData(TI_QUOTE_DATA_TYPE type, void* data, size_t len);
    void reset();
    void reset_buffer(void* data, size_t len);
    bool isFull(size_t len);
    
public:
    TiQuoteCache(void* data, size_t len);
    TiQuoteCache();
    virtual ~TiQuoteCache(){};
};



#endif 