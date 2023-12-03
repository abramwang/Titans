#ifndef COMPUTING_CENTER_H
#define COMPUTING_CENTER_H
#include <string>
#include <list>

#include "ti_hx_quote_client.h"
#include "ti_quote_callback.h"
#include "redis_sync_handle.h"
#include "iopv_operator.h"


class ComputingCenter
    : public TiQuoteCallback, public RedisSyncHandle 
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int         nPort;
        std::string szAuth;

        std::string szConfigKey;
        std::string szStreamKey;
    } ConfigInfo;

    ComputingCenter(std::string configPath);
    virtual ~ComputingCenter(){};

private:
    ConfigInfo* m_config;
    TiHxQuoteClient* m_quoteClient;
    std::list<IopvOperator*> m_operatorList;
    
    int loadConfig(std::string iniFileName);
    int loadIOPVConfig();
    void subStock(const char* exchangeName);

public: //行情回调
    void OnTradingDayRtn(const unsigned int day, const char* exchangeName);

    void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData);
    void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    void OnL2StockOrderRtn(const TiQuoteOrderField* pData){};


};

#endif