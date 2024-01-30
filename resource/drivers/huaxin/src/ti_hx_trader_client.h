#ifndef TI_HX_TRADER_CLIENT_H
#define TI_HX_TRADER_CLIENT_H

#include <string>
#include <memory>
#include "TORATstpTraderApi.h"
#include "ti_trader_client.h"
#include "ti_trader_callback.h"

using namespace TORASTOCKAPI;

class TiHxTraderClient: 
    public CTORATstpTraderSpi, public TiTraderClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szLocations;

        std::string szUser;
        std::string szPass;
        
        std::string szUserProductInfo;

        std::string szDeviceID;
        std::string szCertSerial;
        std::string szProductInfo;

        std::string szTerminalInfo;

        std::string szInvestorIDSH;         //资金账号
        std::string szInvestorIDSZ;         //资金账号密码
        std::string szShareholderIdSH;      //沪市股东号
        std::string szShareholderIdSZ;      //深市股东号
    } ConfigInfo;
    
private:
    int nReqId;
    ConfigInfo* m_config;
    unsigned int m_trading_day;

    CTORATstpTraderApi* m_client;
    std::map<int32_t, int32_t> m_report_sync;
    unsigned long long nSessionId;
    int64_t nOrderId;

    json m_json_rsp;
    std::unordered_map<int64_t, std::shared_ptr<TiRtnOrderStatus>> m_order_map;         //order_id, order_status
    std::multimap<int64_t, std::shared_ptr<TiRtnOrderMatch>> m_matches_map;             //order_id, m_matches_map

    TiTraderCallback* m_cb;
public:
    TiHxTraderClient(std::string configPath, TiTraderCallback* userCb);
    virtual ~TiHxTraderClient();

public:
    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    virtual void OnFrontConnected();
    
    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    ///@param nReason 错误原因
    ///        -3 连接已断开
    ///        -4 网络读失败
    ///        -5 网络写失败
    ///        -6 订阅流错误
    ///        -7 流序号错误
    ///        -8 错误的心跳报文
    ///        -9 错误的报文
    ///		  -15 网络读失败
    ///		  -16 网络写失败
    virtual void OnFrontDisconnected(int nReason);
    
    ///错误应答
    virtual void OnRspError(CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {};
    
    ///获取连接信息应答
    virtual void OnRspGetConnectionInfo(CTORATstpConnectionInfoField *pConnectionInfoField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///登录响应
    virtual void OnRspUserLogin(CTORATstpRspUserLoginField *pRspUserLoginField, CTORATstpRspInfoField *pRspInfoField, int nRequestID);
    
    ///登出响应
    virtual void OnRspUserLogout(CTORATstpUserLogoutField *pUserLogoutField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///修改密码响应
    virtual void OnRspUserPasswordUpdate(CTORATstpUserPasswordUpdateField *pUserPasswordUpdateField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///录入设备序列响应
    virtual void OnRspInputDeviceSerial(CTORATstpRspInputDeviceSerialField *pRspInputDeviceSerialField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///报单录入响应
    virtual void OnRspOrderInsert(CTORATstpInputOrderField *pInputOrderField, CTORATstpRspInfoField *pRspInfoField, int nRequestID);
    
    ///报单回报
    virtual void OnRtnOrder(CTORATstpOrderField *pOrderField) {};
    
    ///报单错误回报
    virtual void OnErrRtnOrderInsert(CTORATstpInputOrderField *pInputOrderField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///成交回报
    virtual void OnRtnTrade(CTORATstpTradeField *pTradeField) {};
    
    ///撤单响应
    virtual void OnRspOrderAction(CTORATstpInputOrderActionField *pInputOrderActionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///撤单错误回报
    virtual void OnErrRtnOrderAction(CTORATstpInputOrderActionField *pInputOrderActionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///市场状态回报
    virtual void OnRtnMarketStatus(CTORATstpMarketStatusField *pMarketStatusField) {};
    
    ///查询集中交易系统资金响应
    virtual void OnRspInquiryJZFund(CTORATstpRspInquiryJZFundField *pRspInquiryJZFundField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///查询银行账户余额响应
    virtual void OnRspInquiryBankAccountFund(CTORATstpRspInquiryBankAccountFundField *pRspInquiryBankAccountFundField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///交易通知回报
    virtual void OnRtnTradingNotice(CTORATstpTradingNoticeField *pTradingNoticeField) {};
    
    ///查询最大报单量响应
    virtual void OnRspInquiryMaxOrderVolume(CTORATstpRspInquiryMaxOrderVolumeField *pRspInquiryMaxOrderVolumeField, CTORATstpRspInfoField *pRspInfoField, int nRequestID) {};
    
    ///查询交易所响应
    virtual void OnRspQryExchange(CTORATstpExchangeField *pExchangeField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询证券信息响应
    virtual void OnRspQrySecurity(CTORATstpSecurityField *pSecurityField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询用户响应
    virtual void OnRspQryUser(CTORATstpUserField *pUserField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询投资者响应
    virtual void OnRspQryInvestor(CTORATstpInvestorField *pInvestorField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询股东账户响应
    virtual void OnRspQryShareholderAccount(CTORATstpShareholderAccountField *pShareholderAccountField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast); 
    
    ///查询报单响应
    virtual void OnRspQryOrder(CTORATstpOrderField *pOrderField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast); 
    
    ///查询撤单请求
    virtual void OnRspQryOrderAction(CTORATstpOrderActionField *pOrderActionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询成交响应
    virtual void OnRspQryTrade(CTORATstpTradeField *pTradeField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast); 
    
    ///查询资金账户响应
    virtual void OnRspQryTradingAccount(CTORATstpTradingAccountField *pTradingAccountField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast); 
    
    ///查询投资者持仓响应
    virtual void OnRspQryPosition(CTORATstpPositionField *pPositionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast); 
    
    ///查询基础交易费率响应
    virtual void OnRspQryTradingFee(CTORATstpTradingFeeField *pTradingFeeField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询佣金费率响应
    virtual void OnRspQryInvestorTradingFee(CTORATstpInvestorTradingFeeField *pInvestorTradingFeeField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询交易通知响应
    virtual void OnRspQryTradingNotice(CTORATstpTradingNoticeField *pTradingNoticeField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询市场响应
    virtual void OnRspQryMarket(CTORATstpMarketField *pMarketField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询ETF清单信息响应
    virtual void OnRspQryETFFile(CTORATstpETFFileField *pETFFileField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询ETF成份证券信息响应
    virtual void OnRspQryETFBasket(CTORATstpETFBasketField *pETFBasketField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询深港通最小价差信息响应
    virtual void OnRspQrySZSEHKPriceTickInfo(CTORATstpSZSEHKPriceTickInfoField *pSZSEHKPriceTickInfoField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询系统节点信息响应
    virtual void OnRspQrySystemNodeInfo(CTORATstpSystemNodeInfoField *pSystemNodeInfoField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
    
    ///查询标准券额度响应
    virtual void OnRspQryStandardBondPosition(CTORATstpStandardBondPositionField *pStandardBondPositionField, CTORATstpRspInfoField *pRspInfoField, int nRequestID, bool bIsLast) {}; 
        

private:
    int loadConfig(std::string iniFileName);

    TI_OrderStatusType getOrderStatus(TTORATstpOrderStatusType status);

    int orderInsertStock(TiReqOrderInsert* req);   //买卖
    int orderInsertEtf(TiReqOrderInsert* req);     //申赎
public:
	void connect();
    int orderInsert(TiReqOrderInsert* req);
    int orderDelete(TiReqOrderDelete* req);
    TiRtnOrderStatus* getOrderStatus(int64_t req_id, int64_t order_id);
    
    
    int QueryAsset();
    int QueryOrders();
    int QueryMatches();
    int QueryPositions();
    
};

#endif

