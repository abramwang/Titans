#include "ti_min_bar.h"
#include "datetime.h"

TiMinBar::TiMinBar()
{
    m_cyc = TI_BarCycType_1m;
};

TiMinBar::TiMinBar(TI_BarCycType cyc)
{
    m_cyc = cyc;
};


int64_t TiMinBar::getBarTimestamp(const int64_t timestamp)
{
    int64_t time_mod  =  timestamp % (int(m_cyc) * 60 * 1000);
    int64_t currentMinute_time = timestamp - time_mod;
    return currentMinute_time + int(m_cyc) * 60 * 1000;
};

void TiMinBar::updateData(const int64_t timestamp,  const double value)
{
    //  获取当前分钟的时间戳
    int64_t currentMinute_time = getBarTimestamp(timestamp);

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

std::vector<TiBarDataPtr> TiMinBar::getBars() {
    std::vector<TiBarDataPtr>  result;
    for  (const  auto&  bar  :  m_bars)  {
        result.push_back(bar.second);
    }
    return result;
};

std::vector<TiBarDataPtr> TiMinBar::getBars(const int64_t last_timestamp){
    std::vector<TiBarDataPtr>  result;
    for  (const  auto&  bar  :  m_bars)  {
        for  (const  auto&  bar  :  m_bars)  {
            result.push_back(bar.second);
        }
    }
    int64_t last_minute_time = getBarTimestamp(last_timestamp);
    auto iter = m_bars.find(last_minute_time);
    if (iter == m_bars.end())   //临时补最后一个bar
    {
        TiBarDataPtr lastBar = nullptr;
        if (getLastBar(lastBar))
        {
            //  创建新的OHLC  bar数据
            TiBarDataPtr newBar = std::make_shared<TiBarData>();
            newBar->cyc     = m_cyc;
            newBar->open    = lastBar->close;
            newBar->high    = lastBar->close;
            newBar->low     = lastBar->close;
            newBar->close   = lastBar->close;
            newBar->num     = 0;
            newBar->timestamp  =  last_minute_time;
            result.push_back(newBar);
        }
    }

    return result;
};

void TiMinBar::getBarsJson(json& j) {
    j = json::array();
    for  (const  auto&  bar  :  m_bars)  {
        json  barJson;
        barJson["timestamp"]  =  datetime::get_format_timestamp_ms(bar.second->timestamp);
        barJson["cyc"]        =  bar.second->cyc;
        barJson["open"]       =  bar.second->open;
        barJson["high"]       =  bar.second->high;
        barJson["low"]        =  bar.second->low;
        barJson["close"]      =  bar.second->close;
        barJson["num"]        =  bar.second->num;

        j.push_back(barJson);
    }
};

void TiMinBar::getBarsJson(json& j, const int64_t last_timestamp)
{
    j = json::array();
    for  (const  auto&  bar  :  m_bars)  {
        json  barJson;
        barJson["timestamp"]  =  datetime::get_format_timestamp_ms(bar.second->timestamp);
        barJson["cyc"]        =  bar.second->cyc;
        barJson["open"]       =  bar.second->open;
        barJson["high"]       =  bar.second->high;
        barJson["low"]        =  bar.second->low;
        barJson["close"]      =  bar.second->close;
        barJson["num"]        =  bar.second->num;

        j.push_back(barJson);
    }


    int64_t last_minute_time = getBarTimestamp(last_timestamp);
    auto iter = m_bars.find(last_minute_time);
    if (iter == m_bars.end())   //临时补最后一个bar
    {
        TiBarDataPtr lastBar = nullptr;
        if (getLastBar(lastBar))
        {
            json  barJson;
            barJson["timestamp"]  =  datetime::get_format_timestamp_ms(last_minute_time);
            barJson["cyc"]        =  m_cyc;
            barJson["open"]       =  lastBar->close;
            barJson["high"]       =  lastBar->close;
            barJson["low"]        =  lastBar->close;
            barJson["close"]      =  lastBar->close;
            barJson["num"]        =  0;

            j.push_back(barJson);
        }
    }
};


bool TiMinBar::getLastBar(TiBarDataPtr& bar)
{
    if  (m_bars.empty())  {
        return false;
    }

    bar = m_bars.rbegin()->second;
    return true;
};

bool TiMinBar::getLastBar(TiBarDataPtr& bar, const int64_t last_timestamp)
{
    if  (m_bars.empty())  {
        return false;
    }

    int64_t last_minute_time = getBarTimestamp(last_timestamp);

    TiBarDataPtr lastBar = nullptr;
    if (getLastBar(lastBar))
    {
        if (last_minute_time == lastBar->timestamp)
        {
            bar = lastBar;
        }else{
            TiBarDataPtr bar = std::make_shared<TiBarData>();
            bar->cyc     = m_cyc;
            bar->open    = lastBar->close;
            bar->high    = lastBar->close;
            bar->low     = lastBar->close;
            bar->close   = lastBar->close;
            bar->num     = 0;
            bar->timestamp  =  last_minute_time;
        }
    }
    return true;

};

bool TiMinBar::getCloseSeries(std::vector<double>& closeSeries)
{
    if  (m_bars.empty())  {
        return false;
    }

    closeSeries.clear();

    for  (const  auto&  bar  :  m_bars)  {
        closeSeries.push_back(bar.second->close);
    }

    return true;
};

bool TiMinBar::getCloseSeries(std::vector<double>& closeSeries, const int64_t last_timestamp)
{
    if  (m_bars.empty())  {
        return false;
    }

    closeSeries.clear();


    std::vector<TiBarDataPtr> bars = getBars(last_timestamp);

    for  (const  auto&  bar  :  bars)  {
        closeSeries.push_back(bar->close);
    }

    return true;
};