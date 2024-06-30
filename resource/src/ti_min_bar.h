#ifndef __TI_MIN_BAR_H__
#define __TI_MIN_BAR_H__

#include "ti_common_struct.h"
#include <memory>
#include <vector>
#include <map>


const char TI_BarCycType_1m         = 1;    // 1 分钟
const char TI_BarCycType_5m         = 5;    // 5 分钟
const char TI_BarCycType_10m        = 10;   // 卖出
const char TI_BarCycType_15m        = 15;   // 买入
const char TI_BarCycType_30m        = 30;   // 申购
const char TI_BarCycType_1h         = 60;   // 赎回
typedef char TI_BarCycType;


struct TiBarData
{
    int64_t timestamp;
    double open;
    double high;
    double low;
    double close;
    int32_t num;
};
typedef std::shared_ptr<TiBarData> TiBarDataPtr;

class TiMinBar {
private:
    std::map<int64_t,  TiBarDataPtr>  m_bars;  //  存储OHLC  bar数据的map

public:
    TiMinBar();
    ~TiMinBar();


    //  处理成交数据
    void  processTrade(const int64_t timestamp,  const double value)  {
        //  获取当前分钟的时间戳
        int64_t time_mod  =  timestamp % (60 * 1000);
        int64_t currentMinute_time = timestamp - time_mod;

        //  查找当前分钟是否有bar数据
        auto  it  =  m_bars.find(currentMinute_time);
        if  (it  !=  m_bars.end())  {
            //  更新OHLC  bar数据
            it->second->high  = std::max(it->second->high,  value);
            it->second->low  =  std::min(it->second->low,  value);
            it->second->close  = value;
            it->second->num++;
        }  else  {
            //  创建新的OHLC  bar数据
            TiBarDataPtr newBar = std::make_shared<TiBarData>();
            newBar->open    = value;
            newBar->high    = value;
            newBar->low     = value;
            newBar->close   = value;
            newBar->num     = 1;
            newBar->timestamp  =  currentMinute_time;

            m_bars[currentMinute_time]  =  newBar;
        }
    }

    //  获取所有生成的OHLC  bar数据
    std::vector<TiBarDataPtr>  getBars()  {
        std::vector<TiBarDataPtr>  result;
        for  (const  auto&  bar  :  m_bars)  {
            result.push_back(bar.second);
        }
        return  result;
    }

};

#endif //__TI_MIN_BAR_H__