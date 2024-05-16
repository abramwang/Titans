#ifndef __IA_ETF_TRADE_WORKER_CENTER_H__
#define __IA_ETF_TRADE_WORKER_CENTER_H__

#include "ti_trader_client.h"
#include "ti_trader_callback.h"

#include "ti_gt_trader_client.h"
#include "ia_etf_user_setting.h"
#include "ia_etf_quote_data_cache.h"
#include "ia_etf_signal_center.h"

#include <map>

#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaEtfTradeWorkerCenter
    : public TiTraderCallback
{

public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){};     //非交易逻辑的统一实现接口
    
    virtual void OnRspAccountInfo(const TiRspAccountInfo* pData);

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData){};
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast);

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData){};
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

private:
    TiGtTraderClient* m_trade_client;
    IaEtfUserSetting* m_user_setting;
    IaEtfQuoteDataCache* m_quote_cache;
    IaEtfSignalCenter* m_signal_center;

    std::map<std::string, TiRspAccountInfo> m_account_info_map;
    std::map<std::string, std::map<std::string, TiRspQryPosition>>  m_account_position_map;
private:
    bool get_position(const std::string &account, const std::string &symbol, TiRspQryPosition &position);
    void create_trading_worker(const std::string &symbol, const std::string &account, TI_TradeSideType side, std::shared_ptr<IaEtfSignalFactor> etf_factor);

public:
    IaEtfTradeWorkerCenter(TiGtTraderClient* trade_client, 
        IaEtfQuoteDataCache* quote_cache, 
        IaEtfUserSetting* user_setting,
        IaEtfSignalCenter* signal_center);
    ~IaEtfTradeWorkerCenter();

public:
    void OnTradingSignal(json &msg);

};

#endif // __IA_ETF_TRADE_WORKER_CENTER_H__