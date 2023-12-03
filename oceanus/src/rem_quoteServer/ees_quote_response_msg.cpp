#include "ees_quote_response_msg.h"

void EES_QUOTE_RESPONSE_MSG::to_json(cJSON *j, EESMarketDepthQuoteData* o){
    cJSON_AddStringToObject(j, "TradingDay", o->TradingDay);
    cJSON_AddStringToObject(j, "InstrumentID", o->InstrumentID);
    cJSON_AddStringToObject(j, "ExchangeID", o->ExchangeID);
    cJSON_AddStringToObject(j, "ExchangeInstID", o->ExchangeInstID);
    cJSON_AddNumberToObject(j, "LastPrice", (unsigned int)o->LastPrice);
    cJSON_AddNumberToObject(j, "PreSettlementPrice", (unsigned int)o->PreSettlementPrice);
    cJSON_AddNumberToObject(j, "PreClosePrice", (unsigned int)o->PreClosePrice);
    cJSON_AddNumberToObject(j, "PreOpenInterest", o->PreOpenInterest);
    cJSON_AddNumberToObject(j, "OpenPrice", (unsigned int)o->OpenPrice);
    cJSON_AddNumberToObject(j, "HighestPrice", (unsigned int)o->HighestPrice);
    cJSON_AddNumberToObject(j, "LowestPrice", (unsigned int)o->LowestPrice);
    cJSON_AddNumberToObject(j, "Volume", o->Volume);
    cJSON_AddNumberToObject(j, "Turnover", o->Turnover);
    cJSON_AddNumberToObject(j, "OpenInterest", o->OpenInterest);
    cJSON_AddNumberToObject(j, "ClosePrice", (unsigned int)o->ClosePrice);
    cJSON_AddNumberToObject(j, "SettlementPrice", (unsigned int)o->SettlementPrice);
    cJSON_AddNumberToObject(j, "UpperLimitPrice", (unsigned int)o->UpperLimitPrice);
    cJSON_AddNumberToObject(j, "LowerLimitPrice", (unsigned int)o->LowerLimitPrice);
    cJSON_AddNumberToObject(j, "PreDelta", (unsigned int)o->PreDelta);
    cJSON_AddNumberToObject(j, "CurrDelta", (unsigned int)o->CurrDelta);
    cJSON_AddStringToObject(j, "UpdateTime", o->UpdateTime);
    cJSON_AddNumberToObject(j, "UpdateMillisec", o->UpdateMillisec);

    cJSON* BidPrice = cJSON_CreateArray();
    cJSON_AddItemToArray(BidPrice, cJSON_CreateNumber((unsigned int)o->BidPrice1));
    cJSON_AddItemToArray(BidPrice, cJSON_CreateNumber((unsigned int)o->BidPrice2));
    cJSON_AddItemToArray(BidPrice, cJSON_CreateNumber((unsigned int)o->BidPrice3));
    cJSON_AddItemToArray(BidPrice, cJSON_CreateNumber((unsigned int)o->BidPrice4));
    cJSON_AddItemToArray(BidPrice, cJSON_CreateNumber((unsigned int)o->BidPrice5));
    cJSON* BidVolume = cJSON_CreateArray();
    cJSON_AddItemToArray(BidVolume, cJSON_CreateNumber(o->BidVolume1));
    cJSON_AddItemToArray(BidVolume, cJSON_CreateNumber(o->BidVolume2));
    cJSON_AddItemToArray(BidVolume, cJSON_CreateNumber(o->BidVolume3));
    cJSON_AddItemToArray(BidVolume, cJSON_CreateNumber(o->BidVolume4));
    cJSON_AddItemToArray(BidVolume, cJSON_CreateNumber(o->BidVolume5));
    cJSON* AskPrice = cJSON_CreateArray();
    cJSON_AddItemToArray(AskPrice, cJSON_CreateNumber((unsigned int)o->AskPrice1));
    cJSON_AddItemToArray(AskPrice, cJSON_CreateNumber((unsigned int)o->AskPrice2));
    cJSON_AddItemToArray(AskPrice, cJSON_CreateNumber((unsigned int)o->AskPrice3));
    cJSON_AddItemToArray(AskPrice, cJSON_CreateNumber((unsigned int)o->AskPrice4));
    cJSON_AddItemToArray(AskPrice, cJSON_CreateNumber((unsigned int)o->AskPrice5));
    cJSON* AskVolume = cJSON_CreateArray();
    cJSON_AddItemToArray(AskVolume, cJSON_CreateNumber(o->AskVolume1));
    cJSON_AddItemToArray(AskVolume, cJSON_CreateNumber(o->AskVolume2));
    cJSON_AddItemToArray(AskVolume, cJSON_CreateNumber(o->AskVolume3));
    cJSON_AddItemToArray(AskVolume, cJSON_CreateNumber(o->AskVolume4));
    cJSON_AddItemToArray(AskVolume, cJSON_CreateNumber(o->AskVolume5));

    cJSON_AddItemToObject(j, "BidPrice", BidPrice);
    cJSON_AddItemToObject(j, "BidVolume", BidVolume);
    cJSON_AddItemToObject(j, "AskPrice", AskPrice);
    cJSON_AddItemToObject(j, "AskVolume", AskVolume);

    cJSON_AddNumberToObject(j, "AveragePrice", (unsigned int)o->AveragePrice);
};
