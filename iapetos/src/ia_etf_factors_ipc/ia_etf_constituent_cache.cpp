#include "ia_etf_constituent_cache.h"
#include <iostream>

IaETFConstituentCache::IaETFConstituentCache(const char* symbol, const char* exchange, IaETFFundInfo::ConstituentStockInfo info, double commission)
{
    strcpy(szSymbol, symbol);
    strcpy(szExchange, exchange);
    m_info = info;
    m_commission = commission;
    memset(&m_last_snap, 0, sizeof(TiQuoteSnapshotStockField));
    
    nTimeStamp = 0;
    nDiffFactor = 0;

    nBidIopvFactor = 0; //买
    nAskIopvFactor = 0; //卖
    
    nSellFee = 0;   //卖手续费
    nBuyFee = 0;    //买手续费
    
    nBuyCost = 0;
    nSellCost = 0;
    nHighLimitValue = 0; // 涨停市值
    nLowLimitValue = 0;  // 跌停市值

    bReplace = false;

    if (m_info.m_replace_flag == 2 || m_info.m_replace_flag == 4 || m_info.m_replace_flag == 6)       // 2-6 为现金替代
    {
        bReplace = true;
        nBidIopvFactor = m_info.m_redemption_amount ? m_info.m_redemption_amount : m_info.m_replace_amount;
        nAskIopvFactor = m_info.m_creation_amount ? m_info.m_creation_amount : m_info.m_replace_amount;
    }

}

IaETFConstituentCache::~IaETFConstituentCache()
{
}


void IaETFConstituentCache::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{   
    memcpy(&m_last_snap, pData, sizeof(TiQuoteSnapshotStockField));
    
    double last = 0;
    if(!pData->last && pData->ask_price[0] && !pData->bid_price[0]){
        last = pData->ask_price[0];
    }else if(!pData->last && !pData->ask_price[0] && pData->bid_price[0]){
        last = pData->bid_price[0];
    }else if(!pData->last && pData->ask_price[0] && pData->bid_price[0]){
        last = (pData->ask_price[0] - pData->bid_price[0]) / 2;
    }else{
        last = pData->last ? pData->last : pData->pre_close;
    }
    
    nBidIopvFactor = pData->bid_price[0] ? (pData->bid_price[0] * m_info.m_disclosure_vol) : (last * m_info.m_disclosure_vol);
    nAskIopvFactor = pData->ask_price[0] ? (pData->ask_price[0] * m_info.m_disclosure_vol) : (last * m_info.m_disclosure_vol);

    nSellFee = nBidIopvFactor * ( m_commission + 0.0005);    //卖手续费 佣金和 千1的印花税
    nBuyFee = nAskIopvFactor * m_commission;                //买手续费

    nBidIopvFactor = nBidIopvFactor - nSellFee;
    nAskIopvFactor = nAskIopvFactor + nBuyFee; 

    if (bReplace)
    {
        nBidIopvFactor = m_info.m_redemption_amount ? m_info.m_redemption_amount : m_info.m_replace_amount;
        nAskIopvFactor = m_info.m_creation_amount ? m_info.m_creation_amount : m_info.m_replace_amount;

        nSellFee = 0;   //卖手续费
        nBuyFee = 0;    //买手续费
    }  

    nTimeStamp  = pData->timestamp;
    //nDiffFactor = m_realityVol ? (m_realityVol - m_disclosureVol) * (pData->last - pData->open) : 0;
    nDiffFactor = m_info.m_reality_vol ? (m_info.m_reality_vol - m_info.m_disclosure_vol) * (last - pData->pre_close) : 0;
    if(!pData->last){
        nDiffFactor = 0;
    }
#if 0
    m_counting_process_json = {
        {"symbol", m_last_snap.symbol},
        {"time", m_last_snap.time},
        {"m_reality_vol" , m_info.m_reality_vol},
        {"m_disclosure_vol" , m_info.m_disclosure_vol},
        {"pre_close" , pData->pre_close},
        {"last" , pData->last},
        {"diff" , nDiffFactor.load()},
    };
    if (!strcmp(m_last_snap.symbol, "688696"))
    {
        std::cout << m_counting_process_json.dump() << std::endl;
    }
    
#endif

    calcTradingCostByIopv(pData);
    //calcTradingCost(pData);
};

void IaETFConstituentCache::calcTradingCostByIopv(const TiQuoteSnapshotStockField* pData)
{
    // 交易成本算上手续费
    nBuyCost = nAskIopvFactor.load();                   // 佣金
    nSellCost = nBidIopvFactor.load();    // 佣金和 千1的印花税
    
    if (pData->time > 150000000)
    {
        return;
    }
    
    nHighLimitValue = 0;
    nLowLimitValue = 0;

    if (pData->high_limit)
    {
        if (pData->last == pData->high_limit)
        {
            nHighLimitValue = pData->high_limit * m_info.m_disclosure_vol;
        }
    }else{
        if (!pData->ask_price[0])
        {
            nHighLimitValue = pData->last * m_info.m_disclosure_vol;
        }
    }

    if (pData->low_limit)
    {
        if (pData->last == pData->low_limit)
        {
            nLowLimitValue = pData->low_limit * m_info.m_disclosure_vol;
        }
    }else{
        if (!pData->bid_price[0])
        {
            nLowLimitValue = pData->last * m_info.m_disclosure_vol;
        }
    }
};

void IaETFConstituentCache::calcTradingCost(const TiQuoteSnapshotStockField* pData)
{
    int buy_vol = 0;
    int sell_vol = 0;
    double buy_turnover = 0;
    double sell_turnover = 0;

    for (size_t i = 0; i < TI_STOCK_ARRAY_LEN; i++)
    {
        // 买
        int _buy_vol_single = pData->ask_volume[i] > (m_info.m_disclosure_vol - buy_vol) ? (m_info.m_disclosure_vol - buy_vol) : pData->ask_volume[i];
        if (_buy_vol_single > 0){
            buy_vol += _buy_vol_single;
            buy_turnover += _buy_vol_single*pData->ask_price[i];
        }

        // 卖
        int _sell_vol_single = pData->bid_volume[i] > (m_info.m_disclosure_vol - sell_vol) ? (m_info.m_disclosure_vol - sell_vol) : pData->bid_volume[i];
        if (_sell_vol_single > 0){
            sell_vol += _sell_vol_single;
            sell_turnover += _sell_vol_single*pData->bid_price[i];
        }
    }

    // 涨跌停没有买到的情况
    if (buy_vol !=  m_info.m_disclosure_vol){
        int _buy_vol_single = m_info.m_disclosure_vol - buy_vol;
        buy_vol += _buy_vol_single;
        buy_turnover += _buy_vol_single*pData->high_limit;
    }
    if (sell_vol !=  m_info.m_disclosure_vol){
        int _sell_vol_single = m_info.m_disclosure_vol - sell_vol;
        sell_vol += _sell_vol_single;
        sell_turnover += _sell_vol_single*pData->low_limit;
    }
    
     // 交易成本算上手续费
    nBuyCost = buy_turnover + buy_turnover * m_commission;                   // 佣金
    nSellCost = sell_turnover - sell_turnover * ( m_commission + 0.0005);    // 佣金和 千1的印花税
    
    nHighLimitValue = 0;
    nLowLimitValue = 0;

    if (pData->high_limit)
    {
        if (pData->last == pData->high_limit)
        {
            nHighLimitValue = pData->high_limit * m_info.m_disclosure_vol;
        }
    }else{
        if (!pData->ask_price[0])
        {
            nHighLimitValue = pData->last * m_info.m_disclosure_vol;
        }
    }

    if (pData->low_limit)
    {
        if (pData->last == pData->low_limit)
        {
            nLowLimitValue = pData->low_limit * m_info.m_disclosure_vol;
        }
    }else{
        if (!pData->bid_price[0])
        {
            nLowLimitValue = pData->last * m_info.m_disclosure_vol;
        }
    }

    //nBuyCost = buy_turnover;
    //nSellCost = sell_turnover; 
};