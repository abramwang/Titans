#ifndef TI_QUOTE_DEPTH_CALLBACK_H
#define TI_QUOTE_DEPTH_CALLBACK_H

#include "ti_quote_struct.h"
#include <vector>

class TiQuoteDepthCallback
{
public:
    virtual void OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
        const std::vector<TiQuoteDepthLevel*> &asks, 
        const std::vector<TiQuoteDepthLevel*> &bids) = 0;
    
    virtual void OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData) = 0;

public:
    TiQuoteDepthCallback(){};
    virtual ~TiQuoteDepthCallback(){};
};

#endif 