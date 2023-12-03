#ifndef __TI_BOOK_DEPTH_LEVEL_H__
#define __TI_BOOK_DEPTH_LEVEL_H__


#include <string>
#include <memory>
#include <map>

#include "ti_quote_struct.h"
#include "ti_book_order.h"

class TiBookDepthLevel
{
private:
    TiQuoteDepthLevel m_info;
    std::map<int32_t, TiBookOrderPtr> m_order_map;       //订单map seq, order
    /* data */
public:
    void add_order_sh(TiBookOrderPtr order_ptr);
    void add_order_sz(TiBookOrderPtr order_ptr);
    void del_order(int32_t seq);
    void update();
    
public:
    inline double get_price(){return m_info.price;};
    inline int32_t get_volume(){return m_info.volume;};
    inline int32_t get_order_count(){return m_info.order_count;};
    inline TiQuoteDepthLevel* get_info(){return &m_info;};
    inline std::map<int32_t, TiBookOrderPtr>* get_order_map(){return &m_order_map;};
public:
    TiBookDepthLevel(double price);
    virtual ~TiBookDepthLevel(){};
};


#endif