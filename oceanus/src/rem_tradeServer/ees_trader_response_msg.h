#ifndef __EES_TRADER_RESPONSE_MSG_H__
#define __EES_TRADER_RESPONSE_MSG_H__

#include "EesTraderDefine.h"
#include "cJSON.h"

namespace EES_TRADER_RESPONSE_MSG {
    //OnQueryUserAccount
    void to_json(cJSON *j, EES_AccountInfo* o);
    //OnQueryAccountPosition
    void to_json(cJSON *j, const char* pAccount, EES_AccountPosition* o);
    //OnQueryAccountOptionPosition
    void to_json(cJSON *j, const char* pAccount, EES_AccountOptionPosition* o);
    //OnQueryAccountOptionPosition
    void to_json(cJSON *j, const char* pAccount, EES_AccountOptionPosition* o);
    //OnQueryAccountBP & OnAccountBPReport
    void to_json(cJSON *j, const char* pAccount, EES_AccountBP* o);
    //OnQuerySymbol
    void to_json(cJSON *j, EES_SymbolField* o);
    //OnQueryAccountTradeMargin
    void to_json(cJSON *j, const char* pAccount, EES_AccountMargin* o);
    //OnQueryAccountTradeFee
    void to_json(cJSON *j, const char* pAccount, EES_AccountFee* o);
    //OnOrderAccept
    void to_json(cJSON *j, EES_OrderAcceptField* o);
    //OnOrderMarketAccept
    void to_json(cJSON *j, EES_OrderMarketAcceptField* o);
    //OnOrderReject
    void to_json(cJSON *j, EES_OrderRejectField* o);
    //OnOrderMarketReject
    void to_json(cJSON *j, EES_OrderMarketRejectField* o);
    //OnOrderExecution
    void to_json(cJSON *j, EES_OrderExecutionField* o);
    //OnOrderCxled
    void to_json(cJSON *j, EES_OrderCxled* o);
    //OnCxlOrderReject
    void to_json(cJSON *j, EES_CxlOrderRej* o);
    //OnQueryTradeOrder
    void to_json(cJSON *j, const char* pAccount, EES_QueryAccountOrder* o);
    //OnQueryTradeOrderExec
    void to_json(cJSON *j, const char* pAccount, EES_QueryOrderExecution* o);
    //OnPostOrder
    void to_json(cJSON *j, EES_PostOrder* o);
    //OnPostOrderExecution
    void to_json(cJSON *j, EES_PostOrderExecution* o);
    //OnQueryMarketSession
    void to_json(cJSON *j, EES_ExchangeMarketSession* o);
    //OnSymbolStatusReport & OnQuerySymbolStatus
    void to_json(cJSON *j, EES_SymbolStatus* o);
    //OnQueryMarketMBLData
    void to_json(cJSON *j, EES_MarketMBLData* o);
    //OnQuerySessionID
    void to_json(cJSON *j, EES_SessionID* o);
};

#endif