#ifndef TI_ZT_QUOTE_CLIENT_H
#define TI_ZT_QUOTE_CLIENT_H

#include <string>
#include "xtp_quote_api.h"
#include "ti_quote_client.h"
#include "ti_quote_callback.h"

using namespace XTP::API;

class TiZtQuoteClient: 
    public QuoteSpi, public TiQuoteClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int nPort;
        std::string szLocalIp;
        bool bEnableUDP;

        std::string szUser;
        std::string szPass;
    } ConfigInfo;
    
    typedef struct LoginStatus
    {
        int nReqId;
        bool bLoginSuccess;
    } LoginStatus;

private:
    int nReqId;
    ConfigInfo* m_config;
    unsigned int m_trading_day;

    QuoteApi*   m_api;

    TiQuoteCallback* m_cb;
public:
    TiZtQuoteClient(std::string configPath, TiQuoteCallback* userCb);
    virtual ~TiZtQuoteClient();

public:
    ///当客户端与行情后台通信连接断开时，该方法被调用。
    virtual void OnDisconnected(int reason);

    ///错误应答
    virtual void OnError(XTPRI *error_info);

    ///订阅行情应答，包括股票、指数和期权
    virtual void OnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

    ///退订行情应答，包括股票、指数和期权
    virtual void OnUnSubMarketData(XTPST *ticker, XTPRI *error_info, bool is_last);

    ///深度行情通知，包含买一卖一队列
    virtual void OnDepthMarketData(XTPMD *market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count);

    ///订阅行情订单簿应答，包括股票、指数和期权
    virtual void OnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last);

    ///退订行情订单簿应答，包括股票、指数和期权
    virtual void OnUnSubOrderBook(XTPST *ticker, XTPRI *error_info, bool is_last);

    ///行情订单簿通知，包括股票、指数和期权
    virtual void OnOrderBook(XTPOB *order_book);

    ///订阅逐笔行情应答，包括股票、指数和期权
    virtual void OnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last);

    ///退订逐笔行情应答，包括股票、指数和期权
    virtual void OnUnSubTickByTick(XTPST *ticker, XTPRI *error_info, bool is_last);

    ///逐笔行情通知，包括股票、指数和期权
    virtual void OnTickByTick(XTPTBT *tbt_data);

private:
    int loadConfig(std::string iniFileName);
public:
	void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif

