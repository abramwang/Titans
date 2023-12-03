#include "ti_book_depth.h"
#include <string.h>
#include "ti_quote_formater.h"

TiBookDepth::TiBookDepth(std::string symbol)
{
    memset(&m_base, 0, sizeof(TiQuoteDepthSnapshotBaseField));
    strcpy(m_base.symbol, symbol.c_str());
}

TiBookOrderPtr TiBookDepth::create_order(const TiQuoteOrderField* pData)
{
    TiBookOrder* order = new TiBookOrder();
    memset(order, 0, sizeof(TiBookOrder));
    memcpy(order, pData, sizeof(TiQuoteOrderField));
    order->dealt_vol = 0;

    return TiBookOrderPtr(order);
};

void TiBookDepth::add_order_sh(const TiQuoteOrderField* pData)
{
    if(!pData->price)
    {
        return;
    }
    TiBookOrderPtr order_ptr = create_order(pData);

    m_order_map[pData->order_orino] = order_ptr;

    TiBookDepthLevelPtr level_ptr = nullptr;
    
    //主动成交对手盘太多，不足以吃掉
    if(pData->function_code == 'B'){
        int32_t volume = 0;
        for (auto iter = m_asks.begin(); iter != m_asks.end(); iter++)
        {
            if (iter->second->get_price() <= pData->price)
            {
                volume += iter->second->get_volume();
            }
            else
            {
                break;
            }
        }
        if (volume > pData->volume)
        {
            return;
        }
    }
    if(pData->function_code == 'S'){
        int32_t volume = 0;
        for (auto iter = m_bids.begin(); iter != m_bids.end(); iter++)
        {
            if (iter->second->get_price() >= pData->price)
            {
                volume += iter->second->get_volume();
            }
            else
            {
                break;
            }
        }
        if (volume > pData->volume)
        {
            return;
        }
    }

    //买
    if(pData->function_code == 'B'){
        auto iter = m_bids.find(pData->price);
        if(iter != m_bids.end()){
            level_ptr = iter->second;
        }else{
            level_ptr = std::make_shared<TiBookDepthLevel>(pData->price);
            m_bids[pData->price] = level_ptr;
        }
    }
    //卖
    if(pData->function_code == 'S'){
        auto iter = m_asks.find(pData->price);
        if(iter != m_asks.end()){
            level_ptr = iter->second;
        }else{
            level_ptr = std::make_shared<TiBookDepthLevel>(pData->price);
            m_asks[pData->price] = level_ptr;
        }
    }

    level_ptr->add_order_sh(order_ptr);
};

void TiBookDepth::add_order_sz(const TiQuoteOrderField* pData)
{
    if (pData->order_type != '0')    //非限价单 不处理
    {
        return;
    }
    if(!pData->price)
    {
        return;
    }
    TiBookOrderPtr order_ptr = create_order(pData);
    m_order_map[pData->seq] = order_ptr;

    TiBookDepthLevelPtr level_ptr = nullptr;
    
    //主动成交对手盘太多，不足以吃掉
    if(pData->function_code == 'B'){
        int32_t volume = 0;
        for (auto iter = m_asks.begin(); iter != m_asks.end(); iter++)
        {
            if (iter->second->get_price() <= pData->price)
            {
                volume += iter->second->get_volume();
            }
            else
            {
                break;
            }
        }
        if (volume > pData->volume)
        {
            return;
        }
    }
    if(pData->function_code == 'S'){
        int32_t volume = 0;
        for (auto iter = m_bids.begin(); iter != m_bids.end(); iter++)
        {
            if (iter->second->get_price() >= pData->price)
            {
                volume += iter->second->get_volume();
            }
            else
            {
                break;
            }
        }
        if (volume > pData->volume)
        {
            return;
        }
    }

    //买
    if(pData->function_code == 'B'){
        auto iter = m_bids.find(pData->price);
        if(iter != m_bids.end()){
            level_ptr = iter->second;
        }else{
            level_ptr = std::make_shared<TiBookDepthLevel>(pData->price);
            m_bids[pData->price] = level_ptr;
        }
    }
    //卖
    if(pData->function_code == 'S'){
        auto iter = m_asks.find(pData->price);
        if(iter != m_asks.end()){
            level_ptr = iter->second;
        }else{
            level_ptr = std::make_shared<TiBookDepthLevel>(pData->price);
            m_asks[pData->price] = level_ptr;
        }
    }

    level_ptr->add_order_sz(order_ptr);
};

void TiBookDepth::del_order(const TiQuoteOrderField* pData)
{
    TiBookOrderPtr order_ptr = nullptr;
    auto order_iter = m_order_map.find(pData->order_orino);
    if(order_iter != m_order_map.end())
    {
        order_ptr = order_iter->second;
    }
    if(!order_ptr)
    {
        return;
    }

    order_ptr->volume = 0;
    auto level_iter = m_asks.find(order_ptr->price);
    if (level_iter != m_asks.end())
    {
        level_iter->second->del_order(pData->order_orino);
        if(!level_iter->second->get_volume())
        {
            m_asks.erase(level_iter);
        }
    }

    level_iter = m_bids.find(order_ptr->price);
    if (level_iter != m_bids.end())
    {
        level_iter->second->del_order(pData->order_orino);
        if(!level_iter->second->get_volume())
        {
            m_bids.erase(level_iter);
        }
    }

    if(order_iter != m_order_map.end())
    {
        m_order_map.erase(order_iter);
    }
};

void TiBookDepth::match_order(const TiQuoteMatchesField* pData)
{
    if(!pData->price)
    {
        return;
    }
    TiBookOrderPtr order_ptr = nullptr;
    //ask
    order_ptr = nullptr;
    auto order_iter = m_order_map.find(pData->ask_order_seq);
    if(order_iter != m_order_map.end())
    {
        order_ptr = order_iter->second;
    }
    if(order_ptr)
    {
        if (order_ptr->volume)
        {
            order_ptr->volume -= pData->volume;
            order_ptr->dealt_vol += pData->volume;
        }
        auto level_iter = m_asks.find(order_ptr->price);
        if (level_iter != m_asks.end())
        {
            if (order_ptr->volume <= 0)
            {
                level_iter->second->del_order(pData->ask_order_seq);
                if(!level_iter->second->get_volume())
                {
                    m_asks.erase(level_iter);
                }
            }else{
                level_iter->second->update();
            }
        }
    }
    // 清理价格之下的所有level
    if(pData->bs_flag == 'B')
    {
        auto level_iter = m_asks.begin();
        while(level_iter != m_asks.end())
        {
            if(level_iter->second->get_price() < pData->price)
            {
                m_asks.erase(level_iter++);
            }else{
                break;
            }
        }

        level_iter = m_bids.begin();
        while(level_iter != m_bids.end())
        {
            if(level_iter->second->get_price() > pData->price)
            {
                m_bids.erase(level_iter++);
            }else{
                break;
            }
        }
    }

    //bid
    order_ptr = nullptr;
    order_iter = m_order_map.find(pData->bid_order_seq);
    if(order_iter != m_order_map.end())
    {
        order_ptr = order_iter->second;
    }
    if(order_ptr){
        if (order_ptr->volume)
        {
            order_ptr->volume -= pData->volume;
            order_ptr->dealt_vol += pData->volume;
        }
        auto level_iter = m_bids.find(order_ptr->price);
        if (level_iter != m_bids.end())
        {
            if (order_ptr->volume <= 0)
            {
                level_iter->second->del_order(pData->bid_order_seq);
                if(!level_iter->second->get_volume())
                {
                    m_bids.erase(level_iter);
                }
            }else{
                level_iter->second->update();
            }
        }
    }
    // 清理价格之上的所有level
    if(pData->bs_flag == 'S')
    {
        auto level_iter = m_bids.begin();
        while(level_iter != m_bids.end())
        {
            if(level_iter->second->get_price() > pData->price)
            {
                m_bids.erase(level_iter++);
            }else{
                break;
            }
        }

        level_iter = m_asks.begin();
        while(level_iter != m_asks.end())
        {
            if(level_iter->second->get_price() < pData->price)
            {
                m_asks.erase(level_iter++);
            }else{
                break;
            }
        }
    }
};

void TiBookDepth::cancel_order(const TiQuoteMatchesField* pData)
{
    TiBookOrderPtr order_ptr = nullptr;
    if (pData->ask_order_seq)
    {
        auto order_iter = m_order_map.find(pData->ask_order_seq);
        if(order_iter != m_order_map.end())
        {
            order_ptr = order_iter->second;
        }

        if(order_ptr)
        {
            order_ptr->volume = 0;
            auto level_iter = m_asks.find(order_ptr->price);
            if (level_iter != m_asks.end())
            {
                level_iter->second->del_order(pData->ask_order_seq);
                if(!level_iter->second->get_volume())
                {
                    m_asks.erase(level_iter);
                }
            }
        }
        
        if(order_iter != m_order_map.end())
        {
            m_order_map.erase(order_iter);
        }
    }
    
    if (pData->bid_order_seq)
    {
        auto order_iter = m_order_map.find(pData->bid_order_seq);
        if(order_iter != m_order_map.end())
        {
            order_ptr = order_iter->second;
        }

        if(order_ptr)
        {
            order_ptr->volume = 0;
            auto level_iter = m_bids.find(order_ptr->price);
            if (level_iter != m_bids.end())
            {
                level_iter->second->del_order(pData->bid_order_seq);
                if(!level_iter->second->get_volume())
                {
                    m_bids.erase(level_iter);
                }
            }
        }
        
        if(order_iter != m_order_map.end())
        {
            m_order_map.erase(order_iter);
        }
    }
};

void TiBookDepth::RecvStockSnap(const TiQuoteSnapshotStockField* pData)
{
    m_base.pre_close = pData->pre_close;
    m_base.open = pData->open;
    m_base.high = pData->high;
    m_base.low = pData->low;
    m_base.high_limit = pData->high_limit;
    m_base.low_limit = pData->low_limit;
};

void TiBookDepth::RecvMatch(const TiQuoteMatchesField* pData)
{
    strcpy(m_base.exchange, pData->exchange);
    strcpy(m_base.symbol, pData->symbol);
    m_base.date = pData->date;
    m_base.time = pData->time;
    m_base.timestamp = pData->timestamp;
    if (pData->price)
    {
        m_base.last = pData->price;
        m_base.acc_volume += pData->volume;
        m_base.acc_turnover += (double)pData->volume * pData->price;
        m_base.match_items += 1;
    }
    // 上交所
    if(!strcmp(pData->exchange, "SH"))
    {
        match_order(pData);
        return;
    }
    // 深圳交所
    if(!strcmp(pData->exchange, "SZ"))
    {
        if (pData->function_code == 'C')
        {
            cancel_order(pData);
        }else{
            match_order(pData);
        }
        return;
    }
};

void TiBookDepth::RecvOrder(const TiQuoteOrderField* pData)
{   
    // 上交所
    if(!strcmp(pData->exchange, "SH"))
    {
        // 插入订单
        if(pData->order_type == 'A')
        {
            add_order_sh(pData);
        }
        if(pData->order_type == 'D')
        {
            del_order(pData);
        }
        return;
    }
    // 深交所
    if(!strcmp(pData->exchange, "SZ"))
    {
        add_order_sz(pData);
        return;
    }
};

void TiBookDepth::get_bids(std::vector<TiQuoteDepthLevel*>& bids, int32_t depth_level)
{
    bids.clear();
    int level = 0;
    for(auto iter = m_bids.begin(); iter != m_bids.end(); iter++)
    {
        ++level;
        bids.push_back(iter->second->get_info());
        if (depth_level > 0 && level >= depth_level)
        {
            break;
        }
    }
};

void TiBookDepth::get_asks(std::vector<TiQuoteDepthLevel*>& asks, int32_t depth_level)
{
    asks.clear();
    int level = 0;
    for(auto iter = m_asks.begin(); iter != m_asks.end(); iter++)
    {
        ++level;
        asks.push_back(iter->second->get_info());
        if (depth_level > 0 && level >= depth_level)
        {
            break;
        }
    }
};

TiBookDepthLevelPtr TiBookDepth::get_ask_depth(int32_t level)
{
    int i = 0;
    for(auto iter = m_asks.begin(); iter != m_asks.end(); iter++)
    {
        if (i == level)
        {
            return iter->second;
        }
        i++;
    }
    return NULL;
};

TiBookDepthLevelPtr TiBookDepth::get_bid_depth(int32_t level)
{
    int i = 0;
    for(auto iter = m_bids.begin(); iter != m_bids.end(); iter++)
    {
        if (i == level)
        {
            return iter->second;
        }
        i++;
    }
    return NULL;
};