#ifndef TI_QUOTE_CLIENT_H
#define TI_QUOTE_CLIENT_H

#include "ti_quote_struct.h"

class TiQuoteClient
{
public:
	virtual void connect() = 0;
    virtual void subData(const char* exchangeName, char* codeList[], size_t len) = 0;
    /* data */
public:
    TiQuoteClient(){};
    virtual ~TiQuoteClient(){};
};

#endif 