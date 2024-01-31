#include "ti_hx_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"

#include <atomic>

TiHxTraderClient::TiHxTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_client = NULL;

    nSessionId = 0;
    nOrderId = 1680000;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 100;   //跳过xtp client设置成交模式的区段
};


TiHxTraderClient::~TiHxTraderClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    m_cb = NULL;
    nReqId = 0;
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////
///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
void TiHxTraderClient::OnFrontConnected(){
    LOG(INFO) << "[OnFrontConnected] ";
    std::cout << "[OnFrontConnected] " << std::endl;

    CTORATstpReqUserLoginField req;

    memset(&req, 0, sizeof(req));
    strcpy(req.LogInAccount, m_config->szUser.c_str());
    req.LogInAccountType = TORA_TSTP_LACT_UserID;

    strcpy(req.Password, m_config->szPass.c_str());
    strcpy(req.UserProductInfo, m_config->szUserProductInfo.c_str());
    strcpy(req.TerminalInfo, m_config->szTerminalInfo.c_str());
    
    int ret = m_client->ReqUserLogin(&req, ++nReqId);

    if (ret != 0)
    {   
        printf("ReqUserLogin fail, ret[%d]\n", ret);
    }
};

void TiHxTraderClient::OnFrontDisconnected(int nReason){
    LOG(INFO) << "[OnFrontDisconnected] nReason: " << nReason;
    std::cout << "[OnFrontDisconnected] nReason: " << nReason << std::endl;
};

void TiHxTraderClient::OnRspUserLogin(CTORATstpRspUserLoginField *pRspUserLoginField, CTORATstpRspInfoField *pRspInfoField, int nRequestID)
{
    std::cout << "[OnRspUserLogin] nRequestID: " << nRequestID << std::endl;
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspUserLogin fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
        return;
    }
    printf("login success\n");
    CTORATstpQryShareholderAccountField req = {0};
    m_client->ReqQryShareholderAccount(&req, ++nReqId);
};

void TiHxTraderClient::OnRspQryShareholderAccount(CTORATstpShareholderAccountField *pShareholderAccountField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) 
{
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryShareholderAccount fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
        return;
    }

    if (bIsLast){
        return;
    }

    printf("OnRspQryShareholderAccount ExchangeID %c, ShareholderID %s, InvestorID %s\n", 
        pShareholderAccountField->ExchangeID,
        pShareholderAccountField->ShareholderID,
        pShareholderAccountField->InvestorID);
    
    if (pShareholderAccountField->ExchangeID == TORA_TSTP_EXD_SSE)
    {
        m_config->szInvestorIDSH = pShareholderAccountField->InvestorID;
        m_config->szShareholderIdSH = pShareholderAccountField->ShareholderID;
    }
    if (pShareholderAccountField->ExchangeID == TORA_TSTP_EXD_SZSE)
    {
        m_config->szInvestorIDSZ = pShareholderAccountField->InvestorID;
        m_config->szShareholderIdSZ = pShareholderAccountField->ShareholderID;
    }
}; 	

void TiHxTraderClient::OnRspQryTradingAccount(CTORATstpTradingAccountField *pTradingAccountField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast)
{
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryTradingAccount fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
    if (bIsLast){
        return;
    }
    json m_json_rsp = {
        { "type", "OnRspQryTradingAccount"},
        { "data", {
            "DepartmentID", pTradingAccountField->DepartmentID,
            "AccountID", pTradingAccountField->AccountID,
            "CurrencyID", pTradingAccountField->CurrencyID,
            "PreDeposit", pTradingAccountField->PreDeposit,
            "UsefulMoney", pTradingAccountField->UsefulMoney,
            "FetchLimit", pTradingAccountField->FetchLimit,
            "PreUnDeliveredMoney", pTradingAccountField->PreUnDeliveredMoney,
            "UnDeliveredMoney", pTradingAccountField->UnDeliveredMoney,
            "Deposit", pTradingAccountField->Deposit,
            "Withdraw", pTradingAccountField->Withdraw,
            "FrozenCash", pTradingAccountField->FrozenCash,
            "UnDeliveredFrozenCash", pTradingAccountField->UnDeliveredFrozenCash,
            "FrozenCommission", pTradingAccountField->FrozenCommission,
            "UnDeliveredFrozenCommission", pTradingAccountField->UnDeliveredFrozenCommission,
            "Commission", pTradingAccountField->Commission,
            "UnDeliveredCommission", pTradingAccountField->UnDeliveredCommission,
            "AccountType", pTradingAccountField->AccountType,
            "InvestorID", pTradingAccountField->InvestorID,
            "BankID", pTradingAccountField->BankID,
            "BankAccountID", pTradingAccountField->BankAccountID,
        }}
    };
    std::cout << m_json_rsp.dump() << std::endl;
    m_cb->OnCommonJsonRespones(&m_json_rsp, nRequestID, true, pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
}; 

void TiHxTraderClient::OnRspQryPosition(CTORATstpPositionField *pPositionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast)
{
    printf("OnRspQryPosition, is_last[%d], req_id[%d]\n", bIsLast, nRequestID);
    
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryPosition fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
    if (bIsLast){
        return;
    }

    json m_json_rsp = {
        { "type", "OnRspQryPosition"},
        { "data", {
            "ExchangeID" , pPositionField->ExchangeID,
            "InvestorID" , pPositionField->InvestorID,
            "BusinessUnitID" , pPositionField->BusinessUnitID,
            "MarketID" , pPositionField->MarketID,
            "ShareholderID" , pPositionField->ShareholderID,
            "TradingDay" , pPositionField->TradingDay,
            "SecurityID" , pPositionField->SecurityID,
            "SecurityName" , TiEncodingTool::GbkToUtf8(pPositionField->SecurityName).c_str(),
            "HistoryPos" , pPositionField->HistoryPos,
            "HistoryPosFrozen" , pPositionField->HistoryPosFrozen,
            "TodayBSPos" , pPositionField->TodayBSPos,
            "TodayBSPosFrozen" , pPositionField->TodayBSPosFrozen,
            "TodayPRPos" , pPositionField->TodayPRPos,
            "TodayPRPosFrozen" , pPositionField->TodayPRPosFrozen,
            "TodaySMPos" , pPositionField->TodaySMPos,
            "TodaySMPosFrozen" , pPositionField->TodaySMPosFrozen,
            "HistoryPosPrice" , pPositionField->HistoryPosPrice,
            "TotalPosCost" , pPositionField->TotalPosCost,
            "PrePosition" , pPositionField->PrePosition,
            "AvailablePosition" , pPositionField->AvailablePosition,
            "CurrentPosition" , pPositionField->CurrentPosition,
            "OpenPosCost" , pPositionField->OpenPosCost,
            "CreditBuyPos" , pPositionField->CreditBuyPos,
            "CreditSellPos" , pPositionField->CreditSellPos,
            "TodayCreditSellPos" , pPositionField->TodayCreditSellPos,
            "CollateralOutPos" , pPositionField->CollateralOutPos,
            "RepayUntradeVolume" , pPositionField->RepayUntradeVolume,
            "RepayTransferUntradeVolume" , pPositionField->RepayTransferUntradeVolume,
            "CollateralBuyUntradeAmount" , pPositionField->CollateralBuyUntradeAmount,
            "CollateralBuyUntradeVolume" , pPositionField->CollateralBuyUntradeVolume,
            "CreditBuyAmount" , pPositionField->CreditBuyAmount,
            "CreditBuyUntradeAmount" , pPositionField->CreditBuyUntradeAmount,
            "CreditBuyFrozenMargin" , pPositionField->CreditBuyFrozenMargin,
            "CreditBuyInterestFee" , pPositionField->CreditBuyInterestFee,
            "CreditBuyUntradeVolume" , pPositionField->CreditBuyUntradeVolume,
            "CreditSellAmount" , pPositionField->CreditSellAmount,
            "CreditSellUntradeAmount" , pPositionField->CreditSellUntradeAmount,
            "CreditSellFrozenMargin" , pPositionField->CreditSellFrozenMargin,
            "CreditSellInterestFee" , pPositionField->CreditSellInterestFee,
            "CreditSellUntradeVolume" , pPositionField->CreditSellUntradeVolume,
            "CollateralInPos" , pPositionField->CollateralInPos,
            "CreditBuyFrozenCirculateMargin" , pPositionField->CreditBuyFrozenCirculateMargin,
            "CreditSellFrozenCirculateMargin" , pPositionField->CreditSellFrozenCirculateMargin,
            "CloseProfit" , pPositionField->CloseProfit,
            "TodayTotalOpenVolume" , pPositionField->TodayTotalOpenVolume,
            "TodayCommission" , pPositionField->TodayCommission,
            "TodayTotalBuyAmount" , pPositionField->TodayTotalBuyAmount,
            "TodayTotalSellAmount" , pPositionField->TodayTotalSellAmount,
            "PreFrozen" , pPositionField->PreFrozen,
        }}
    };
    std::cout << m_json_rsp.dump() << std::endl;
    m_cb->OnCommonJsonRespones(&m_json_rsp, nRequestID, true, pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
}; 

void TiHxTraderClient::OnRspQryETFFile(CTORATstpETFFileField *pETFFileField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast)
{
    printf("OnRspQryETFFile, is_last[%d], req_id[%d]\n", bIsLast, nRequestID);
    
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryETFFile fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
    if (bIsLast){
        return;
    }

    json m_json_rsp = {
        { "type", "OnRspQryETFFile"},
        { "data", {
            "TradingDay" , pETFFileField->TradingDay,
            "ExchangeID" , pETFFileField->ExchangeID,
            "ETFSecurityID" , pETFFileField->ETFSecurityID,
            "ETFCreRedSecurityID" , pETFFileField->ETFCreRedSecurityID,
            "CreationRedemptionUnit" , pETFFileField->CreationRedemptionUnit,
            "Maxcashratio" , pETFFileField->Maxcashratio,
            "EstimateCashComponent" , pETFFileField->EstimateCashComponent,
            "CashComponent" , pETFFileField->CashComponent,
            "NAV" , pETFFileField->NAV,
            "NAVperCU" , pETFFileField->NAVperCU,
            "DividendPerCU" , pETFFileField->DividendPerCU,
            "ETFCreRedType" , pETFFileField->ETFCreRedType,
            "ETFSecurityName" , TiEncodingTool::GbkToUtf8(pETFFileField->ETFSecurityName).c_str(),
        }}
    };
    std::cout << m_json_rsp.dump() << std::endl;
    m_cb->OnCommonJsonRespones(&m_json_rsp, nRequestID, true, pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
}; 
    
void TiHxTraderClient::OnRspQryETFBasket(CTORATstpETFBasketField *pETFBasketField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast)
{
    printf("OnRspQryETFFile, is_last[%d], req_id[%d]\n", bIsLast, nRequestID);
    
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryETFFile fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
    if (bIsLast){
        return;
    }

    json m_json_rsp = {
        { "type", "OnRspQryETFBasket"},
        { "data", {
            "TradingDay" , pETFBasketField->TradingDay,
            "ExchangeID" , pETFBasketField->ExchangeID,
            "ETFSecurityID" , pETFBasketField->ETFSecurityID,
            "SecurityID" , pETFBasketField->SecurityID,
            "SecurityName" , TiEncodingTool::GbkToUtf8(pETFBasketField->SecurityName).c_str(),
            "Volume" , pETFBasketField->Volume,
            "ETFCurrenceReplaceStatus" , pETFBasketField->ETFCurrenceReplaceStatus,
            "Premium" , pETFBasketField->Premium,
            "CreationReplaceAmount" , pETFBasketField->CreationReplaceAmount,
            "RedemptionReplaceAmount" , pETFBasketField->RedemptionReplaceAmount,
            "MarketID" , pETFBasketField->MarketID,
            "ETFCreRedType" , pETFBasketField->ETFCreRedType,
        }}
    };
    std::cout << m_json_rsp.dump() << std::endl;
    m_cb->OnCommonJsonRespones(&m_json_rsp, nRequestID, true, pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
}; 

void TiHxTraderClient::OnRspQryOrder(CTORATstpOrderField *pOrderField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast)
{
    printf("OnRspQryOrder, is_last[%d], req_id[%d]\n", bIsLast, nRequestID);
    
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryOrder fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
    if (bIsLast){
        return;
    }

    std::shared_ptr<TiRtnOrderStatus> order_ptr;

    auto iter = m_order_map.find(pOrderField->OrderRef);
    if (iter == m_order_map.end())
    {
        order_ptr = std::make_shared<TiRtnOrderStatus>();
        memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
        strcpy(order_ptr->szSymbol, pOrderField->SecurityID);
        if (pOrderField->ExchangeID == TORA_TSTP_EXD_SSE) {
            strcpy(order_ptr->szExchange, "SH");
        } else if (pOrderField->ExchangeID == TORA_TSTP_EXD_SZSE) {
            strcpy(order_ptr->szExchange, "SZ");
        } else {
            strcpy(order_ptr->szExchange, "");
        }

        switch (pOrderField->Direction)
        {
        case TORA_TSTP_D_Sell:
            order_ptr->nTradeSideType = TI_TradeSideType_Sell;
            break;
        case TORA_TSTP_D_Buy:
            order_ptr->nTradeSideType = TI_TradeSideType_Buy;
            break;
        case TORA_TSTP_D_ETFPur:  
            order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
            break;
        case TORA_TSTP_D_ETFRed: 
            order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
            break;
        default:
            break;
        }

        order_ptr->nBusinessType = TI_BusinessType_Stock;
        order_ptr->nOrderPrice = pOrderField->LimitPrice;
        order_ptr->nOrderVol = pOrderField->VolumeTotalOriginal;

        m_order_map[order_ptr->nOrderId] = order_ptr;
    }else{
        order_ptr = iter->second;
    }

    order_ptr->nReqId = pOrderField->RequestID;
    order_ptr->nOrderId = pOrderField->OrderRef;
    strcpy(order_ptr->szOrderStreamId, pOrderField->OrderSysID);

    order_ptr->nSubmitVol = pOrderField->VolumeTotalOriginal;
    order_ptr->nDealtVol = pOrderField->VolumeTraded;
    order_ptr->nStatus = getOrderStatus(pOrderField->OrderStatus);

    order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(pOrderField->InsertDate, pOrderField->InsertTime);

    if (order_ptr->nLastUpdateTimestamp == 0)
    {
        order_ptr->nLastUpdateTimestamp = order_ptr->nInsertTimestamp;
    }

    nOrderId = std::max(nOrderId, (order_ptr->nOrderId + 1));

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
};

void TiHxTraderClient::OnRspQryTrade(CTORATstpTradeField *pTradeField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast)
{
    printf("OnRspQryTrade, is_last[%d], req_id[%d]\n", bIsLast, nRequestID);
    
    if (pRspInfoField->ErrorID != 0)
    {
        printf("OnRspQryTrade fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
    if (bIsLast){
        return;
    }

    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = pTradeField->OrderRef;
    strncpy(match_ptr->szStreamId, pTradeField->OrderSysID, 64);
    match_ptr->nMatchPrice = pTradeField->Price;
    match_ptr->nMatchVol = pTradeField->Volume;
    strcpy(match_ptr->szSymbol, pTradeField->SecurityID);

    if (pTradeField->ExchangeID == TORA_TSTP_EXD_SSE) {
        strcpy(match_ptr->szExchange, "SH");
    } else if (pTradeField->ExchangeID == TORA_TSTP_EXD_SZSE) {
        strcpy(match_ptr->szExchange, "SZ");
    } else {
        strcpy(match_ptr->szExchange, "");
    }

    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(pTradeField->TradeDate, pTradeField->TradeTime);
    if (pTradeField->Direction == TORA_TSTP_D_Buy) {
        match_ptr->nTradeSideType = TI_TradeSideType_Buy;
    }
    if (pTradeField->Direction == TORA_TSTP_D_Sell) {
        match_ptr->nTradeSideType = TI_TradeSideType_Sell;
    }
    m_matches_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderMatch>>(match_ptr->nOrderId, match_ptr));

}; 
    

///报单录入响应
void TiHxTraderClient::OnRspOrderInsert(CTORATstpInputOrderField *pInputOrderField, CTORATstpRspInfoField *pRspInfoField, int nRequestID)
{
    std::cout << "[OnRspOrderInsert] nRequestID: " << nRequestID << std::endl;
    if (pRspInfoField->ErrorID == 0)
    {
        printf("order insert success\n");
    }
    else
    {
        printf("order insert fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }

    std::shared_ptr<TiRtnOrderStatus> order_ptr;

    auto iter = m_order_map.find(pInputOrderField->OrderRef);
    if (iter == m_order_map.end())
    {
        order_ptr = std::make_shared<TiRtnOrderStatus>();
        memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
        strcpy(order_ptr->szSymbol, pInputOrderField->SecurityID);
        if (pInputOrderField->ExchangeID == TORA_TSTP_EXD_SSE) {
            strcpy(order_ptr->szExchange, "SH");
        } else if (pInputOrderField->ExchangeID == TORA_TSTP_EXD_SZSE) {
            strcpy(order_ptr->szExchange, "SZ");
        } else {
            strcpy(order_ptr->szExchange, "");
        }

        switch (pInputOrderField->Direction)
        {
        case TORA_TSTP_D_Sell:
            order_ptr->nTradeSideType = TI_TradeSideType_Sell;
            break;
        case TORA_TSTP_D_Buy:
            order_ptr->nTradeSideType = TI_TradeSideType_Buy;
            break;
        case TORA_TSTP_D_ETFPur:  
            order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
            break;
        case TORA_TSTP_D_ETFRed: 
            order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
            break;
        default:
            break;
        }

        order_ptr->nBusinessType = TI_BusinessType_Stock;
        order_ptr->nOrderPrice = pInputOrderField->LimitPrice;
        order_ptr->nOrderVol = pInputOrderField->VolumeTotalOriginal;

        m_order_map[pInputOrderField->OrderRef] = order_ptr;
    }else{
        order_ptr = iter->second;
    }

    order_ptr->nReqId = nRequestID;
    order_ptr->nOrderId = pInputOrderField->OrderRef;
    strcpy(order_ptr->szOrderStreamId, pInputOrderField->OrderSysID);
    order_ptr->nSubmitVol = pInputOrderField->VolumeTotalOriginal;
    order_ptr->nStatus = TI_OrderStatusType_unAccept;
    order_ptr->nInsertTimestamp = datetime::get_now_timestamp_ms();
    order_ptr->nLastUpdateTimestamp = datetime::get_now_timestamp_ms();
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
};

 void TiHxTraderClient::OnRtnOrder(CTORATstpOrderField *pOrderField)
 {
    std::shared_ptr<TiRtnOrderStatus> order_ptr;

    auto iter = m_order_map.find(pOrderField->OrderRef);
    if (iter == m_order_map.end())
    {
        order_ptr = std::make_shared<TiRtnOrderStatus>();
        memset(order_ptr.get(), 0, sizeof(TiRtnOrderStatus));
        strcpy(order_ptr->szSymbol, pOrderField->SecurityID);
        if (pOrderField->ExchangeID == TORA_TSTP_EXD_SSE) {
            strcpy(order_ptr->szExchange, "SH");
        } else if (pOrderField->ExchangeID == TORA_TSTP_EXD_SZSE) {
            strcpy(order_ptr->szExchange, "SZ");
        } else {
            strcpy(order_ptr->szExchange, "");
        }

        switch (pOrderField->Direction)
        {
        case TORA_TSTP_D_Sell:
            order_ptr->nTradeSideType = TI_TradeSideType_Sell;
            break;
        case TORA_TSTP_D_Buy:
            order_ptr->nTradeSideType = TI_TradeSideType_Buy;
            break;
        case TORA_TSTP_D_ETFPur:  
            order_ptr->nTradeSideType = TI_TradeSideType_Purchase;
            break;
        case TORA_TSTP_D_ETFRed: 
            order_ptr->nTradeSideType = TI_TradeSideType_Redemption;
            break;
        default:
            break;
        }

        order_ptr->nBusinessType = TI_BusinessType_Stock;
        order_ptr->nOrderPrice = pOrderField->LimitPrice;
        order_ptr->nOrderVol = pOrderField->VolumeTotalOriginal;

        m_order_map[pOrderField->OrderRef] = order_ptr;
    }else{
        order_ptr = iter->second;
    }
    
    order_ptr->nReqId = pOrderField->RequestID;
    order_ptr->nOrderId = pOrderField->OrderRef;
    strcpy(order_ptr->szOrderStreamId, pOrderField->OrderSysID);

    order_ptr->nSubmitVol = pOrderField->VolumeTotalOriginal;
    order_ptr->nDealtVol = pOrderField->VolumeTraded;
    order_ptr->nStatus = getOrderStatus(pOrderField->OrderStatus);

    order_ptr->nInsertTimestamp = datetime::get_timestamp_ms(pOrderField->InsertDate, pOrderField->InsertTime);
    order_ptr->nLastUpdateTimestamp = datetime::get_now_timestamp_ms();
    order_ptr->nUsedTime = order_ptr->nLastUpdateTimestamp - order_ptr->nInsertTimestamp;

    m_cb->OnRtnOrderStatusEvent(order_ptr.get());
};

void TiHxTraderClient::OnErrRtnOrderInsert(CTORATstpInputOrderField *pInputOrderField, CTORATstpRspInfoField *pRspInfoField, int nRequestID)
{
    OnRspOrderInsert(pInputOrderField, pRspInfoField, nRequestID);
};

void TiHxTraderClient::OnRtnTrade(CTORATstpTradeField *pTradeField)
{
    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = pTradeField->OrderRef;
    strncpy(match_ptr->szStreamId, pTradeField->OrderSysID, 64);
    match_ptr->nMatchPrice = pTradeField->Price;
    match_ptr->nMatchVol = pTradeField->Volume;
    strcpy(match_ptr->szSymbol, pTradeField->SecurityID);

    if (pTradeField->ExchangeID == TORA_TSTP_EXD_SSE) {
        strcpy(match_ptr->szExchange, "SH");
    } else if (pTradeField->ExchangeID == TORA_TSTP_EXD_SZSE) {
        strcpy(match_ptr->szExchange, "SZ");
    } else {
        strcpy(match_ptr->szExchange, "");
    }

    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(pTradeField->TradeDate, pTradeField->TradeTime);
    if (pTradeField->Direction == TORA_TSTP_D_Buy) {
        match_ptr->nTradeSideType = TI_TradeSideType_Buy;
    }
    if (pTradeField->Direction == TORA_TSTP_D_Sell) {
        match_ptr->nTradeSideType = TI_TradeSideType_Sell;
    }
    m_matches_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderMatch>>(match_ptr->nOrderId, match_ptr));

    m_cb->OnRtnOrderMatchEvent(match_ptr.get());
};

///撤单响应
void TiHxTraderClient::OnRspOrderAction(CTORATstpInputOrderActionField *pInputOrderActionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID)
{
    std::cout << "[OnRspOrderAction] nRequestID: " << nRequestID << std::endl;
    if (pRspInfoField->ErrorID == 0)
    {
        printf("OnRspOrderAction success\n");
    }
    else
    {
        printf("OnRspOrderAction fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
};
    
///撤单错误回报
void TiHxTraderClient::OnErrRtnOrderAction(CTORATstpInputOrderActionField *pInputOrderActionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID)
{
    std::cout << "[OnErrRtnOrderAction] nRequestID: " << nRequestID << std::endl;
    if (pRspInfoField->ErrorID == 0)
    {
        printf("OnErrRtnOrderAction success\n");
    }
    else
    {
        printf("OnErrRtnOrderAction fail, error_id[%d] error_msg[%s]\n", pRspInfoField->ErrorID, TiEncodingTool::GbkToUtf8(pRspInfoField->ErrorMsg).c_str() );
    }
};

    
////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiHxTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations           = string(_iniFile["ti_hx_trader_client"]["locations"]);
    
    m_config->szUser                = string(_iniFile["ti_hx_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_hx_trader_client"]["pass"]);

    m_config->szUserProductInfo     = string(_iniFile["ti_hx_trader_client"]["user_product_info"]);
    m_config->szDeviceID            = string(_iniFile["ti_hx_trader_client"]["device_id"]);
    m_config->szCertSerial          = string(_iniFile["ti_hx_trader_client"]["cert_serial"]);
    m_config->szProductInfo         = string(_iniFile["ti_hx_trader_client"]["product_info"]);
    m_config->szTerminalInfo        = string(_iniFile["ti_hx_trader_client"]["terminal_info"]);

    
    if( m_config->szLocations.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

TI_OrderStatusType TiHxTraderClient::getOrderStatus(TTORATstpOrderStatusType status)
{   
    switch (status)
    {
    case TORA_TSTP_OST_Unknown:
        return TI_OrderStatusType_unAccept;
    case TORA_TSTP_OST_PartTraded:
        return TI_OrderStatusType_dealt;
    case TORA_TSTP_OST_AllTraded:
        return TI_OrderStatusType_dealt;
    case TORA_TSTP_OST_PartTradeCanceled:
        return TI_OrderStatusType_removed;
    case TORA_TSTP_OST_AllCanceled:
        return TI_OrderStatusType_removed;
    case TORA_TSTP_OST_Rejected:
        return TI_OrderStatusType_fail;
    case TORA_TSTP_OST_Cached:
        return TI_OrderStatusType_queued;
    case TORA_TSTP_OST_SendTradeEngine:
        return TI_OrderStatusType_queued;
    case TORA_TSTP_OST_Accepted:
        return TI_OrderStatusType_queued;
    default:
        return TI_OrderStatusType_unAccept;
    }
   return TI_OrderStatusType_unAccept;
};

int TiHxTraderClient::orderInsertStock(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;

    CTORATstpInputOrderField msg = {0};
    if (!strcmp(req->szExchange, "SH"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SSE;             // 市场ID，上海
        strncpy(msg.ShareholderID, m_config->szShareholderIdSH.c_str(), 11);               // 股东号
        strncpy(msg.InvestorID, m_config->szInvestorIDSH.c_str(), 16);                // 投资者ID
        std::cout << "ShareholderID: " << msg.ShareholderID << std::endl;
        std::cout << "InvestorID: " << msg.InvestorID << std::endl;
    }
    if (!strcmp(req->szExchange, "SZ"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SZSE;             // 市场ID，上海
        strncpy(msg.ShareholderID, m_config->szShareholderIdSZ.c_str(), 11);               // 账户ID
        strncpy(msg.InvestorID, m_config->szInvestorIDSZ.c_str(), 16);                     // 投资者ID
    }
    strncpy(msg.SecurityID, req->szSymbol, 31);               // 证券代码

    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Sell:
        msg.Direction = TORA_TSTP_D_Sell;   
        break;
    case TI_TradeSideType_Buy:
        msg.Direction = TORA_TSTP_D_Buy;   
        break;
    }
    msg.OrderPriceType = TORA_TSTP_OPT_LimitPrice;      // 订单类型，限价
    msg.LimitPrice = req->nOrderPrice;                  // 委托价格 N13(4)，21.0000元
    msg.VolumeTotalOriginal = req->nOrderVol;           //
    msg.TimeCondition = TORA_TSTP_TC_GFD;
    msg.VolumeCondition = TORA_TSTP_VC_AV;
    msg.OrderRef = ++nOrderId;

    int ret = m_client->ReqOrderInsert(&msg, req->nReqId);
    if (ret != 0)
    {   
        printf("ReqOrderInsert fail, ret[%d]\n", ret);
    }
    return nReqId;
};
int TiHxTraderClient::orderInsertEtf(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;

    CTORATstpInputOrderField msg = {0};
    if (!strcmp(req->szExchange, "SH"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SSE;             // 市场ID，上海
        strncpy(msg.ShareholderID, m_config->szShareholderIdSH.c_str(), 11);               // 股东号
        strncpy(msg.InvestorID, m_config->szInvestorIDSH.c_str(), 16);                // 投资者ID
        std::cout << "ShareholderID: " << msg.ShareholderID << std::endl;
        std::cout << "InvestorID: " << msg.InvestorID << std::endl;
    }
    if (!strcmp(req->szExchange, "SZ"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SZSE;             // 市场ID，上海
        strncpy(msg.ShareholderID, m_config->szShareholderIdSZ.c_str(), 11);               // 账户ID
        strncpy(msg.InvestorID, m_config->szInvestorIDSZ.c_str(), 16);                     // 投资者ID
    }
    strncpy(msg.SecurityID, req->szSymbol, 31);               // 证券代码

    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Purchase:
        msg.Direction = TORA_TSTP_D_ETFPur;   
        break;
    case TI_TradeSideType_Redemption:
        msg.Direction = TORA_TSTP_D_ETFRed;   
        break;
    }
    msg.OrderPriceType = TORA_TSTP_OPT_LimitPrice;      // 订单类型，限价
    msg.LimitPrice = req->nOrderPrice;                  // 委托价格 N13(4)，21.0000元
    msg.VolumeTotalOriginal = req->nOrderVol;           //
    msg.TimeCondition = TORA_TSTP_TC_GFD;
    msg.VolumeCondition = TORA_TSTP_VC_AV;
    msg.OrderRef = ++nOrderId;

    int ret = m_client->ReqOrderInsert(&msg, req->nReqId);
    if (ret != 0)
    {   
        printf("ReqOrderInsert fail, ret[%d]\n", ret);
    }
    return nReqId;
};

void TiHxTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    if (m_client)   //如果已经实例了就退出
    {
        return;
    }
    m_client = CTORATstpTraderApi::CreateTstpTraderApi();
    m_client->RegisterSpi(this);
    std::cout << "[connect] " << m_config->szLocations << std::endl;
    m_client->RegisterFront((char*)m_config->szLocations.c_str());
    m_client->SubscribePrivateTopic(TORA_TERT_QUICK);
    m_client->SubscribePublicTopic(TORA_TERT_RESTART);
    m_client->Init();
};

int TiHxTraderClient::orderInsert(TiReqOrderInsert* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    if(req->nBusinessType == TI_BusinessType_Stock){
        return orderInsertStock(req);
    }else if(req->nBusinessType == TI_BusinessType_ETF){
        return orderInsertEtf(req);
    }
    return -1;
};

int TiHxTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    TiRtnOrderStatus* order = getOrderStatus(-1, req->nOrderId);

    auto iter = m_order_map.begin();
    for (; iter != m_order_map.end(); iter++)
    {

        std::cout << iter->first << ":" << iter->second->szOrderStreamId << std::endl;
        /* code */
    }
    
    std::cout << order << std::endl;
    if(!order)
    {
        return -1;
    }

    CTORATstpInputOrderActionField msg = {0};
    strncpy(msg.OrderSysID, order->szOrderStreamId, 21);
    msg.ActionFlag = TORA_TSTP_AF_Delete;

    if (!strcmp(order->szExchange, "SH"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SSE;             // 市场ID，上海
    }
    if (!strcmp(order->szExchange, "SZ"))
    {
        msg.ExchangeID = TORA_TSTP_EXD_SZSE;             // 市场ID，上海                   // 投资者ID
    }
    
    int ret = m_client->ReqOrderAction(&msg, ++nReqId);
    if (ret != 0)
    {   
        printf("ReqQryOrder fail, ret[%d]\n", ret);
    }
    return nReqId;
};

TiRtnOrderStatus* TiHxTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    if (m_order_map.find(order_id) != m_order_map.end())
    {
        return m_order_map[order_id].get();
    }
    auto iter = m_order_map.begin();
    for( ; iter != m_order_map.end(); iter++)
    {
        if (iter->second->nReqId == req_id)
        {
            return iter->second.get();
        }
    }
    return NULL;
};
    

int TiHxTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    CTORATstpQryTradingAccountField req = {0};
    
	int ret = m_client->ReqQryTradingAccount(&req, ++nReqId);
    if (ret != 0)
    {   
        printf("QueryAsset fail, ret[%d]\n", ret);
    }
    return nReqId;
};

int TiHxTraderClient::QueryOrders()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    CTORATstpQryOrderField req = {0};
    int ret = m_client->ReqQryOrder(&req, ++nReqId); 
    if (ret != 0)
    {   
        printf("ReqQryOrder fail, ret[%d]\n", ret);
    }
    return nReqId;
};

int TiHxTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    CTORATstpQryTradeField req = {0};
    int ret = m_client->ReqQryTrade(&req, ++nReqId); 
    if (ret != 0)
    {   
        printf("QueryMatches fail, ret[%d]\n", ret);
    }
    return nReqId;
};

int TiHxTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    CTORATstpQryPositionField req = {0};
    int ret = m_client->ReqQryPosition(&req, ++nReqId); 
    if (ret != 0)
    {   
        printf("QueryPositions fail, ret[%d]\n", ret);
    }
    return nReqId;
};


int TiHxTraderClient::QueryETFFile()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    CTORATstpQryETFFileField req = {0};
    int ret = m_client->ReqQryETFFile(&req, ++nReqId); 
    if (ret != 0)
    {   
        printf("QueryETFFile fail, ret[%d]\n", ret);
    }
    return nReqId;
};

int TiHxTraderClient::QueryETFBasket()
{
    CTORATstpQryETFBasketField req = {0};
    int ret = m_client->ReqQryETFBasket(&req, ++nReqId); 
    if (ret != 0)
    {   
        printf("QueryETFFile fail, ret[%d]\n", ret);
    }
    return nReqId;
};

