#include "ti_quote_wrap.h"
#include <string.h>


char* TiQuoteWrap::CreatWrap(const TiQuoteSnapshotIndexField* pData)
{
    if (!pData)
    {
        return NULL;
    }
    char* buff = new char[sizeof(TiQuoteSnapshotIndexField) + 1];
    buff[0] = TI_QUOTE_DATA_TYPE_SNAPSHOT_INDEX;
    memcpy(buff + 1, pData, sizeof(TiQuoteSnapshotIndexField));
    return buff;
};

char* TiQuoteWrap::CreatWrap(const TiQuoteSnapshotFutureField* pData)
{
    if (!pData)
    {
        return NULL;
    }
    char* buff = new char[sizeof(TiQuoteSnapshotFutureField) + 1];
    buff[0] = TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE;
    memcpy(buff + 1, pData, sizeof(TiQuoteSnapshotFutureField));
    return buff;
};

char* TiQuoteWrap::CreatWrap(const TiQuoteSnapshotStockField* pData)
{
    if (!pData)
    {
        return NULL;
    }
    char* buff = new char[sizeof(TiQuoteSnapshotStockField) + 1];
    buff[0] = TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK;
    memcpy(buff + 1, pData, sizeof(TiQuoteSnapshotStockField));
    return buff;
};

char* TiQuoteWrap::CreatWrap(const TiQuoteOrderField* pData)
{
    if (!pData)
    {
        return NULL;
    }
    char* buff = new char[sizeof(TiQuoteOrderField) + 1];
    buff[0] = TI_QUOTE_DATA_TYPE_ORDER;
    memcpy(buff + 1, pData, sizeof(TiQuoteOrderField));
    return buff;
};

char* TiQuoteWrap::CreatWrap(const TiQuoteMatchesField* pData)
{
    if (!pData)
    {
        return NULL;
    }
    char* buff = new char[sizeof(TiQuoteMatchesField) + 1];
    buff[0] = TI_QUOTE_DATA_TYPE_MATCH;
    memcpy(buff + 1, pData, sizeof(TiQuoteMatchesField));
    return buff;
};

char* TiQuoteWrap::CreatWrap(const TiQuoteOrderBookField* pData)
{
    if (!pData)
    {
        return NULL;
    }
    char* buff = new char[sizeof(TiQuoteOrderBookField) + 1];
    buff[0] = TI_QUOTE_DATA_TYPE_ORDERBOOK_LEVEL;
    memcpy(buff + 1, pData, sizeof(TiQuoteOrderBookField));
    return buff;
};

TI_QUOTE_DATA_TYPE TiQuoteWrap::GetDataType(char* buff)
{
    if (!buff)
    {
        return TI_QUOTE_DATA_TYPE_NONE;
    }
    return buff[0];
}

TiQuoteSnapshotIndexField* TiQuoteWrap::GetSnapshotIndexData(char* buff)
{
    if (!buff)
    {
        return NULL;
    }
    if (buff[0] != TI_QUOTE_DATA_TYPE_SNAPSHOT_INDEX)
    {
        return NULL;
    }
    return (TiQuoteSnapshotIndexField*)(buff + 1);
};

TiQuoteSnapshotFutureField* TiQuoteWrap::GetSnapshotFutureData(char* buff)
{
    if (!buff)
    {
        return NULL;
    }
    if (buff[0] != TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE)
    {
        return NULL;
    }
    return (TiQuoteSnapshotFutureField*)(buff + 1);
};

TiQuoteSnapshotStockField* TiQuoteWrap::GetSnapshotStockData(char* buff)
{
    if (!buff)
    {
        return NULL;
    }
    if (buff[0] != TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK)
    {
        return NULL;
    }
    return (TiQuoteSnapshotStockField*)(buff + 1);
}

TiQuoteOrderField* TiQuoteWrap::GetOrderData(char* buff)
{
    if (!buff)
    {
        return NULL;
    }
    if (buff[0] != TI_QUOTE_DATA_TYPE_ORDER)
    {
        return NULL;
    }
    return (TiQuoteOrderField*)(buff + 1);
}

TiQuoteMatchesField* TiQuoteWrap::GetMatchesData(char* buff)
{
    if (!buff)
    {
        return NULL;
    }
    if (buff[0] != TI_QUOTE_DATA_TYPE_MATCH)
    {
        return NULL;
    }
    return (TiQuoteMatchesField*)(buff + 1);
}

TiQuoteOrderBookField* TiQuoteWrap::GetOrderBookData(char* buff)
{
    if (!buff)
    {
        return NULL;
    }
    if (buff[0] != TI_QUOTE_DATA_TYPE_ORDERBOOK_LEVEL)
    {
        return NULL;
    }
    return (TiQuoteOrderBookField*)(buff + 1);
}