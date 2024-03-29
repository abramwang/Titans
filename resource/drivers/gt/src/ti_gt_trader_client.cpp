#include "ti_gt_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"
#include "ti_trader_formater.h"
#include <thread>


#include <atomic>

TiGtTraderClient::TiGtTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_client = NULL;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 100;   //跳过xtp client设置成交模式的区段
    nOrderId = 5186;
};


TiGtTraderClient::~TiGtTraderClient()
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

void TiGtTraderClient::work_cb(uv_work_t* req)
{
    std::thread::id threadId = std::this_thread::get_id();
    std::cout << "TiGtTraderClient::work_cb" << "Current thread ID: " << threadId << std::endl;
    TiGtTraderClient* client = (TiGtTraderClient*)req->data;
    client->m_client->join_async();
};

void TiGtTraderClient::after_work_cb(uv_work_t* req, int status)
{
    
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiGtTraderClient::onConnected(bool success, const char* errorMsg) 
{
    cout << "[onConnected] server connect " << (success ? string("success") : string("failure, err: ") + errorMsg) << endl;
    if (!success)
    {
        return;
    }
    std::cout << m_config->szUser.c_str() << " " << m_config->szPass.c_str() << std::endl;
    m_client->userLogin(m_config->szUser.c_str(), m_config->szPass.c_str(), nReqId++);
}

void TiGtTraderClient::onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error)
{
    cout << "[onUserLogin] login " << (error.isSuccess() ? "success" : string("failure, err: ") + error.errorMsg()) << endl;
}

void TiGtTraderClient::onRtnLoginStatus(const char* accountId, EBrokerLoginStatus status, int brokerType, const char* errorMsg)
{
    cout << "[onRtnLoginStatus] account id: " << accountId << ", type: " << brokerType << ", status: " << status << endl;

    TI_BrokerType ti_broker_type;
    switch (brokerType)
    {
    case AT_STOCK:
        ti_broker_type = TI_BrokerType_Stock;
        break;
    case AT_FUTURE:
        ti_broker_type = TI_BrokerType_Future;
        break;
    case AT_STOCK_OPTION:
        ti_broker_type = TI_BrokerType_Option;
        break;
    case AT_CREDIT:
        ti_broker_type = TI_BrokerType_Credit;
        break;
    default:
        break;
    }
    TI_AccountType account_id = {0};
    strncpy(account_id, accountId, sizeof(account_id));

    std::shared_ptr<TiGtTraderAccount> account_ptr = std::make_shared<TiGtTraderAccount>(ti_broker_type, account_id, m_cb);
    m_account_map[account_id] = account_ptr;

    m_client->reqAccountDetail(accountId, ++nReqId);
}

 // 资金账号信息
void TiGtTraderClient::onReqAccountDetail(const char* accountId, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error)
{
    json rspData = {
        {"type", "onReqAccountDetail"},
        {"api_type", "gt"},
        {"account_id", accountId},
        {"data", {
            {"account_id", data->m_strAccountID},
            {"account_type", data->m_nAccountType},
            {"status", data->m_strStatus},
            {"trading_date", data->m_strTradingDate},

            {"frozen_margin", data->m_dFrozenMargin},
            {"frozen_cash", data->m_dFrozenCash},
            {"frozen_commission", data->m_dFrozenCommission},
            {"risk", data->m_dRisk},
            {"nav", data->m_dNav},
            {"pre_balance", data->m_dPreBalance},
            {"balance", data->m_dBalance},
            {"available", data->m_dAvailable},
            {"commission", data->m_dCommission},
            {"position_profit", data->m_dPositionProfit},
            {"close_profit", data->m_dCloseProfit},
            {"cash_in", data->m_dCashIn},
            {"curr_margin", data->m_dCurrMargin},
            {"instrument_value", data->m_dInstrumentValue},
            {"deposit", data->m_dDeposit},
            {"withdraw", data->m_dWithdraw},
            {"credit", data->m_dCredit},
            {"mortgage", data->m_dMortgage},
            {"stock_value", data->m_dStockValue},
            {"loan_value", data->m_dLoanValue},
            {"fund_value", data->m_dFundValue},
            {"repurchase_value", data->m_dRepurchaseValue},
            {"long_value", data->m_dLongValue},
            {"short_value", data->m_dShortValue},
            {"net_value", data->m_dNetValue},
            {"assure_asset", data->m_dAssureAsset},
            {"total_debit", data->m_dTotalDebit},
            {"premium_net_expense", data->m_dPremiumNetExpense},
            {"enable_margin", data->m_dEnableMargin},
            {"fetch_balance", data->m_dFetchBalance},
            {"dual_status", data->m_eDualStatus},
            {"available_sh", data->m_dAvailableSH},
            {"available_sz", data->m_dAvailableSZ},
            {"account_key", data->m_strAccountKey},
            {"product_id", data->m_nProductId},
            {"used_margin", data->m_dUsedMargin},
            {"royalty", data->m_dRoyalty},
            {"product_name", data->m_strProductName},
            {"days_profit", data->m_dDaysProfit}
        }}
    };
    
    auto account_iter = m_account_map.find(accountId);
    if (account_iter != m_account_map.end())
    {
        account_iter->second->OnCommonJsonRespones(&rspData, nRequestId, isLast, error.errorID(), error.errorMsg());
        //nReqId ++;
        //m_client->reqSecuAccount(accountId, nReqId);
    }

    /* 
    cout << "[onReqAccountDetail]  资金账号 :" << data->m_strAccountID
        << "\n    账号状态:" << data->m_strStatus
        << "\n    交易日:" << data->m_strTradingDate
        << "\n    可用资金:" << data->m_dAvailable
        << "\n    净资产:" << data->m_dAssureAsset
        << "\n    总负债:" << data->m_dTotalDebit
        << "\n    期初权益:" << data->m_dPreBalance
        << endl;
    */
};

void TiGtTraderClient::onReqSecuAccount(const char* accountID, int nRequestId, const char* accountKey, const CSecuAccount* data, bool isLast, const XtError& error)
{
    std::cout << "[onReqSecuAccount]:" << isLast << " " << accountID << " " << nRequestId << " " << accountKey << " " << data  << " " << data->m_strExchangeID << std::endl;
    if(!data){
        return;
    }
    std::string main_flag = data->m_eMainFlag == MAIN_FLAG_VICE ? "vice" : "main";
    json rspData = {
        {"type", "onBatchReqSecuAccount"},
        {"api_type", "gt"},
        {"account_id", accountID},
        {"data", {
            {"exchange", data->m_strExchangeID},
            {"security_account", data->m_strSecuAccount},
            {"main_flag", main_flag },
        }}
    };

    auto account_iter = m_account_map.find(accountID);
    if (account_iter != m_account_map.end())
    {
        account_iter->second->OnCommonJsonRespones(&rspData, nRequestId, isLast, error.errorID(), error.errorMsg());
    }
};

void TiGtTraderClient::onReqPositionDetail(const char* accountID, int nRequestId, const CPositionDetail* data, bool isLast, const XtError& error)
{
    if (data == NULL)
    {
        return;
    }

    auto account_iter = m_account_map.find(data->m_strAccountID);
    if (account_iter == m_account_map.end())
    {
        return;
    }

    std::shared_ptr<TiRspQryPosition> position_ptr = std::make_shared<TiRspQryPosition>();
    account_iter->second->enterPosition(position_ptr);

    strcpy(position_ptr->szSymbol, data->m_strInstrumentID);
    strcpy(position_ptr->szExchange, data->m_strExchangeID);
    strcpy(position_ptr->szAccount, data->m_strAccountID);
    position_ptr->nPosition = data->m_nVolume;
    position_ptr->nPrice = data->m_dOpenPrice;
    //position_ptr->nProfit = data->m_dProfitRate;
    //position_ptr->nSettledProfit = data->m_dProfitRate;
    strcpy(position_ptr->szShareholderId, data->m_strSecuAccount);

    m_cb->OnRspQryPosition(position_ptr.get(), isLast);
};

void TiGtTraderClient::onReqOrderDetail(const char* accountID, int nRequestId, const COrderDetail* data, bool isLast, const XtError& error)
{
    std::cout << "[onReqOrderDetail]:" << isLast << " " << accountID << " " << nRequestId << " " << data << std::endl;

    if (data == NULL)
    {
        return;
    }

    auto account_iter = m_account_map.find(data->m_strAccountID);
    if (account_iter == m_account_map.end())
    {
        return;
    }
    TiRtnOrderStatus* order = account_iter->second->getOrderStatus(data->m_nOrderID, data->m_strInstrumentID);
    if(!order){
        order = account_iter->second->getOrderStatus(data->m_strOrderSysID);
    }

    strcpy(order->szExchange, data->m_strExchangeID);
    strcpy(order->szSymbol, data->m_strInstrumentID);
    strcpy(order->szAccount, data->m_strAccountID);

    order->nTradeSideType = convertTradeSide(data->m_nOrderPriceType, data->m_eOffsetFlag);
    order->nOrderPrice = data->m_dLimitPrice;
    order->nOrderVol = data->m_nTotalVolume;
    order->nSubmitVol = data->m_nTotalVolume;
    order->nDealtPrice = data->m_dAveragePrice;
    order->nDealtVol = data->m_nTradedVolume;
    order->nStatus = convertOrderStatus(data->m_eOrderStatus);

    if (order->nStatus < 0)
    {
        if (order->nStatus == TI_OrderStatusType_fail){
            order->nInValid = order->nTotalWithDrawnVol = order->nOrderVol - order->nDealtVol;
        }else{
            order->nTotalWithDrawnVol = order->nOrderVol - order->nDealtVol;
        }
    }

    strcpy(order->szShareholderId, data->m_strSecuAccount);

    order->nInsertTimestamp = datetime::get_timestamp_ms(atoi(data->m_strInsertDate), atoi(data->m_strInsertTime)*1000);
    order->nLastUpdateTimestamp = datetime::get_timestamp_ms(atoi(data->m_strInsertDate), atoi(data->m_strInsertTime)*1000);
    order->nUsedTime = order->nReqTimestamp?(order->nLastUpdateTimestamp - order->nReqTimestamp):(order->nLastUpdateTimestamp - order->nInsertTimestamp);

    order->nDealtVol = data->m_nTradedVolume;
    strcpy(order->szOrderStreamId, data->m_strOrderSysID);
    order->nFee = data->m_dFrozenCommission;

    /*
    sprintf(order->szErr, "%d-%d-%d-%d-%d-%d-%d-%d", 
        data->m_nOrderPriceType,
        data->m_nDirection,
        data->m_eOffsetFlag,
        data->m_eHedgeFlag,
        data->m_eOrderSubmitStatus,
        data->m_eOrderStatus,
        data->m_eEntrustType,
        data->m_eCoveredFlag);
    */

    m_cb->OnRspQryOrder(order, isLast);
};

void TiGtTraderClient::onReqDealDetail(const char* accountID, int nRequestId, const CDealDetail* data, bool isLast, const XtError& error)
{
    std::cout << "[onReqDealDetail]:" << isLast << " " << accountID << " " << nRequestId << " " << data << std::endl;

    if (data == NULL)
    {
        return;
    }

    auto account_iter = m_account_map.find(data->m_strAccountID);
    if (account_iter == m_account_map.end())
    {
        return;
    }
    TiRtnOrderStatus* order_ptr = account_iter->second->getOrderStatus(data->m_strOrderSysID);
    
    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = order_ptr->nOrderId;
    strncpy(match_ptr->szOrderStreamId, data->m_strOrderSysID, 64);
    strncpy(match_ptr->szStreamId, data->m_strTradeID, 64);
    strcpy(match_ptr->szAccount, data->m_strAccountID);

    match_ptr->nMatchPrice = data->m_dAveragePrice;
    match_ptr->nMatchVol = data->m_nVolume;
    strcpy(match_ptr->szSymbol, data->m_strInstrumentID);
    strcpy(match_ptr->szExchange, data->m_strExchangeID);
    strcpy(match_ptr->szShareholderId, data->m_strSecuAccount);

    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(atoi(data->m_strTradeDate), atoi(data->m_strTradeTime)*1000);
    match_ptr->nTradeSideType = convertTradeSide(data->m_nOrderPriceType, data->m_nOffsetFlag);

    account_iter->second->enterMatch(match_ptr);

    /*
    sprintf(match_ptr->szErr, "%d-%d-%d-%d-%d-%d", 
        data->m_nDirection,
        data->m_nOffsetFlag,
        data->m_nHedgeFlag,
        data->m_nOrderPriceType,
        data->m_eEntrustType,
        data->m_eCoveredFlag);
    */

    m_cb->OnRspQryMatch(match_ptr.get(), isLast);
};

void TiGtTraderClient::onOrder(int nRequestId, int orderID, const char* strRemark, const XtError& error)
{
    cout << "[onOrder] isSuccess: " << (error.isSuccess()?"true":"false")
        << "\n    orderId:  " << orderID
        << "\n    RequestId: " << nRequestId  
        << "\n    errorMsg: " << error.errorMsg()
        << endl;
    
    auto range = m_order_batch_req_map.equal_range(nRequestId);

    for (auto it = range.first; it != range.second; ++it) {
        it->second->nInsertTimestamp = datetime::get_now_timestamp_ms();
        it->second->nOrderId = orderID;

        auto account_iter = m_account_map.find(it->second->szAccount);
        if (account_iter == m_account_map.end())
        {
            return;
        }
        account_iter->second->enterBatchOrder(it->second);
    }

};

void TiGtTraderClient::onRtnOrder(const COrderInfo* data)
{
    string orderStatus = "";
    switch(data->m_eStatus)
    {
    case OCS_CHECKING:   orderStatus = "风控检查中";  break;
    case OCS_APPROVING:  orderStatus = "审批中";  break;
    case OCS_REJECTED:   orderStatus = "已驳回";  break;
    case OCS_RUNNING:    orderStatus = "运行中";  break;
    case OCS_CANCELING:  orderStatus = "撤销中";  break;
    case OCS_FINISHED:   orderStatus = "已完成";  break;
    case OCS_STOPPED:    orderStatus = "已撤销";  break;
    }

    cout << "[onRtnOrder]"
        << "\n    下单ID: " << data->m_nOrderID
        << "\n    m_startTime：" << data->m_startTime
        << "\n    m_endTime: " << data->m_endTime
        << "\n    指令状态：" << orderStatus
        << "\n    成交量：" << data->m_dTradedVolume
        << "\n    撤销者：" << data->m_canceler
        << "\n    指令执行信息：" << data->m_strMsg
        << "\n    指令执行信息：" << data->m_strRemark
        << endl;
};

void TiGtTraderClient::onDirectOrder(int nRequestId, const char* strOrderSysID, const char* strRemark, const XtError& error)
{
    auto order_iter = m_order_req_map.find(nRequestId);
    if (order_iter == m_order_req_map.end())
    {
        return;
    }
    order_iter->second->nInsertTimestamp = datetime::get_now_timestamp_ms();
    
    nOrderId++;
    order_iter->second->nOrderId = nOrderId;
    strcpy(order_iter->second->szOrderStreamId, strOrderSysID);
    auto account_iter = m_account_map.find(order_iter->second->szAccount);
    if (account_iter == m_account_map.end())
    {
        return;
    }
    account_iter->second->enterOrder(order_iter->second);
};

void TiGtTraderClient::onRtnOrderDetail(const COrderDetail* data)
{
    if (data == NULL)
    {
        return;
    }

    auto account_iter = m_account_map.find(data->m_strAccountID);
    if (account_iter == m_account_map.end())
    {
        return;
    }
    TiRtnOrderStatus* order = account_iter->second->getOrderStatus(data->m_nOrderID, data->m_strInstrumentID);
    if(!order){
        order = account_iter->second->getOrderStatus(data->m_strOrderSysID);
    }

    strcpy(order->szExchange, data->m_strExchangeID);
    strcpy(order->szSymbol, data->m_strInstrumentID);
    strcpy(order->szAccount, data->m_strAccountID);

    order->nTradeSideType = convertTradeSide(data->m_nOrderPriceType, data->m_eOffsetFlag);
    order->nOrderPrice = data->m_dLimitPrice;
    order->nOrderVol = data->m_nTotalVolume;
    order->nSubmitVol = data->m_nTotalVolume;
    order->nDealtPrice = data->m_dAveragePrice;
    order->nDealtVol = data->m_nTradedVolume;
    order->nStatus = convertOrderStatus(data->m_eOrderStatus);

    if (order->nStatus < 0)
    {
        if (order->nStatus == TI_OrderStatusType_fail){
            order->nInValid = order->nTotalWithDrawnVol = order->nOrderVol - order->nDealtVol;
        }else{
            order->nTotalWithDrawnVol = order->nOrderVol - order->nDealtVol;
        }
    }

    strcpy(order->szShareholderId, data->m_strSecuAccount);

    order->nLastUpdateTimestamp = datetime::get_now_timestamp_ms();
    order->nUsedTime = order->nReqTimestamp?(order->nLastUpdateTimestamp - order->nReqTimestamp):(order->nLastUpdateTimestamp - order->nInsertTimestamp);

    order->nDealtVol = data->m_nTradedVolume;
    strcpy(order->szOrderStreamId, data->m_strOrderSysID);
    order->nFee = data->m_dFrozenCommission;

    m_cb->OnRtnOrderStatusEvent(order);
    return;
}

void TiGtTraderClient::onRtnDealDetail(const CDealDetail* data)
{
    if (data == NULL)
    {
        return;
    }

    auto account_iter = m_account_map.find(data->m_strAccountID);
    if (account_iter == m_account_map.end())
    {
        return;
    }
    TiRtnOrderStatus* order_ptr = account_iter->second->getOrderStatus(data->m_strOrderSysID);
    
    std::shared_ptr<TiRtnOrderMatch> match_ptr = std::make_shared<TiRtnOrderMatch>();
    memset(match_ptr.get(), 0, sizeof(TiRtnOrderMatch));
    match_ptr->nOrderId = order_ptr->nOrderId;
    strncpy(match_ptr->szOrderStreamId, data->m_strOrderSysID, 64);
    strncpy(match_ptr->szStreamId, data->m_strTradeID, 64);
    strcpy(match_ptr->szAccount, data->m_strAccountID);

    match_ptr->nMatchPrice = data->m_dAveragePrice;
    match_ptr->nMatchVol = data->m_nVolume;
    strcpy(match_ptr->szSymbol, data->m_strInstrumentID);
    strcpy(match_ptr->szExchange, data->m_strExchangeID);
    strcpy(match_ptr->szShareholderId, data->m_strSecuAccount);

    match_ptr->nMatchTimestamp = datetime::get_timestamp_ms(atoi(data->m_strTradeDate), atoi(data->m_strTradeTime)*1000);
    match_ptr->nTradeSideType = convertTradeSide(data->m_nOrderPriceType, data->m_nOffsetFlag);

    account_iter->second->enterMatch(match_ptr);

    m_cb->OnRtnOrderMatchEvent(match_ptr.get());
}

void TiGtTraderClient::onRtnOrderError(const COrderError* data)
{
    if (data == NULL)
    {
        return;
    }
    std::thread::id threadId = std::this_thread::get_id();
    std::cout << "TiGtTraderClient::onRtnOrderError" << "Current thread ID: " << threadId << std::endl;
    cout << "[onRtnOrderError] orderId: " << data->m_nOrderID 
        << "\n    error id: " << data->m_nErrorID
        << "\n    errormsg: " << data->m_strErrorMsg
        << "\n    m_nRequestID: " << data->m_nRequestID
        << "\n    m_nOrderID: " << data->m_nOrderID
        << endl;
}
    
////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiGtTraderClient::loadConfig(std::string iniFileName)
{
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations           = string(_iniFile["ti_gt_trader_client"]["locations"]);
    
    m_config->szUser                = string(_iniFile["ti_gt_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_gt_trader_client"]["pass"]);
    
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

TI_OrderStatusType TiGtTraderClient::convertOrderStatus(EEntrustStatus status)
{
    switch(status)
    {
    case ENTRUST_STATUS_WAIT_END:
        return TI_OrderStatusType_queued;
    case ENTRUST_STATUS_UNREPORTED:
        return TI_OrderStatusType_unAccept;
    case ENTRUST_STATUS_WAIT_REPORTING:
        return TI_OrderStatusType_unAccept;
    case ENTRUST_STATUS_REPORTED:
        return TI_OrderStatusType_accepted;
    case ENTRUST_STATUS_REPORTED_CANCEL:
        return TI_OrderStatusType_toRemove;
    case ENTRUST_STATUS_PARTSUCC_CANCEL:
        return TI_OrderStatusType_toRemove;
    case ENTRUST_STATUS_PART_CANCEL:
        return TI_OrderStatusType_removed;
    case ENTRUST_STATUS_CANCELED:
        return TI_OrderStatusType_removed;
    case ENTRUST_STATUS_PART_SUCC:
        return TI_OrderStatusType_queued;
    case ENTRUST_STATUS_SUCCEEDED:
        return TI_OrderStatusType_dealt;
    case ENTRUST_STATUS_JUNK:
        return TI_OrderStatusType_fail;
    case ENTRUST_STATUS_ACCEPT:
        return TI_OrderStatusType_accepted;
    case ENTRUST_STATUS_CONFIRMED:
        return TI_OrderStatusType_accepted;
    case ENTRUST_STATUS_DETERMINED:
        return TI_OrderStatusType_accepted;
    case ENTRUST_STATUS_PREPARE_ORDER:
        return TI_OrderStatusType_accepted;
    case ENTRUST_STATUS_PREPARE_CANCELED:
        return TI_OrderStatusType_removed;
    default:
        return TI_OrderStatusType_unAccept;
    }
    return TI_OrderStatusType_unAccept;
};

TI_TradeSideType TiGtTraderClient::convertTradeSide(EOperationType operation)
{
    switch(operation)
    {
    case OPT_BUY:
        return TI_TradeSideType_Buy;
    case OPT_SELL:
        return TI_TradeSideType_Sell;
    case OPT_ETF_PURCHASE:
        return TI_TradeSideType_Purchase;
    case OPT_ETF_REDEMPTION:
        return TI_TradeSideType_Redemption;
    default:
        return TI_TradeSideType_Default;
    }
};

TI_TradeSideType TiGtTraderClient::convertTradeSide(EBrokerPriceType price_type, EOffsetFlagType offset)
{
    if (price_type == BROKER_PRICE_LIMIT)
    {
        if (offset == EOFF_THOST_FTDC_OF_Open)
        {
            return TI_TradeSideType_Buy;
        }
        else if (offset == EOFF_THOST_FTDC_OF_Close)
        {
            return TI_TradeSideType_Sell;
        }
    }
    
    if (price_type == BROKER_PRICE_PROP_ETF)
    {
        if (offset == EOFF_THOST_FTDC_OF_Open)
        {
            return TI_TradeSideType_Purchase;
        }
        else if (offset == EOFF_THOST_FTDC_OF_Close)
        {
            return TI_TradeSideType_Redemption;
        }
    }

    return TI_TradeSideType_Default;  
};

void TiGtTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    if (m_client)   //如果已经实例了就退出
    {
        return;
    }

    m_client = XtTraderApi::createXtTraderApi(m_config->szLocations.c_str());
    if (NULL == m_client)
    {
        cout << "[init] create api failure" << endl;
        return;
    }

    m_client->setCallback(this);
    m_client->init("../config");

    //m_client->join_async();

    memset(&m_work_req, 0, sizeof(uv_work_t));
    m_work_req.data = this;

    uv_queue_work(uv_default_loop(), &m_work_req, TiGtTraderClient::work_cb, TiGtTraderClient::after_work_cb);
};

int TiGtTraderClient::orderInsertBatch(std::vector<TiReqOrderInsert> &req_vec, std::string account_id)
{
    std::cout << "orderInsertBatch: " << account_id << std::endl;
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    auto account_iter = m_account_map.find(account_id);
    if (account_iter == m_account_map.end())
    {
        return -1;
    }

    std::cout << "orderInsertBatch: " << req_vec.size() << std::endl;

    ++nReqId;

    COrdinaryGroupOrder msg;

    strcpy(msg.m_strAccountID, account_id.c_str());

    msg.m_ePriceType            = PRTP_FIX;
    msg.m_eHedgeFlag            = HEDGE_FLAG_SPECULATION;
    msg.m_eOverFreqOrderMode    = OFQ_QUEUE;

    for(size_t i = 0; i <= req_vec.size(); i++)
    {
        req_vec[i].nReqId = nReqId;
        strcpy(msg.m_strMarket[i], req_vec[i].szExchange);
        strcpy(msg.m_strInstrument[i], req_vec[i].szSymbol);
        msg.m_nVolume[i] = req_vec[i].nOrderVol;
        msg.m_dPrice[i] = req_vec[i].nOrderPrice;
        switch (req_vec[i].nTradeSideType)
        {
        case TI_TradeSideType_Sell:
            msg.m_eOperationType[i] = OPT_SELL; 
            break;
        case TI_TradeSideType_Buy:
            msg.m_eOperationType[i] = OPT_BUY; 
            break;
        case TI_TradeSideType_Purchase:
            msg.m_eOperationType[i] = OPT_ETF_PURCHASE; 
            break;
        case TI_TradeSideType_Redemption:
            msg.m_eOperationType[i] = OPT_ETF_REDEMPTION; 
            break;
        }
        // 投资备注
        strcpy(msg.m_strRemark, req_vec[i].szUseStr);

        std::shared_ptr<TiRtnOrderStatus> order_ptr = std::make_shared<TiRtnOrderStatus>();
        memcpy(order_ptr.get(), &req_vec[i], sizeof(TiReqOrderInsert));
        m_order_batch_req_map.insert(std::pair<int64_t, std::shared_ptr<TiRtnOrderStatus>>(nReqId, order_ptr));
        order_ptr->nReqTimestamp = datetime::get_now_timestamp_ms();
    }
    msg.m_nOrderNum = req_vec.size();

    m_client->order(&msg, nReqId);
    return nReqId;
};

int TiGtTraderClient::orderInsert(TiReqOrderInsert* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    auto account_iter = m_account_map.find(req->szAccount);
    if (account_iter == m_account_map.end())
    {
        return -1;
    }

    
    req->nReqId = ++nReqId;

    COrdinaryOrder msg;

    // 资金账号，必填参数。不填会被api打回，并且通过onOrder反馈失败
    strcpy(msg.m_strAccountID, req->szAccount);
    // 报单市场。必填字段。股票市场有"SH"/"SZ"，如果填空或填错都会被api直接打回
    strcpy(msg.m_strMarket, req->szExchange);
    // 报单合约代码，必填字段。
    strcpy(msg.m_strInstrument, req->szSymbol);
    // 报单委托量，必填字段。默认int最大值，填0或不填会被api打回
    msg.m_nVolume = req->nOrderVol;
    // 报单委托类型。必填字段。根据相应的业务选择，具体请参考XtDataType.h，默认为无效值(OPT_INVALID)。不填会被api打回
    switch (req->nTradeSideType)
    {
    case TI_TradeSideType_Sell:
        msg.m_eOperationType = OPT_SELL; 
        break;
    case TI_TradeSideType_Buy:
        msg.m_eOperationType = OPT_BUY; 
        break;
    case TI_TradeSideType_Purchase:
        msg.m_eOperationType = OPT_ETF_PURCHASE; 
        break;
    case TI_TradeSideType_Redemption:
        msg.m_eOperationType = OPT_ETF_REDEMPTION; 
        break;
    }
    // 报单价格类型，必填字段。默认为无效(PTRP_INVALID)，具体可参考XtDataType.h
    msg.m_ePriceType = PRTP_FIX;
    // 报单价格，默认为double最大值。当价格类型m_ePriceType为指定价PRTP_FIX时，必填字段。当价格类型为其他时填了也没用
    msg.m_dPrice = req->nOrderPrice;
    // 投机套保标志，选填字段。有"投机"/"套利"/"套保"方式。除期货三个方式都可选之外都是填“投机”。默认为“投机”
    msg.m_eHedgeFlag = HEDGE_FLAG_SPECULATION;

    // 投资备注
    strcpy(msg.m_strRemark, req->szUseStr);
    
    std::shared_ptr<TiRtnOrderStatus> order = std::make_shared<TiRtnOrderStatus>();
    memcpy(order.get(), req, sizeof(TiReqOrderInsert));
    m_order_req_map[order->nReqId] = order;
    order->nReqTimestamp = datetime::get_now_timestamp_ms();

    m_client->directOrder(&msg, nReqId);

    return nReqId;
};

int TiGtTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

TiRtnOrderStatus* TiGtTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    return NULL;
};

int TiGtTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

int TiGtTraderClient::QueryOrders()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    ++nReqId;
    auto account_iter = m_account_map.begin();
    for (; account_iter != m_account_map.end(); account_iter++)
    {
        m_client->reqOrderDetail(account_iter->first.c_str(), nReqId);
    }
    
    return nReqId;
};

int TiGtTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    ++nReqId;
    auto account_iter = m_account_map.begin();
    for (; account_iter != m_account_map.end(); account_iter++)
    {
        m_client->reqDealDetail(account_iter->first.c_str(), nReqId);
    }
    
    return nReqId;
};

int TiGtTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    ++nReqId;
    auto account_iter = m_account_map.begin();
    for (; account_iter != m_account_map.end(); account_iter++)
    {
        m_client->reqPositionDetail(account_iter->first.c_str(), nReqId);
    }

    return nReqId;
};
