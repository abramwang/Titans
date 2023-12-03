#ifndef TI_QUOTE_TOOLS_H
#define TI_QUOTE_TOOLS_H

#include "ti_quote_struct.h"
#include <string.h>

namespace TiQuoteTools
{
    inline int64_t GetSymbolID(const TI_ExchangeType exchange, const TI_SymbolType symbol)
    {
        int64_t id = 0;
        memcpy(&id, exchange, 2);
        memcpy((char*)&id+2, symbol, 6);
        return id;
    }
}

#endif