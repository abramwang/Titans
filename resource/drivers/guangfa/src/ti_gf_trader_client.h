#ifndef TI_AX_TRADER_CLIENT_H
#define TI_AX_TRADER_CLIENT_H

#include <string>
#include <memory>
#include "atp_trade_api.h"
#include "ti_trader_client.h"
#include "ti_trader_callback.h"

class TiGfTraderClient: 
    public ATPTradeHandler, public TiTraderClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szLocations;
        std::string szLocations2;

        std::string szUser;
        std::string szPass;
        
        std::string szSoftwareName;
        std::string szSoftwareVersion;

        std::string szBranchCode;   //营业部编号
        std::string szCustomerId;   //客户代码
        std::string szFundAccount;  //资金账号
        std::string szFundPass;     //资金账号密码
        std::string szShareholderIdSH;     //沪市股东号
        std::string szShareholderIdSZ;     //深市股东号
    } ConfigInfo;
    
private:
    int nReqId;
    ConfigInfo* m_config;
    unsigned int m_trading_day;

    ATPTradeAPI* m_client;
    std::map<int32_t, int32_t> m_report_sync;
    unsigned long long nSessionId;

    json m_json_rsp;
    std::unordered_map<int64_t, std::shared_ptr<TiRtnOrderStatus>> m_order_req_map;     //req_id, order_status
    std::unordered_map<int64_t, std::shared_ptr<TiRtnOrderStatus>> m_order_map;         //order_id, order_status
    std::multimap<int64_t, std::shared_ptr<TiRtnOrderMatch>> m_matches_map;             //order_id, m_matches_map

    TiTraderCallback* m_cb;
public:
    TiGfTraderClient(std::string configPath, TiTraderCallback* userCb);
    virtual ~TiGfTraderClient();

public:
    virtual void OnLogin(const std::string& reason);

    // 登出回调
    virtual void OnLogout(const std::string& reason);

    // 连接失败
    virtual void OnConnectFailure(const std::string &reason);

    // 连接超时
    virtual void OnConnectTimeOut(const std::string &reason);

    // 心跳超时
    virtual void OnHeartbeatTimeout(const std::string &reason);

    // 连接关闭
    virtual void OnClosed(const std::string &reason);
	
    // 连接结束回调
    virtual void OnEndOfConnection(const std::string& reason);

    // 客户号登入回调
    virtual void OnRspCustLoginResp(const ATPRspCustLoginRespOtherMsg &cust_login_resp);
    // 客户号登出回调
    virtual void OnRspCustLogoutResp(const ATPRspCustLogoutRespOtherMsg &cust_logout_resp);

    /**
	 * @brief 资金查询结果
	 * @param[in] fund_query_result 资金查询结果消息
	 */
	virtual void OnRspFundQueryResult(const ATPRspFundQueryResultMsg& fund_query_result);
    /**
	 * @brief 股份查询结果
	 * @param[in] share_query_result 股份查询结果消息
	 */
	virtual void OnRspShareQueryResult(const ATPRspShareQueryResultMsg& share_query_result);
	/**
	 * @brief 订单查询结果
	 * @param[in] order_query_result 订单查询结果消息
	 */
	virtual void OnRspOrderQueryResult(const ATPRspOrderQueryResultMsg& order_query_result) ;
    /**
     * @brief ETF申赎成交查询结果
     * @param[in] ATPRspETFTradeOrderQueryResultMsg
     * @type query
     */
    virtual void OnRspEtfTradeOrderQueryResult(const ATPRspETFTradeOrderQueryResultMsg &trade_order_query_result);
	/**
	 * @brief 成交查询结果
	 * @param[in] trade_order_query_result 成交查询结果消息
	 */
	virtual void OnRspTradeOrderQueryResult(const ATPRspTradeOrderQueryResultMsg& trade_order_query_result);





    // 订单下达内部响应
    virtual void OnRspOrderStatusInternalAck(const ATPRspOrderStatusAckMsg& order_status_ack);

    // 订单下达交易所确认
    virtual void OnRspOrderStatusAck(const ATPRspOrderStatusAckMsg& order_status_ack);

    // 成交回报
    virtual void OnRspCashAuctionTradeER(const ATPRspCashAuctionTradeERMsg& cash_auction_trade_er);

    // 申赎成交回报
    virtual void OnRspETFRedemptionTradeER(const ATPRspETFRedemptionTradeERMsg& etf_redemption_trade_er);
	

    // 订单下达内部拒绝
    virtual void OnRspBizRejection(const ATPRspBizRejectionOtherMsg& biz_rejection);


    void onOrderRtn(const ATPRspOrderStatusAckMsg& msg);
    void onCancelOrderRtn(const ATPRspOrderStatusAckMsg& msg);
private:
    int loadConfig(std::string iniFileName);
    bool init_encrypt();
    ATPRetCodeType connect(ATPTradeAPI* client, ATPTradeHandler* handler);
    ATPRetCodeType close(ATPTradeAPI* client);
    ATPRetCodeType login(ATPTradeAPI* client);
    ATPRetCodeType logout(ATPTradeAPI* client);
    bool init(ATPTradeAPI* client, ATPTradeHandler* handler);
    TI_OrderStatusType getOrderStatus(ATPExecTypeType status);
    TI_OrderStatusType getOrderStatus(ATPOrdStatusType status);

    
    std::shared_ptr<TiRtnOrderStatus> getOrderPtr(int64_t req_id, int64_t order_id);
    void updateOrderMap(std::shared_ptr<TiRtnOrderStatus> order_ptr);
    void updateOrderMatch(std::shared_ptr<TiRtnOrderMatch> match_ptr);

    int orderInsertStock(TiReqOrderInsert* req);   //买卖
    int orderInsertEtf(TiReqOrderInsert* req);     //申赎
    int queryOrders(int64_t start_index);
    int queryEtfOrders(int64_t start_index);
    int queryMatches(int64_t start_index);
public:
	void connect();
    int orderInsertBatch(std::vector<TiReqOrderInsert> &req_vec, std::string account_id){return -1;};
    int orderInsert(TiReqOrderInsert* req);
    int orderDelete(TiReqOrderDelete* req);
    TiRtnOrderStatus* getOrderStatus(int64_t req_id, int64_t order_id);
    
    
    int QueryAsset();
    int QueryOrders();
    int QueryMatches();
    int QueryPositions();
    
};

#endif

