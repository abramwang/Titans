#ifndef TI_GT_TRADER_CLIENT_H
#define TI_GT_TRADER_CLIENT_H

#include <string>
#include <memory>
#include <vector>
#include <uv.h>

#include "XtDataType.h"
#include "MarketType.h"
#include "XtTraderApi.h"

#include "ti_trader_client.h"
#include "ti_trader_callback.h"

#include "ti_gt_trader_account.h"

using namespace xti;

class TiGtTraderClient: 
    public XtTraderApiCallback, public TiTraderClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szLocations;

        std::string szUser;
        std::string szPass;

        std::string m_strStockAccount;   // 股票账号
        std::string m_strFutureAccount;  // 期货账号
        std::string m_strOptionAccount;  // 股票期权账号
        std::string m_strCreditAccount;  // 信用账号
    } ConfigInfo;
    
private:
    int nReqId;
    int64_t nOrderId;
    ConfigInfo* m_config;
    unsigned int m_trading_day;

    uv_work_t m_work_req;

    XtTraderApi* m_client;

    std::unordered_map<int64_t, std::shared_ptr<TiRtnOrderStatus>> m_order_req_map;     //req_id, order_status
    std::multimap<int64_t, std::shared_ptr<TiRtnOrderStatus>> m_order_batch_req_map;    //req_id, batch_order_status
    std::unordered_map<std::string, std::shared_ptr<TiGtTraderAccount>> m_account_map;  //account_id, account

    TiTraderCallback* m_cb;
public:
    TiGtTraderClient(std::string configPath, TiTraderCallback* userCb);
    virtual ~TiGtTraderClient();

public:
    // 回调方法
    static void work_cb(uv_work_t* req);
    static void after_work_cb(uv_work_t* req, int status);
public:
    // 建立连接的回调
    // @param   success 反馈是否成功与服务器建立连接
    // @param   errorMsg 反馈连接失败时具体的错误信息
    virtual void onConnected(bool success, const char* errorMsg);

    // 用户登录的回馈
    // @param   userName 用户名
    // @param   password 用户对应的密码
    // @param   nRequestId 客户自己维护的请求号
    // @param   error 反馈登录信息，error的isSuccess可判断是否登录成功
    virtual void onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error);

    // 资金账号的主推信息
    // @param   accountId 资金账户
    // @param   status 表明这个资金账号目前在迅投系统的登录状态
    // @param   brokerType 表明这个资金账号的类型，1:期货，2:股票，3:信用，6:股票期权
    // @param   errorMsg 如果有错误，通过errorMsg反馈错误信息
    virtual void onRtnLoginStatus(const char* accountId, EBrokerLoginStatus status, int brokerType, const char* errorMsg);


    // 资金账号的信息
    // @param   accountId 资金账户
    // @param   nRequestId 客户自己维护的请求号
    // @param   data 资金账号的具体信息，具体可参考XtStructs.h
    // @param   isLast 是否为返回最后一条记录，一次请求的结果一般只有一条资金账号信息
    // @param   error 反馈这次查询请求是否有错误
    virtual void onReqAccountDetail(const char* accountId, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error);
    virtual void onReqSecuAccount(const char* accountID, int nRequestId, const char* accountKey, const CSecuAccount* data, bool isLast, const XtError& error);

    virtual void onOrder(int nRequestId, int orderID, const char* strRemark, const XtError& error);
    virtual void onRtnOrder(const COrderInfo* data);

    virtual void onDirectOrder(int nRequestId, const char* strOrderSysID, const char* strRemark, const XtError& error);

    // 委托回报的主推信息
    // @param   data COrderDetail
    virtual void onRtnOrderDetail(const COrderDetail* data);

    // 成交回报的主推信息
    // @param   data 具体信息有CDealDetail携带
    virtual void onRtnDealDetail(const CDealDetail* data);

    // 委托错误的主推信息
    // @param   data 具体信息有COrderError携带
    virtual void onRtnOrderError(const COrderError* data);



private:
    int loadConfig(std::string iniFileName);

    TI_OrderStatusType convertOrderStatus(EEntrustStatus status);
    TI_TradeSideType convertTradeSide(EOperationType operation);
    
public:
	void connect();
    int orderInsertBatch(std::vector<TiReqOrderInsert> &req_vec, std::string account_id);
    int orderInsert(TiReqOrderInsert* req);
    int orderDelete(TiReqOrderDelete* req);
    TiRtnOrderStatus* getOrderStatus(int64_t req_id, int64_t order_id);
    
    
    int QueryAsset();
    int QueryOrders();
    int QueryMatches();
    int QueryPositions();

    int QueryETFFile();
    int QueryETFBasket();
    
};

#endif

