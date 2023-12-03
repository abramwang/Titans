#include "etf_constituent_cash.h"

ETFConstituentCash::ETFConstituentCash(const char* symbol, const char* exchange, double weight)
{
    strcpy(szSymbol, symbol);
    strcpy(szExchange, exchange);
    nWeight = weight;
    

    nLast = 0;
    nTimeStamp = 0;
    nFactor = 0;
}

ETFConstituentCash::~ETFConstituentCash()
{
}

void ETFConstituentCash::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    if(pData->volume && pData->price){
        nLast       = pData->price;
        nTimeStamp  = pData->timestamp;
        nFactor     = nLast * nWeight;
    }
};
