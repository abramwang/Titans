#include "ti_book_depth_level.h"

TiBookDepthLevel::TiBookDepthLevel(double price)
{
    m_info.price = price;
    m_info.volume = 0;
    m_info.order_count = 0;
}

void TiBookDepthLevel::add_order_sh(TiBookOrderPtr order_ptr)
{
    m_order_map[order_ptr->order_orino] = order_ptr;
    m_info.volume += order_ptr->volume;
    m_info.order_count = m_order_map.size();
};

void TiBookDepthLevel::add_order_sz(TiBookOrderPtr order_ptr)
{
    m_order_map[order_ptr->seq] = order_ptr;
    m_info.volume += order_ptr->volume;
    m_info.order_count = m_order_map.size();
};

void TiBookDepthLevel::del_order(int32_t seq)
{
    auto iter = m_order_map.find(seq);
    if(iter != m_order_map.end())
    {
        m_order_map.erase(iter);
    }
    update();
};

void TiBookDepthLevel::update()
{
    m_info.volume = 0;
    for(auto iter = m_order_map.begin(); iter != m_order_map.end(); iter++)
    {
        m_info.volume += iter->second->volume;
    }
    m_info.order_count = m_order_map.size();
};