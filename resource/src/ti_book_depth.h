#ifndef __TI_BOOK_DEPTH_H__
#define __TI_BOOK_DEPTH_H__

#include <string>
#include <memory>
#include <unordered_map>
#include <map>
#include <vector>
#include "ti_book_depth_level.h"
#include "ti_book_order.h"

typedef std::shared_ptr<TiBookDepthLevel> TiBookDepthLevelPtr;

class TiBookDepth
{
private:
    TiQuoteDepthSnapshotBaseField m_base;
    
    std::unordered_map<int32_t, TiBookOrderPtr> m_order_map;                 //订单map seq, order

    std::map<double, TiBookDepthLevelPtr, std::greater<double>> m_bids;            //叫买 降排序
    std::map<double, TiBookDepthLevelPtr, std::less<double>> m_asks;               //叫卖 升排序

private:
    TiBookOrderPtr create_order(const TiQuoteOrderField* pData);

    void add_order_sh(const TiQuoteOrderField* pData);
    void add_order_sz(const TiQuoteOrderField* pData);
    void del_order(const TiQuoteOrderField* pData);
    void match_order(const TiQuoteMatchesField* pData);
    void cancel_order(const TiQuoteMatchesField* pData);    //深交所才用
    
public:
    void RecvStockSnap(const TiQuoteSnapshotStockField* pData);
    void RecvMatch(const TiQuoteMatchesField* pData);
    void RecvOrder(const TiQuoteOrderField* pData);

public:
    inline const TiQuoteDepthSnapshotBaseField* get_base(){return &m_base;};
    void get_bids(std::vector<TiQuoteDepthLevel*>& bids, int32_t depth_level);
    void get_asks(std::vector<TiQuoteDepthLevel*>& asks, int32_t depth_level);
    TiBookDepthLevelPtr get_ask_depth(int32_t level);
    TiBookDepthLevelPtr get_bid_depth(int32_t level);
public:
    TiBookDepth(std::string symbol);
    virtual ~TiBookDepth(){};
};

#endif