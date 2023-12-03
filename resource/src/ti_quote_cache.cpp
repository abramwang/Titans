#include <iostream>
#include <string.h>
#include "ti_quote_cache.h"

TiQuoteCache::TiQuoteCache(void* data, size_t len)
{
    m_buffer = data;
    m_len = len;
    m_used = 0;
    m_dataPtr = NULL;
};

TiQuoteCache::TiQuoteCache()
{
    m_buffer = NULL;
    m_len = 0;
    m_used = 0;
    m_dataPtr = NULL;
};


bool TiQuoteCache::try_getNext(DataPtr* &dataPtr)
{
    if (!m_buffer)
    {
        return false;
    }
    if (!m_dataPtr)
    {
        return false;
    }
    if (m_dataPtr->next)
    {
        m_dataPtr->next = (DataPtr*)((char*)m_dataPtr + sizeof(DataPtr) + m_dataPtr->len);
    }
    m_dataPtr->data = (DataPtr*)((char*)m_dataPtr + sizeof(DataPtr));
    dataPtr = m_dataPtr;
    m_dataPtr = m_dataPtr->next;
    return true;
};

bool TiQuoteCache::try_addData(TI_QUOTE_DATA_TYPE type, void* data, size_t len)
{
    if (!m_buffer)
    {
        return false;
    }
    if (!data)
    {
        return false;
    }
    if (len > m_len)
    {
        return false;
    }
    if ((m_used + sizeof(DataPtr) + len) > m_len)
    {
        return false;
    }
    if (!m_dataPtr)
    {
        m_dataPtr = (DataPtr*)m_buffer;
        m_dataPtr->dataType = type;
        m_dataPtr->len = len;
        m_dataPtr->data = (void*)((char*)m_dataPtr + sizeof(DataPtr));
        memcpy(m_dataPtr->data, data, len);
        m_dataPtr->next = NULL;
        m_used += sizeof(DataPtr) + len;
        return true;
    }
    while (m_dataPtr->next)
    {
        m_dataPtr = m_dataPtr->next;
    }
    m_dataPtr->next = (DataPtr*)((char*)m_dataPtr + sizeof(DataPtr) + m_dataPtr->len);
    DataPtr* ptr = m_dataPtr->next;
    ptr->dataType = type;
    ptr->len = len;
    ptr->data = (void*)((char*)ptr + sizeof(DataPtr));
    memcpy(ptr->data, data, len);
    ptr->next = NULL;
    m_used += sizeof(DataPtr) + len;
    m_dataPtr = ptr;
    return true;
};

void TiQuoteCache::reset()
{
    m_used = 0;
    m_dataPtr = NULL;
};

void TiQuoteCache::reset_buffer(void* data, size_t len)
{
    m_buffer = data;
    m_len = len;
    m_used = 0;
    m_dataPtr = (DataPtr*)m_buffer;
};

bool TiQuoteCache::isFull(size_t len)
{
    return ((m_used + sizeof(DataPtr) + len) > m_len);
};