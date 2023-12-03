#include "ti_trader_formater.h"
#include "datetime.h"

void TiTraderFormater::FormatOrderDelete(const TiRspOrderDelete* pData, json& j)
{

};
void TiTraderFormater::FormatPosition(const TiRspQryPosition* pData, json& j)
{

};
void TiTraderFormater::FormatOrderStatus(const TiRtnOrderStatus* pData, json& j)
{
    char order_id[128];
    sprintf(order_id, "%lu", pData->nOrderId);
    j ={
        {"nReqId" , pData->nReqId},
        {"szSymbol" , pData->szSymbol},
        {"szExchange" , pData->szExchange},
        {"nTradeSideType" , pData->nTradeSideType},
        {"nOffsetType" , pData->nOffsetType},
        {"nBusinessType" , pData->nBusinessType},
        {"nOrderPrice" , pData->nOrderPrice},
        {"nOrderVol" , pData->nOrderVol},
        {"szReqTimestamp" , pData->nReqTimestamp ? datetime::get_format_timestamp_ms(pData->nReqTimestamp):""},
        {"szOrderId" , order_id},
        {"nSubmitVol" , pData->nSubmitVol},
        {"nDealtPrice" , pData->nDealtPrice},
        {"nDealtVol" , pData->nDealtVol},
        {"nTotalWithDrawnVol" , pData->nTotalWithDrawnVol},
        {"nInValid" , pData->nInValid},
        {"nStatus" , pData->nStatus},
        {"szInsertTime" , datetime::get_format_timestamp_ms(pData->nInsertTimestamp)},
        {"szLastUpdateTime" , datetime::get_format_timestamp_ms(pData->nLastUpdateTimestamp)},
        {"szUsedTime" , datetime::get_format_time_duration_ms(pData->nUsedTime)},
        {"nFee" , pData->nFee},
        {"szErr" , pData->szErr},
    };
};
void TiTraderFormater::FormatOrderMatchEvent(const TiRtnOrderMatch* pData, json& j)
{
    char order_id[128];
    sprintf(order_id, "%lu", pData->nOrderId);
    j = {
        {"szOrderId" , order_id},
        {"szStreamId" , pData->szStreamId},
        {"nMatchPrice" , pData->nMatchPrice},
        {"nMatchVol" , pData->nMatchVol},
        {"szSymbol" , pData->szSymbol},
        {"szExchange" , pData->szExchange},
        {"szMatchTime" , datetime::get_format_timestamp_ms(pData->nMatchTimestamp)},
        {"nTradeSideType" , pData->nTradeSideType},
    };
};