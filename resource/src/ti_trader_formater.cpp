#include "ti_trader_formater.h"
#include "datetime.h"

void TiTraderFormater::FormatOrderDelete(const TiRspOrderDelete* pData, json& j)
{

};

void TiTraderFormater::FormatPosition(const TiRspQryPosition* pData, json& j)
{
    j = {
        {"szSymbol" , pData->szSymbol},
        {"szExchange" , pData->szExchange},
        {"szAccount" , pData->szAccount},
        {"nPosition" , pData->nPosition},
        {"nProfit" , pData->nProfit},
        {"nSettledProfit" , pData->nSettledProfit},
        {"szShareholderId" , pData->szShareholderId},
    };
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
        {"szOrderStreamId" , pData->szOrderStreamId},
        {"szShareholderId" , pData->szShareholderId},
        {"nSubmitVol" , pData->nSubmitVol},
        {"nDealtPrice" , pData->nDealtPrice},
        {"nDealtVol" , pData->nDealtVol},
        {"nTotalWithDrawnVol" , pData->nTotalWithDrawnVol},
        {"nInValid" , pData->nInValid},
        {"nStatus" , pData->nStatus},
        {"szInsertTime" , datetime::get_format_timestamp_ms(pData->nInsertTimestamp)},
        {"szLastUpdateTime" , datetime::get_format_timestamp_ms(pData->nLastUpdateTimestamp)},
        {"szUsedTime" , datetime::get_format_time_duration_ms(pData->nUsedTime)},
        {"szUseStr" , pData->szUseStr},
        {"nFee" , pData->nFee},
        {"szErr" , pData->szErr},
    };
    switch (pData->nStatus)
    {
    case TI_OrderStatusType_fail:
        j["szStatus"] = "fail";
        break;
    case TI_OrderStatusType_removed:
        j["szStatus"] = "removed";
        break;
    case TI_OrderStatusType_dealt:
        j["szStatus"] = "dealt";
        break;
    case TI_OrderStatusType_unAccept:
        j["szStatus"] = "unAccept";
        break;
    case TI_OrderStatusType_accepted:
        j["szStatus"] = "accepted";
        break;
    case TI_OrderStatusType_queued:
        j["szStatus"] = "queued";
        break;
    case TI_OrderStatusType_toRemove:
        j["szStatus"] = "toRemove";
        break;
    case TI_OrderStatusType_removing:
        j["szStatus"] = "removing";
        break;
    default:
        break;
    }
};

void TiTraderFormater::FormatOrderMatchEvent(const TiRtnOrderMatch* pData, json& j)
{
    char order_id[128];
    sprintf(order_id, "%lu", pData->nOrderId);
    j = {
        {"szOrderId" , order_id},
        {"szOrderStreamId" , pData->szOrderStreamId},
        {"szStreamId" , pData->szStreamId},
        {"nMatchPrice" , pData->nMatchPrice},
        {"nMatchVol" , pData->nMatchVol},
        {"szSymbol" , pData->szSymbol},
        {"szExchange" , pData->szExchange},
        {"szMatchTime" , datetime::get_format_timestamp_ms(pData->nMatchTimestamp)},
        {"nTradeSideType" , pData->nTradeSideType},
        {"szShareholderId" , pData->szShareholderId},
        {"szErr" , pData->szErr},
    };
};