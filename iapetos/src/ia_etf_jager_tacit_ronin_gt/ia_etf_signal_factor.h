#ifndef __IA_ETF_SIGNAL_FACTOR_H__
#define __IA_ETF_SIGNAL_FACTOR_H__

#include <memory>
#include <vector>

#include "ti_quote_callback.h"
#include "ia_etf_info_struct.h"
#include "ia_etf_quote_data_cache.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaEtfSignalFactor 
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
    std::shared_ptr<IaEtfInfo>                          m_etf_info_ptr;
    std::vector<std::shared_ptr<IaEtfConstituentInfo>>  m_constituent_info_vec;
    IaEtfQuoteDataCache* m_quote_data_cache;

    json m_out;     //输出数据

private:
    double calc_diff(); //计算现金差值
public:
    IaEtfSignalFactor(std::shared_ptr<IaEtfInfo> etf_info_ptr, 
        std::vector<std::shared_ptr<IaEtfConstituentInfo>> constituent_info_vec, 
        IaEtfQuoteDataCache* etf_quote_data_cache);
    ~IaEtfSignalFactor();

public:
    void GetJsonOut(json& j);

};

#endif