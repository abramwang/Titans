#include "redis_uv.h"
#include <unistd.h>

#include "ti_hx_quote_client.h"
#include "ti_quote_callback.h"

#define LOG_ENABLE True

class Callback: public TiQuoteCallback
{
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){};
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
        printf("[OnL2StockMatchesRtn] %s, %s, %d, %f, %d, %c\n", pData->symbol, pData->time_str, pData->seq, pData->price, pData->volume, pData->bs_flag);
    };
};

int main()
{

    Callback* cb = new Callback();

    TiHxQuoteClient spi("./config.ini", cb);

    spi.connect();

    while (1)
    {
        sleep(10000);
    }
    return 0;
}
