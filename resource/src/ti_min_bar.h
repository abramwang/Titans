#ifndef __TI_MIN_BAR_H__
#define __TI_MIN_BAR_H__

#include "ti_common_struct.h"
#include <memory>
#include <vector>


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
};

class TiMinBar {
private:
    std::vector<std::shared_ptr<TiBarData>> m_bars;

    void updateBar(const TiBarData& bar);
public:
    TiMinBar();
    ~TiMinBar();

    void update(const TiBarData& bar);
    void reset();

    const TiBarData& getBar() const;
    const TiBarData& getPrevBar() const;

};

#endif //__TI_MIN_BAR_H__