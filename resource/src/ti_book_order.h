#ifndef __TI_BOOK_ORDER_H__
#define __TI_BOOK_ORDER_H__

#include "ti_quote_struct.h"
#include <memory>

struct TiBookOrder : TiQuoteOrderField
{
    int32_t    dealt_vol;       //  预成交量
};

typedef std::shared_ptr<TiBookOrder> TiBookOrderPtr;

#endif