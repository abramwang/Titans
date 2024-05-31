#ifndef IA_ETF_PRICE_TOOL_H
#define IA_ETF_PRICE_TOOL_H

#include <iostream>
#include "ti_quote_struct.h"

namespace IaEtfPriceTool
{
    //获取可以买到的成交价
    inline double get_order_dealt_price(int dealt_vol, const double price_level[], const int32_t vol_level[], int level)
    {
        double turnover = 0.0;
        int32_t vol = 0;
        for (int i = 0; i < level; i++)
        {
            if (vol_level[i] < (dealt_vol - vol))
            {
                turnover += vol_level[i] * price_level[i];
                vol += vol_level[i];
            }
            else
            {
                turnover += (dealt_vol - vol) * price_level[i];
                vol += (dealt_vol - vol);
                break;
            }
        }
        return turnover / vol;
    };

    //获取可以买到的委托价
    inline double get_order_price(int dealt_vol, const double price_level[], const int32_t vol_level[], int level)
    {
        int order_level = 0;
        int32_t vol = 0;
        for (int i = 0; i < level; i++)
        {
            order_level = i;
            if (vol_level[i] < (dealt_vol - vol))
            {
                vol += vol_level[i];
            }
            else
            {
                vol += (dealt_vol - vol);
                break;
            }
        }
        if ((order_level + 2) < (level - 1))
        {
            return price_level[order_level + 2];
        }else{
            return price_level[level - 1];
        }
    };

    //获取可以买到的委托价
    inline double get_order_price(int dealt_vol, const double price_level[], const int32_t vol_level[], int level, double limit_price)
    {
        int order_level = 0;
        int32_t vol = 0;
        double_t price = 0;
        for (int i = 0; i < level; i++)
        {
            order_level = i;
            if (vol_level[i] < (dealt_vol - vol))
            {
                vol += vol_level[i];
            }
            else
            {
                vol += (dealt_vol - vol);
                break;
            }
        }
        if ((order_level + 2) < (level - 1))
        {
            price = price_level[order_level + 2];
        }else{
            price = price_level[level - 1];
        }
        return !price? limit_price : price;
    };

};

#endif
