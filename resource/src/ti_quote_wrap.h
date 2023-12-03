#ifndef TI_QUOTE_WRAP_H
#define TI_QUOTE_WRAP_H

#include "ti_quote_struct.h"

namespace TiQuoteWrap
{
    char* CreatWrap(const TiQuoteSnapshotFutureField* pData);
    char* CreatWrap(const TiQuoteSnapshotStockField* pData);
    char* CreatWrap(const TiQuoteOrderField* pData);
    char* CreatWrap(const TiQuoteMatchesField* pData);
    char* CreatWrap(const TiQuoteOrderBookField* pData);

    TI_QUOTE_DATA_TYPE GetDataType(char* buff);
    TiQuoteSnapshotFutureField* GetSnapshotFutureData(char* buff);
    TiQuoteSnapshotStockField* GetSnapshotStockData(char* buff);
    TiQuoteOrderField* GetOrderData(char* buff);
    TiQuoteMatchesField* GetMatchesData(char* buff);
    TiQuoteOrderBookField* GetOrderBookData(char* buff);
}
#endif 