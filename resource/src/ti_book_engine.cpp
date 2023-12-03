#include "ti_book_engine.h"
#include "ti_quote_tools.h"
#include "datetime.h"

TiBookEngine::TiBookEngine(TiQuoteDepthCallback *cb, int32_t update_freq, int32_t depth_level)
{
    m_cb = cb;
    m_update_freq = update_freq;
    m_depth_level = depth_level;
}

void TiBookEngine::ouput_orderbook(TiBookDepthPtr depth_ptr)
{
    TiQuoteOrderBookField orderbook;
    memset(&orderbook, 0, sizeof(orderbook));
    const TiQuoteDepthSnapshotBaseField* base = depth_ptr->get_base();
    memcpy(&orderbook, base, sizeof(TiQuoteDepthSnapshotBaseField));

    std::map<int32_t, TiBookOrderPtr>* bid_order_map = NULL, *ask_order_map = NULL;
    orderbook.level = 0;

    TiBookDepthLevelPtr bid_depth = depth_ptr->get_bid_depth(0);
    if (bid_depth)
    {
        orderbook.bid_price = bid_depth->get_price();
        orderbook.bid_volume = bid_depth->get_volume();
        orderbook.bid_order_count = bid_depth->get_order_count();
        bid_order_map = bid_depth->get_order_map();
        int32_t i = 0;
        for (auto iter = bid_order_map->begin(); iter != bid_order_map->end(); iter++)
        {
            if (i >= TI_ORDERBOOK_ARRAY_LEN)
            {
                break;
            }
            orderbook.bid_orders[i] = {
                iter->second->seq,
                iter->second->order_orino,
                iter->second->volume,
                iter->second->dealt_vol
            };
            i++;
        }
    }
    
    TiBookDepthLevelPtr ask_depth = depth_ptr->get_ask_depth(0);
    if (ask_depth)
    {
        orderbook.ask_price = ask_depth->get_price();
        orderbook.ask_volume = ask_depth->get_volume();
        orderbook.ask_order_count = ask_depth->get_order_count();
        ask_order_map = ask_depth->get_order_map();
        int32_t i = 0;
        for (auto iter = ask_order_map->begin(); iter != ask_order_map->end(); iter++)
        {
            if (i >= TI_ORDERBOOK_ARRAY_LEN)
            {
                break;
            }
            orderbook.ask_orders[i] = {
                iter->second->seq,
                iter->second->order_orino,
                iter->second->volume,
                iter->second->dealt_vol
            };
            i++;
        }
    }

    m_cb->OnDepthOrderBookLevelRtn(&orderbook);
};

void TiBookEngine::ouput_depth(TiBookDepthPtr depth_ptr, const TiQuoteMatchesField* pData){
    int64_t id = TiQuoteTools::GetSymbolID(pData->exchange, pData->symbol);
    int64_t update_time = m_update_time_map[id];
    if ((pData->time - update_time) < m_update_freq && m_update_freq)
    {
        return;
    }
    m_update_time_map[id] = pData->time; 

    if(update_time < 93000010){
        return;
    }
    if(update_time > 145700000){
        return;
    }

    depth_ptr->get_bids(m_bids_cache, m_depth_level);
    depth_ptr->get_asks(m_asks_cache, m_depth_level);
    
    if (m_cb)
    {
        m_cb->OnDepthSnapshotRtn(depth_ptr->get_base(), m_asks_cache, m_bids_cache);
        ouput_orderbook(depth_ptr);
    }
};

TiBookDepthPtr TiBookEngine::get_depth(const TI_ExchangeType exchange, const TI_SymbolType symbol)
{
    int64_t id = TiQuoteTools::GetSymbolID(exchange, symbol);
    TiBookDepthPtr _depth;

    auto iter = m_depth_map.find(id); 
    if(iter != m_depth_map.end())
    {
        _depth = iter->second;
    }else{
        _depth = std::make_shared<TiBookDepth>(symbol);
        m_depth_map[id] = _depth;
    }
    return _depth;
};

void TiBookEngine::RecvStockSnap(const TiQuoteSnapshotStockField* pData)
{
    TiBookDepthPtr _depth = get_depth(pData->exchange, pData->symbol);
    _depth->RecvStockSnap(pData);
};
    
void TiBookEngine::RecvOrder(const TiQuoteOrderField* pData)
{
    TiBookDepthPtr _depth = get_depth(pData->exchange, pData->symbol);
    _depth->RecvOrder(pData);
};

void TiBookEngine::RecvMatch(const TiQuoteMatchesField* pData)
{
    TiBookDepthPtr _depth = get_depth(pData->exchange, pData->symbol);
    _depth->RecvMatch(pData);
    
    // 上交所
    if(!strcmp(pData->exchange, "SH"))
    {
        ouput_depth(_depth, pData);
        return;
    }
    // 深交所
    if(!strcmp(pData->exchange, "SZ"))
    {
        if (pData->function_code != 'C')
        {
            ouput_depth(_depth, pData);
        }
    }
    return;
};
