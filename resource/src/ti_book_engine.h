#ifndef __TI_BOOK_ENGINE_H__
#define __TI_BOOK_ENGINE_H__

#include <memory>
#include <unordered_map>
#include "ti_book_depth.h"
#include "ti_book_order.h"
#include "ti_quote_depth_callback.h"

typedef std::shared_ptr<TiBookDepth> TiBookDepthPtr;

class TiBookEngine
{
private:
    std::unordered_map<int64_t, TiBookDepthPtr> m_depth_map;
    std::unordered_map<int64_t, int64_t> m_update_time_map;
    TiQuoteDepthCallback *m_cb;
    int32_t m_update_freq;      //设置更新频率，单位毫秒
    int32_t m_depth_level;      //设置输出深度挡位

    std::vector<TiQuoteDepthLevel*> m_bids_cache;
    std::vector<TiQuoteDepthLevel*> m_asks_cache;

    void ouput_orderbook(TiBookDepthPtr depth_ptr);
    void ouput_depth(TiBookDepthPtr depth_ptr, const TiQuoteMatchesField* pData);
    TiBookDepthPtr get_depth(const TI_ExchangeType exchange, const TI_SymbolType symbol);
public:
    void RecvStockSnap(const TiQuoteSnapshotStockField* pData);
    void RecvOrder(const TiQuoteOrderField* pData);
    void RecvMatch(const TiQuoteMatchesField* pData);
public:
    TiBookEngine(TiQuoteDepthCallback *cb, int32_t update_freq, int32_t depth_level);
    virtual ~TiBookEngine(){};
};


#endif