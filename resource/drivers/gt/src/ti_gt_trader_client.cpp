#include "ti_gt_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"
#include <thread>


#include <atomic>

TiGtTraderClient::TiGtTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_client = NULL;

    nOrderId = 1680000;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 100;   //跳过xtp client设置成交模式的区段
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
        {"apt_type", "gt"},
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
        account_iter->second->OnCommonJsonRespones(&rspData, nRequestId, data, isLast, error.errorMsg());
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
}

void TiGtTraderClient::onOrder(int nRequestId, int orderId, const char* strRemark, const XtError& error)
{
    std::thread::id threadId = std::this_thread::get_id();
    std::cout << "TiGtTraderClient::onOrder" << "Current thread ID: " << threadId << std::endl;
    cout << "[onOrder] isSuccess: " << (error.isSuccess()?"true":"false")
        << "\n    orderId:  " << orderId
        << "\n    RequestId: " << nRequestId  
        << "\n    errorMsg: " << error.errorMsg()
        << endl;
}

void TiGtTraderClient::onRtnOrder(const COrderInfo* data)
{
    auto account_iter = m_account_map.find(data->m_strAccountID);
    if (account_iter == m_account_map.end())
    {
        return;
    }
    

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

    std::thread::id threadId = std::this_thread::get_id();
    std::cout << "TiGtTraderClient::onRtnOrder" << "Current thread ID: " << threadId << std::endl;
    cout << "[onRtnOrder]"
        << "\n    下单ID: " << data->m_nOrderID
        << "\n    m_startTime：" << data->m_startTime
        << "\n    m_endTime: " << data->m_endTime
        << "\n    指令状态：" << orderStatus
        << "\n    成交量：" << data->m_dTradedVolume
        << "\n    撤销者：" << data->m_canceler
        << "\n    指令执行信息：" << data->m_strMsg
        << endl;
}

void TiGtTraderClient::onRtnOrderDetail(const COrderDetail* data)
{
    string entrust_status;
    switch(data->m_eOrderStatus)
    {
    case ENTRUST_STATUS_UNREPORTED:  entrust_status = "未报";  break;
    case ENTRUST_STATUS_WAIT_REPORTING:  entrust_status = "待报"; break;
    case ENTRUST_STATUS_REPORTED:        entrust_status = "已报"; break;
    case ENTRUST_STATUS_REPORTED_CANCEL: entrust_status = "已报待撤";  break;
    case ENTRUST_STATUS_PARTSUCC_CANCEL: entrust_status = "部成待撤";  break;
    case ENTRUST_STATUS_PART_CANCEL:     entrust_status = "部撤";  break;
    case ENTRUST_STATUS_CANCELED:        entrust_status = "已撤";  break;
    case ENTRUST_STATUS_PART_SUCC:       entrust_status = "部成";  break;
    case ENTRUST_STATUS_SUCCEEDED:       entrust_status = "已成";  break;
    case ENTRUST_STATUS_JUNK:            entrust_status = "废单";  break;
    }
    if (data == NULL)
    {
        return;
    }
    std::thread::id threadId = std::this_thread::get_id();
    std::cout << "TiGtTraderClient::onRtnOrderDetail" << "Current thread ID: " << threadId << std::endl;
    cout << "[onRtnOrderDetail]"
        << "\n    委托号：" << data->m_strOrderSysID
        << "\n    委托状态：" << entrust_status
        << "\n    已成交量：" << data->m_nTradedVolume 
        << "\n    成交均价：" << data->m_dAveragePrice 
        << "\n    成交额: " << data->m_dTradeAmount 
        << "\n    市场ID：" << data->m_strExchangeID
        << "\n    产品ID：" << data->m_strProductID
        << "\n    股票/期货代码：" << data->m_strInstrumentID
        << "\n    冻结保证金：" << data->m_dFrozenMargin
        << "\n    冻结手续费：" << data->m_dFrozenCommission
        << "\n    ErrorID：" << data->m_nErrorID
        << "\n    ErrorMsg: " << data->m_strErrorMsg
        << endl;

}

void TiGtTraderClient::onRtnDealDetail(const CDealDetail* data)
{
    if (data == NULL)
    {
        return;
    }
    std::thread::id threadId = std::this_thread::get_id();
    std::cout << "TiGtTraderClient::onRtnDealDetail" << "Current thread ID: " << threadId << std::endl;
    cout << "[onRtnDealDetail]"
        << "\n    orderId: " << data->m_nOrderID 
        << "\n    成交量： " << data->m_nVolume
        << "\n    成交额: " << data->m_dAmount
        << "\n    成交均价： " << data->m_dAveragePrice
        << endl;
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

TI_OrderStatusType TiGtTraderClient::getOrderStatus(EOrderCommandStatus status)
{
    switch(status)
    {
    case ENTRUST_STATUS_UNREPORTED:
        return TI_OrderStatusType_unAccept;
    case ENTRUST_STATUS_WAIT_REPORTING:
        return TI_OrderStatusType_accepted;
    case ENTRUST_STATUS_REPORTED:
        return TI_OrderStatusType_accepted;
    case ENTRUST_STATUS_REPORTED_CANCEL:
        return TI_OrderStatusType_toRemove;
    case ENTRUST_STATUS_PARTSUCC_CANCEL:
        return TI_OrderStatusType_removing;
    case ENTRUST_STATUS_PART_CANCEL:
        return TI_OrderStatusType_removed;
    case ENTRUST_STATUS_CANCELED:
        return TI_OrderStatusType_removed;
    case ENTRUST_STATUS_PART_SUCC:
        return TI_OrderStatusType_queued;
    case ENTRUST_STATUS_SUCCEEDED:
        return TI_OrderStatusType_queued;
    case ENTRUST_STATUS_JUNK:
        return TI_OrderStatusType_fail;
    default:
        return TI_OrderStatusType_unAccept;
    }
   return TI_OrderStatusType_unAccept;
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
    strcpy(msg.m_strRemark, "ti_gt_trader_client");
    
    account_iter->second->enterOrder(req);

    m_client->order(&msg, nReqId);

    return nReqId;
};

int TiGtTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    TiRtnOrderStatus* order = getOrderStatus(-1, req->nOrderId);
    
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
    
    return nReqId;
};

int TiGtTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    return nReqId;
};

int TiGtTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};


int TiGtTraderClient::QueryETFFile()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

int TiGtTraderClient::QueryETFBasket()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    return nReqId;
};

