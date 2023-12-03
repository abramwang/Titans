#ifndef EES_TRADE_CLIENT_H
#define EES_TRADE_CLIENT_H
#include <string>
#include <EesTraderApi.h>
#include "trade_responser.h"

class EESTraderClient : public EESTraderEvent
{
public:
	typedef struct ConfigInfo
    {
        std::string	szBlock;
        int 		nBrokerId;

        std::string szUser;
        std::string szUserPass;
		std::string szAppId;
		std::string szAuthCode;

        std::string szTradeIp;
        int 		nTradePort;
        std::string szQueryIp;
        int 		nQueryPort;
    } ConfigInfo;
private:
	EESTraderApi* api;
    ConfigInfo* config;
	TradeResponser* responser;

public:
	EESTraderClient(std::string configPath);
	virtual ~EESTraderClient();
	
	virtual void OnConnection(ERR_NO errNo, const char* pErrStr );
	virtual void OnDisConnection(ERR_NO errNo, const char* pErrStr );
	/// 登录消息的回调
	virtual void OnUserLogon(EES_LogonResponse* pLogon);
	/// 修改密码响应回调
	virtual void OnRspChangePassword(EES_ChangePasswordResult nResult);
	/// 查询用户下面帐户的返回事件
	virtual void OnQueryUserAccount(EES_AccountInfo * pAccoutnInfo, bool bFinish);
	/// 查询帐户下面期货仓位信息的返回事件	
	virtual void OnQueryAccountPosition(const char* pAccount, EES_AccountPosition* pAccoutnPosition, int nReqId, bool bFinish);
	/// 查询帐户下面期权仓位信息的返回事件, 注意这个回调, 和上一个OnQueryAccountPosition, 会在一次QueryAccountPosition请求后, 分别返回, 先返回期货, 再返回期权, 即使没有期权仓位, 也会返回一条bFinish=true的记录
	virtual void OnQueryAccountOptionPosition(const char* pAccount, EES_AccountOptionPosition* pAccoutnOptionPosition, int nReqId, bool bFinish);
	/// 查询帐户下面资金信息的返回事件
	virtual void OnQueryAccountBP(const char* pAccount, EES_AccountBP* pAccoutnBP, int nReqId);
	/// 发生出入金后主动报告资金信息事件
	virtual void OnAccountBPReport(const char* pAccount, EES_AccountBP* pAccoutnBP);
	/// 查询合约列表的返回事件
	virtual void OnQuerySymbol(EES_SymbolField* pSymbol, bool bFinish);
	/// 查询帐户交易保证金的返回事件
	virtual void OnQueryAccountTradeMargin(const char* pAccount, EES_AccountMargin* pSymbolMargin, bool bFinish );
	/// 查询帐户交易费用的返回事件
	virtual void OnQueryAccountTradeFee(const char* pAccount, EES_AccountFee* pSymbolFee, bool bFinish );
	/// 下单被柜台系统接受的事件
	virtual void OnOrderAccept(EES_OrderAcceptField* pAccept);
	/// 下单被市场接受的事件
	virtual void OnOrderMarketAccept(EES_OrderMarketAcceptField* pAccept);
	///	下单被柜台系统拒绝的事件
	virtual void OnOrderReject(EES_OrderRejectField* pReject );
	///	下单被市场拒绝的事件
	virtual void OnOrderMarketReject(EES_OrderMarketRejectField* pReject);
	///	订单成交的消息事件
    /// \brief	成交里面包括了订单市场ID，建议用这个ID查询对应的订单
	virtual void OnOrderExecution(EES_OrderExecutionField* pExec);
	///	订单成功撤销事件
    /// \brief	成交里面包括了订单市场ID，建议用这个ID查询对应的订单
	virtual void OnOrderCxled(EES_OrderCxled* pCxled );
	///	撤单被拒绝的消息事件
    /// \brief	一般会在发送撤单以后，收到这个消息，表示撤单被拒绝
	virtual void OnCxlOrderReject(EES_CxlOrderRej* pReject );
	///	查询订单的返回事件
    /// \brief	查询订单信息时候的回调，这里面也可能包含不是当前用户下的订单
	virtual void OnQueryTradeOrder(const char* pAccount, EES_QueryAccountOrder* pQueryOrder, bool bFinish);
	///	查询订单的返回事件
	/// \brief	查询订单信息时候的回调，这里面也可能包含不是当前用户下的订单成交
	virtual void OnQueryTradeOrderExec(const char* pAccount, EES_QueryOrderExecution* pQueryOrderExec, bool bFinish);
	///	接收外部订单的消息
    /// \brief	一般会在系统订单出错，进行人工调整的时候用到。
	virtual void OnPostOrder(EES_PostOrder* pPostOrder );
	///	接收外部订单成交的消息
    /// \brief	一般会在系统订单出错，进行人工调整的时候用到。
	virtual void OnPostOrderExecution(EES_PostOrderExecution* pPostOrderExecution );
	///	查询交易所可用连接的响应
	/// \brief	每个当前系统支持的汇报一次，当bFinish= true时，表示所有交易所的响应都已到达，但本条消息本身不包含有用的信息。
	virtual void OnQueryMarketSession(EES_ExchangeMarketSession* pMarketSession, bool bFinish);
	///	交易所连接状态变化报告，
	/// \brief	当交易所连接发生连接/断开时报告此状态
	virtual void OnMarketSessionStatReport(EES_MarketSessionId MarketSessionId, bool ConnectionGood);
	///	合约状态变化报告
	/// \brief	当合约状态发生变化时报告
	virtual void OnSymbolStatusReport(EES_SymbolStatus* pSymbolStatus);
	///	合约状态查询响应
	/// \brief  响应合约状态查询请求
	virtual void OnQuerySymbolStatus(EES_SymbolStatus* pSymbolStatus, bool bFinish);
	/// 深度行情查询响应
	virtual void OnQueryMarketMBLData(EES_MarketMBLData* pMarketMBLData, bool bFinish);
	/// 席位代号查询响应
	virtual void OnQuerySessionID(EES_SessionID* pSessionID);
private:
    int loadConfig(std::string iniFileName);
public:
	void connect();
	void logon();
	void getMaxToken(OUT EES_ClientToken* orderToken);
	void querySymbolList();
	void queryUserAccount();
	void queryAccountPosition(const char* accountId, int nReqId);
	void queryAccountBP(const char* accountId, int nReqId);
	void queryAccountTradeMargin(const char* accountId);
	void queryAccountTradeFee(const char* accountId);
	void enterOrder(EES_EnterOrderField* pOrder);
	void cancelOrder(EES_CancelOrder* pOrder);
	void queryAccountOrder(const char* accountId);
	void queryAccountOrderExecution(const char* accountId); 

	void queryMarketMBLData(int nRequestId, EES_ExchangeID exchangeID, int nSide);

};

#endif