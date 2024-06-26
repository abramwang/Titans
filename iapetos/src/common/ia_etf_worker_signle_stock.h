#ifndef __IA_ETF_WORKER_SINGLE_STOCK_H__
#define __IA_ETF_WORKER_SINGLE_STOCK_H__

#include "ia_etf_trading_worker.h"
#include "ia_etf_info_struct.h"
#include <unordered_map>
#include <memory.h>

/*
{
    symbol: "000001",               //股票代码
    exchange: "SH",                 //股票市场
    expect_cost: 1000000,           //期望交易成本 包含成交额和手续费
    volume: 1000000,                //交易数量
    real_cost: 1000000,             //实际交易成本 包含成交额和手续费
    finish_volume: 1000000,         //实际成交数量
    orders: [
        {
            req_id,
            order_id,
        }
    ],                    //股票交易信息
}
*/

class IaETFWorkerSingleStock:
    public IaETFTradingWorker
{
private:
    TI_TradeSideType m_side; 
public:
    int64_t open();
    json getStatusJson();
public:
    IaETFWorkerSingleStock(int64_t id, TiTraderClient* client, IaEtfQuoteDataCache* quote_cache, IaEtfSignalFactorPtr factor, std::string account, 
        std::shared_ptr<IaEtfConstituentInfo> constituentInfoPtr, TI_TradeSideType side);
    virtual ~IaETFWorkerSingleStock(){};
};

typedef std::shared_ptr<IaETFWorkerSingleStock> IaETFWorkerSingleStockPtr;

#endif