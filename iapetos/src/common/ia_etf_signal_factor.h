#ifndef __IA_ETF_SIGNAL_FACTOR_H__
#define __IA_ETF_SIGNAL_FACTOR_H__

#include <memory>
#include <vector>
#include <armadillo>

#include "ti_fee.h"
#include "ti_quote_callback.h"
#include "ia_etf_info_struct.h"
#include "ia_etf_quote_data_cache.h"


#include <nlohmann/json.hpp>
using namespace nlohmann;

class IaEtfTradeWorkerCenter;
class IaEtfSignalFactor 
    : public TiQuoteCallback
{
public:

struct profit_info
{
    /// @brief 申购瞬时利润
    double buy_stock_amount;            //买入股票市值
    double buy_stock_amount_local;      //买入本市场股票市值
    double buy_stock_amount_cross;      //买入跨市场股票市值
    double buy_stock_fee;               //买入股票手续费
    double buy_stock_fee_local;         //买入本市场手续费
    double buy_stock_fee_cross;         //买入跨市场手续费

    double buy_stock_replace_amount;    //买入替代股票市值
    double buy_stock_replace_margin;    //买入替代股票占款

    double sell_etf_amount;             //卖出ETF市值
    double sell_etf_fee;                //卖出ETF手续费

    double creation_turnover;           //申购成交额    
    
    double creation_profit;             //申购瞬时利润   

    /// @brief 赎回瞬时利润

    double sell_stock_amount;           //卖出股票市值
    double sell_stock_amount_local;     //卖出本市场股票市值
    double sell_stock_amount_cross;     //卖出跨市场股票市值
    double sell_stock_fee;              //卖出股票手续费
    double sell_stock_fee_local;        //卖出本市场手续费
    double sell_stock_fee_cross;        //卖出跨市场手续费

    double sell_stock_replace_amount;   //卖出替代股票市值
    double sell_stock_replace_margin;   //卖出替代股票占款

    double buy_etf_amount;              //买入ETF市值
    double buy_etf_fee;                 //买入ETF手续费

    double redemption_turnover;         //赎回成交额

    double redemption_profit;           //赎回瞬时利润

    /// @brief diff
    double diff;                        //现金差值

    /// @brief IOPV
    double iopv;                        //IOPV 自己计算的iopv
    double creation_iopv;
    double redemption_iopv;

    /// @brief 相关性
    double corr;                        //与大盘的相关性
};

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

    profit_info m_info;
    json m_out;         //输出数据

private:
    double get_last_price(const TiQuoteSnapshotStockField* pData);
    double get_bid_price(const TiQuoteSnapshotStockField* pData);
    double get_ask_price(const TiQuoteSnapshotStockField* pData);

    //获取期货替代指数行情
    TiQuoteSnapshotIndexField* get_future_replace_price(std::string future_symbol);

    double calc_corr(const TiQuoteSnapshotStockField* pData); //计算与大盘的相关性

    double calc_diff(); //计算现金差值
    void calc_iopv(const TiQuoteSnapshotStockField* pEtfSnap, profit_info &info); //计算IOPV
    //格式化输出
    void format_json_profit(profit_info &info);
    //格式化influx数据
    void format_influx_factor(const TiQuoteSnapshotStockField* pEtfSnap, profit_info &info);
public:
    IaEtfSignalFactor(std::shared_ptr<IaEtfInfo> etf_info_ptr, 
        std::vector<std::shared_ptr<IaEtfConstituentInfo>> constituent_info_vec, 
        IaEtfQuoteDataCache* etf_quote_data_cache);
    ~IaEtfSignalFactor();

public:
    bool GetJsonOut(json& j);
    std::shared_ptr<IaEtfInfo> GetEtfInfo();
    std::vector<std::shared_ptr<IaEtfConstituentInfo>>* GetConstituentInfo();


public:
    friend class IaEtfTradeWorkerCenter;
};

typedef std::shared_ptr<IaEtfSignalFactor> IaEtfSignalFactorPtr;

#endif