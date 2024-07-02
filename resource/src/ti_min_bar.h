#ifndef __TI_MIN_BAR_H__
#define __TI_MIN_BAR_H__

#include "ti_common_struct.h"
#include <memory>
#include <vector>
#include <map>

#include <nlohmann/json.hpp>
using namespace nlohmann;

const char TI_BarCycType_1m         = 1;    // 1 分钟
const char TI_BarCycType_5m         = 5;    // 5 分钟
const char TI_BarCycType_10m        = 10;   // 10 分钟
const char TI_BarCycType_15m        = 15;   // 15 分钟
const char TI_BarCycType_30m        = 30;   // 30 分钟
const char TI_BarCycType_1h         = 60;   // 1 小时
typedef char TI_BarCycType;


struct TiBarData
{
    int64_t timestamp;
    TI_BarCycType cyc;
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
    TI_BarCycType m_cyc;

private:
    //  生成OHLC  bar数据
    int64_t getBarTimestamp(const int64_t timestamp);
public:
    TiMinBar();
    TiMinBar(TI_BarCycType cyc);
    ~TiMinBar(){};


    //  处理成交数据
    void processTrade(const int64_t timestamp,  const double value);

    //  获取所有生成的OHLC  bar数据
    std::vector<TiBarDataPtr>  getBars();
    void getBarsJson(json& j);

    bool getLastBar(TiBarDataPtr& bar);
    bool getCloseSeries(std::vector<double>& closeSeries);
    

};

#endif //__TI_MIN_BAR_H__