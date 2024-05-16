#ifndef __IA_ETF_SIGNAL_CENTER_H__
#define __IA_ETF_SIGNAL_CENTER_H__

#include <unordered_map>

#include "ti_quote_callback.h"
#include "ia_etf_user_setting.h"
#include "ia_etf_quote_data_cache.h"

#include "ia_etf_signal_factor.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaEtfSignalCenter 
    : public TiQuoteCallback
{
/*   行情回调   */
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};
    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){};
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

/*   定时器   */
public:
    virtual void OnTimer();

private:
    IaEtfUserSetting* m_etf_user_setting;
    IaEtfQuoteDataCache* m_quote_data_cache;

    std::unordered_map<std::string, std::shared_ptr<IaEtfSignalFactor>> m_etf_signal_factor_map;

    json m_out;     //输出数据

private:
    void init_etf_signal_factor();
public:
    IaEtfSignalCenter(IaEtfUserSetting* etf_user_setting, IaEtfQuoteDataCache* etf_quote_data_cache);
    ~IaEtfSignalCenter();

public:
    void GetJsonOut(json& j);
    bool GetEtfFactor(const std::string etf_symbol, std::shared_ptr<IaEtfSignalFactor> &etf_factor);
};

#endif