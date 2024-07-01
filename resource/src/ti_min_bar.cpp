#include "ti_min_bar.h"

TiMinBar::TiMinBar()
{
    m_cyc = TI_BarCycType_1m;
};

TiMinBar::TiMinBar(TI_BarCycType cyc)
{
    m_cyc = cyc;
};

void TiMinBar::processTrade(const int64_t timestamp,  const double value)  {
    //  获取当前分钟的时间戳
    int64_t time_mod  =  timestamp % (int(m_cyc) * 60 * 1000);
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
        newBar->cyc     = m_cyc;
        newBar->open    = value;
        newBar->high    = value;
        newBar->low     = value;
        newBar->close   = value;
        newBar->num     = 1;
        newBar->timestamp  =  currentMinute_time;

        m_bars[currentMinute_time]  =  newBar;
    }
};

std::vector<TiBarDataPtr> TiMinBar::getBars()  {
    std::vector<TiBarDataPtr>  result;
    for  (const  auto&  bar  :  m_bars)  {
        result.push_back(bar.second);
    }
    return result;
}