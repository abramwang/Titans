#include "redis_uv.h"
#include <cstring>
#include <unistd.h>

#include "ti_hx_quote_client.h"
#include "ti_quote_callback.h"

#define LOG_ENABLE True

class Callback: public TiQuoteCallback
{
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){
        printf("[OnTradingDayRtn] %d, %s\n", day, exchangeName);

        if (strcmp(exchangeName, "SH") == 0)
        {
            char* shCodeList[3] = {"600000", "600004", "600008"};
            if(client)
            {
                client->subData("SH", shCodeList, 3);
            }
        }
    };
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData){};
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
        printf("[OnL2StockMatchesRtn] %s, %s, %d, %f, %d, %c\n", pData->symbol, pData->time_str, pData->seq, pData->price, pData->volume, pData->bs_flag);
    };
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};

    Callback(){
        client = NULL;
    };
public:
    TiHxQuoteClient* client;
};

int main()
{
    Callback* cb = new Callback();

    TiHxQuoteClient api("./config.ini", cb);
    cb->client = &api;

    api.connect();


    while (1)
    {
        sleep(10000);
    }
    return 0;
}
