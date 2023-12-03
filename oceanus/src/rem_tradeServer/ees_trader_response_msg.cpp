#include "ees_trader_response_msg.h"

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_AccountInfo* o){
    cJSON_AddStringToObject(j, "m_Account", o->m_Account);
    cJSON_AddNumberToObject(j, "m_Previlege", o->m_Previlege);
    cJSON_AddNumberToObject(j, "m_InitialBp", o->m_InitialBp);
    cJSON_AddNumberToObject(j, "m_AvailableBp", o->m_AvailableBp);
    cJSON_AddNumberToObject(j, "m_Margin", o->m_Margin);
    cJSON_AddNumberToObject(j, "m_FrozenMargin", o->m_FrozenMargin);
    cJSON_AddNumberToObject(j, "m_CommissionFee", o->m_CommissionFee);
    cJSON_AddNumberToObject(j, "m_FrozenCommission", o->m_FrozenCommission);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, const char* pAccount, EES_AccountPosition* o){
    cJSON_AddStringToObject(j, "m_Account", pAccount);
    cJSON_AddStringToObject(j, "m_ActId", o->m_actId);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_Symbol);
    cJSON_AddNumberToObject(j, "m_InitOvnQty", o->m_InitOvnQty);
    cJSON_AddNumberToObject(j, "m_OvnQty", o->m_OvnQty);
    cJSON_AddNumberToObject(j, "m_FrozenOvnQty", o->m_FrozenOvnQty);
    cJSON_AddNumberToObject(j, "m_TodayQty", o->m_TodayQty);
    cJSON_AddNumberToObject(j, "m_FrozenTodayQty", o->m_FrozenTodayQty);
    cJSON_AddNumberToObject(j, "m_OvnMargin", o->m_OvnMargin);
    cJSON_AddNumberToObject(j, "m_TodayMargin", o->m_TodayMargin);
    cJSON_AddNumberToObject(j, "m_PositionCost", o->m_PositionCost);
    cJSON_AddNumberToObject(j, "m_HedgeFlag", o->m_HedgeFlag);
};

 void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, const char* pAccount, EES_AccountOptionPosition* o){
    cJSON_AddStringToObject(j, "m_Account", pAccount);
    cJSON_AddStringToObject(j, "m_ActId", o->m_actId);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_Symbol);
    cJSON_AddNumberToObject(j, "m_PosiDirection", o->m_PosiDirection);
    cJSON_AddStringToObject(j, "m_UnderlyingSymbol", o->m_UnderlyingSymbol);
    cJSON_AddNumberToObject(j, "m_CallPut", o->m_CallPut);
    cJSON_AddNumberToObject(j, "m_StrikePrice", o->m_StrikePrice);
    cJSON_AddNumberToObject(j, "m_ExpireDate", o->m_ExpireDate);
    cJSON_AddNumberToObject(j, "m_InitOvnQty", o->m_InitOvnQty);
    cJSON_AddNumberToObject(j, "m_CurOvnQty", o->m_CurOvnQty);
    cJSON_AddNumberToObject(j, "m_CurTodayQty", o->m_CurTodayQty);
    cJSON_AddNumberToObject(j, "m_CoverOvnLockedQty", o->m_CoverOvnLockedQty);
    cJSON_AddNumberToObject(j, "m_CoverTodayLockedQty", o->m_CoverTodayLockedQty);
    cJSON_AddNumberToObject(j, "m_ExecPendingQty", o->m_ExecPendingQty);
    cJSON_AddNumberToObject(j, "m_ExecAppliedQty", o->m_ExecAppliedQty);
    cJSON_AddNumberToObject(j, "m_CxlExecPendingQty", o->m_CxlExecPendingQty);
    cJSON_AddNumberToObject(j, "m_LiquidPl", o->m_LiquidPl);
    cJSON_AddNumberToObject(j, "m_AvgPrice", o->m_AvgPrice);
    cJSON_AddNumberToObject(j, "m_TotalCommissionFee", o->m_TotalCommissionFee);
    cJSON_AddNumberToObject(j, "m_HedgeFlag", o->m_HedgeFlag);
 };

 void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, const char* pAccount, EES_AccountBP* o){
    cJSON_AddStringToObject(j, "m_Account", pAccount);
    cJSON_AddNumberToObject(j, "m_InitialBp", o->m_InitialBp);
    cJSON_AddNumberToObject(j, "m_AvailableBp", o->m_AvailableBp);
    cJSON_AddNumberToObject(j, "m_Margin", o->m_Margin);
    cJSON_AddNumberToObject(j, "m_FrozenMargin", o->m_FrozenMargin);
    cJSON_AddNumberToObject(j, "m_CommissionFee", o->m_CommissionFee);
    cJSON_AddNumberToObject(j, "m_FrozenCommission", o->m_FrozenCommission);
    cJSON_AddNumberToObject(j, "m_OvnInitMargin", o->m_OvnInitMargin);
    cJSON_AddNumberToObject(j, "m_TotalLiquidPL", o->m_TotalLiquidPL);
    cJSON_AddNumberToObject(j, "m_TotalMarketPL", o->m_TotalMarketPL);
 };

 void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_SymbolField* o){
    cJSON_AddNumberToObject(j, "m_SecType", o->m_SecType);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_symbol);
    cJSON_AddStringToObject(j, "m_SymbolName", o->m_symbolName);
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddStringToObject(j, "m_ProdId", o->m_ProdID);
    cJSON_AddNumberToObject(j, "m_DeliveryYear", o->m_DeliveryYear);
    cJSON_AddNumberToObject(j, "m_DeliveryMonth", o->m_DeliveryMonth);
    cJSON_AddNumberToObject(j, "m_MaxMarketOrderVolume", o->m_MaxMarketOrderVolume);
    cJSON_AddNumberToObject(j, "m_MinMarketOrderVolume", o->m_MinMarketOrderVolume);
    cJSON_AddNumberToObject(j, "m_MaxLimitOrderVolume", o->m_MaxLimitOrderVolume);
    cJSON_AddNumberToObject(j, "m_MinLimitOrderVolume", o->m_MinLimitOrderVolume);
    cJSON_AddNumberToObject(j, "m_VolumeMultiple", o->m_VolumeMultiple);
    cJSON_AddNumberToObject(j, "m_PriceTick", o->m_PriceTick);
    cJSON_AddNumberToObject(j, "m_CreateDate", o->m_CreateDate);
    cJSON_AddNumberToObject(j, "m_OpenDate", o->m_OpenDate);
    cJSON_AddNumberToObject(j, "m_ExpireDate", o->m_ExpireDate);
    cJSON_AddNumberToObject(j, "m_StartDelivDate", o->m_StartDelivDate);
    cJSON_AddNumberToObject(j, "m_EndDelivDate", o->m_EndDelivDate);
    cJSON_AddNumberToObject(j, "m_InstLifePhase", o->m_InstLifePhase);
    cJSON_AddNumberToObject(j, "m_IsTrading", o->m_IsTrading);
    cJSON_AddNumberToObject(j, "m_StrikePrice", o->m_StrikePrice);
    cJSON_AddNumberToObject(j, "m_CallPut", o->m_CallPut);
    cJSON_AddStringToObject(j, "m_UnderlyingSymbol", o->m_UnderlyingSymbol);
 };
 
void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, const char* pAccount, EES_AccountMargin* o){
    cJSON_AddStringToObject(j, "m_Account", pAccount);
    cJSON_AddNumberToObject(j, "m_HedgeFlag", o->m_HedgeFlag);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_symbol);
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddStringToObject(j, "m_ProdId", o->m_ProdID);
    cJSON_AddNumberToObject(j, "m_LongMarginRatio", o->m_LongMarginRatio);
    cJSON_AddNumberToObject(j, "m_ShortMarginRatio", o->m_ShortMarginRatio);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, const char* pAccount, EES_AccountFee* o){
    cJSON_AddStringToObject(j, "m_Account", pAccount);
    cJSON_AddNumberToObject(j, "m_HedgeFlag", o->m_HedgeFlag);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_symbol);
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddStringToObject(j, "m_ProdId", o->m_ProdID);
    cJSON_AddNumberToObject(j, "m_OpenRatioByMoney", o->m_OpenRatioByMoney);
    cJSON_AddNumberToObject(j, "m_OpenRatioByVolume", o->m_OpenRatioByVolume);
    cJSON_AddNumberToObject(j, "m_CloseYesterdayRatioByMoney", o->m_CloseYesterdayRatioByMoney);
    cJSON_AddNumberToObject(j, "m_CloseYesterdayRatioByVolume", o->m_CloseYesterdayRatioByVolume);
    cJSON_AddNumberToObject(j, "m_CloseTodayRatioByMoney", o->m_CloseTodayRatioByMoney);
    cJSON_AddNumberToObject(j, "m_CloseTodayRatioByVolume", o->m_CloseTodayRatioByVolume);
    cJSON_AddNumberToObject(j, "m_PositionDir", o->m_PositionDir);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_OrderAcceptField* o){
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_OrderState", o->m_OrderState);
    cJSON_AddNumberToObject(j, "m_UserId", o->m_UserID);
    cJSON_AddNumberToObject(j, "m_AcceptTime", o->m_AcceptTime);
    cJSON_AddStringToObject(j, "m_Account", o->m_Account);
    cJSON_AddNumberToObject(j, "m_Side", o->m_Side);
    cJSON_AddNumberToObject(j, "m_Exchange", o->m_Exchange);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_Symbol);
    cJSON_AddNumberToObject(j, "m_SecType", o->m_SecType);
    cJSON_AddNumberToObject(j, "m_Price", o->m_Price);
    cJSON_AddNumberToObject(j, "m_Qty", o->m_Qty);
    cJSON_AddNumberToObject(j, "m_ForceCloseReason", o->m_ForceCloseReason);
    cJSON_AddNumberToObject(j, "m_Tif", o->m_Tif);
    cJSON_AddNumberToObject(j, "m_MinQty", o->m_MinQty);
    cJSON_AddNumberToObject(j, "m_CustomField", o->m_CustomField);
    cJSON_AddNumberToObject(j, "m_MarketSessionId", o->m_MarketSessionId);
    cJSON_AddNumberToObject(j, "m_HedgeFlag", o->m_HedgeFlag);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_OrderMarketAcceptField* o){
    cJSON_AddStringToObject(j, "m_Account", o->m_Account);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddStringToObject(j, "m_MarketOrderId", o->m_MarketOrderId);
    cJSON_AddNumberToObject(j, "m_MarketTime", o->m_MarketTime);
    cJSON_AddNumberToObject(j, "m_UserId", o->m_UserID);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_ExchangeStatus", o->m_ExchangeStatus);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_OrderRejectField* o){
    cJSON_AddNumberToObject(j, "m_UserId", o->m_Userid);
    cJSON_AddNumberToObject(j, "m_Timestamp", o->m_Timestamp);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_RejectedMan", o->m_RejectedMan);
    cJSON_AddNumberToObject(j, "m_ReasonCode", o->m_ReasonCode);
    cJSON_AddStringToObject(j, "m_GrammerResult", o->m_GrammerResult);
    cJSON_AddStringToObject(j, "m_RiskResult", o->m_RiskResult);
    cJSON_AddStringToObject(j, "m_GrammerText", o->m_GrammerText);
    cJSON_AddStringToObject(j, "m_RiskText", o->m_RiskText);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_OrderMarketRejectField* o){
    cJSON_AddStringToObject(j, "m_Account", o->m_Account);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_MarketTimestamp", o->m_MarketTimestamp);
    cJSON_AddStringToObject(j, "m_ReasonText", o->m_ReasonText);
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeId);
    cJSON_AddNumberToObject(j, "m_ExchangeErrorId", o->m_ExchangeErrorId);
    cJSON_AddNumberToObject(j, "m_UserId", o->m_UserID);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_OrderExecutionField* o){
    cJSON_AddNumberToObject(j, "m_UserId", o->m_Userid);
    cJSON_AddNumberToObject(j, "m_Timestamp", o->m_Timestamp);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_Quantity", o->m_Quantity);
    cJSON_AddNumberToObject(j, "m_Price", o->m_Price);
    cJSON_AddNumberToObject(j, "m_ExecutionId", o->m_ExecutionID);
    cJSON_AddStringToObject(j, "m_MarketExecId", o->m_MarketExecID);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_OrderCxled* o){
    cJSON_AddNumberToObject(j, "m_UserId", o->m_Userid);
    cJSON_AddNumberToObject(j, "m_Timestamp", o->m_Timestamp);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_Decrement", o->m_Decrement);
    cJSON_AddNumberToObject(j, "m_Reason", o->m_Reason);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_CxlOrderRej* o){
    cJSON_AddStringToObject(j, "m_Account", o->m_account);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_ReasonCode", o->m_ReasonCode);
    cJSON_AddStringToObject(j, "m_ReasonText", o->m_ReasonText);
    cJSON_AddNumberToObject(j, "m_UserId", o->m_UserID);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddNumberToObject(j, "m_ActionFlag", o->m_ActionFlag);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, const char* pAccount, EES_QueryAccountOrder* o){
    cJSON_AddStringToObject(j, "m_Account", pAccount);
    cJSON_AddNumberToObject(j, "m_UserId", o->m_Userid);
    cJSON_AddNumberToObject(j, "m_Timestamp", o->m_Timestamp);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_SideType", o->m_SideType);
    cJSON_AddNumberToObject(j, "m_Quantity", o->m_Quantity);
    cJSON_AddNumberToObject(j, "m_InstrumentType", o->m_InstrumentType);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_symbol);
    cJSON_AddNumberToObject(j, "m_Price", o->m_Price);
    cJSON_AddStringToObject(j, "m_Account", o->m_account);
    cJSON_AddNumberToObject(j, "m_ExchengeId", o->m_ExchengeID);
    cJSON_AddNumberToObject(j, "m_ForceCloseReason", o->m_ForceCloseReason);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_OrderStatus", o->m_OrderStatus);
    cJSON_AddNumberToObject(j, "m_CloseTime", o->m_CloseTime);
    cJSON_AddNumberToObject(j, "m_FilledQty", o->m_FilledQty);
    cJSON_AddNumberToObject(j, "m_Tif", o->m_Tif);
    cJSON_AddNumberToObject(j, "m_MinQty", o->m_MinQty);
    cJSON_AddNumberToObject(j, "m_CustomField", o->m_CustomField);
    cJSON_AddStringToObject(j, "m_MarketOrderId", o->m_MarketOrderId);
    cJSON_AddNumberToObject(j, "m_HedgeFlag", o->m_HedgeFlag);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, const char* pAccount, EES_QueryOrderExecution* o){
    cJSON_AddStringToObject(j, "m_Account", pAccount);
    cJSON_AddNumberToObject(j, "m_UserId", o->m_Userid);
    cJSON_AddNumberToObject(j, "m_Timestamp", o->m_Timestamp);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_ExecutedQuantity", o->m_ExecutedQuantity);
    cJSON_AddNumberToObject(j, "m_ExecutionPrice", o->m_ExecutionPrice);
    cJSON_AddNumberToObject(j, "m_ExecutionId", o->m_ExecutionID);
    cJSON_AddStringToObject(j, "m_MarketExecId", o->m_MarketExecID);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_PostOrder* o){
    cJSON_AddNumberToObject(j, "m_UserId", o->m_Userid);
    cJSON_AddNumberToObject(j, "m_Timestamp", o->m_Timestamp);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_ClientOrderToken", o->m_ClientOrderToken);
    cJSON_AddNumberToObject(j, "m_SideType", o->m_SideType);
    cJSON_AddNumberToObject(j, "m_Quantity", o->m_Quantity);
    cJSON_AddNumberToObject(j, "m_SecType", o->m_SecType);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_Symbol);
    cJSON_AddNumberToObject(j, "m_Price", o->m_price);
    cJSON_AddStringToObject(j, "m_Account", o->m_account);
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddNumberToObject(j, "m_ForceCloseReason", o->m_ForceCloseReason);
    cJSON_AddNumberToObject(j, "m_OrderState", o->m_OrderState);
    cJSON_AddStringToObject(j, "m_ExchangeOrderId", o->m_ExchangeOrderID);
    cJSON_AddNumberToObject(j, "m_HedgeFlag", o->m_HedgeFlag);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_PostOrderExecution* o){
    cJSON_AddNumberToObject(j, "m_UserId", o->m_Userid);
    cJSON_AddNumberToObject(j, "m_Timestamp", o->m_Timestamp);
    cJSON_AddNumberToObject(j, "m_MarketOrderToken", o->m_MarketOrderToken);
    cJSON_AddNumberToObject(j, "m_ExecutedQuantity", o->m_ExecutedQuantity);
    cJSON_AddNumberToObject(j, "m_ExecutionPrice", o->m_ExecutionPrice);
    cJSON_AddNumberToObject(j, "m_ExecutionNumber", o->m_ExecutionNumber);
    cJSON_AddStringToObject(j, "m_MarketExecId", o->m_MarketExecID);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_ExchangeMarketSession* o){
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddNumberToObject(j, "m_SessionCount", o->m_SessionCount);
    //cJSON_AddStringToObject(j, "m_SessionId", std::string(o->m_SessionId).c_str());
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_SymbolStatus* o){
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_Symbol);
    cJSON_AddNumberToObject(j, "m_InstrumentStatus", o->m_InstrumentStatus);
    cJSON_AddNumberToObject(j, "m_TradingSegmentSN", o->m_TradingSegmentSN);
    cJSON_AddStringToObject(j, "m_EnterTime", o->m_EnterTime);
    cJSON_AddNumberToObject(j, "m_EnterReason", o->m_EnterReason);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_MarketMBLData* o){
    cJSON_AddNumberToObject(j, "m_RequestId", o->m_RequestId);
    cJSON_AddNumberToObject(j, "m_Result", o->m_Result);
    cJSON_AddStringToObject(j, "m_Symbol", o->m_symbol);
    cJSON_AddNumberToObject(j, "m_ExchangeId", o->m_ExchangeID);
    cJSON_AddNumberToObject(j, "m_Price", o->m_Price);
    cJSON_AddNumberToObject(j, "m_Volume", o->m_Volume);
    cJSON_AddNumberToObject(j, "m_IsBid", o->m_IsBid);
};

void EES_TRADER_RESPONSE_MSG::to_json(cJSON *j, EES_SessionID* o){
    cJSON_AddStringToObject(j, "m_TradeNo", o->m_TradeNo);
    cJSON_AddNumberToObject(j, "m_Result", o->m_Result);
    cJSON_AddNumberToObject(j, "m_SessionId", o->m_SessionID);
};