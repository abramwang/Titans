#ifndef TI_DF_QUOTE_CLIENT_H
#define TI_DF_QUOTE_CLIENT_H

#include <string>
#include "ti_quote_callback.h"
#include "quote_api_lv2.h"

#include "ti_df_quote_l1_client.h"


class TiDfQuoteClient : public EMQ::API::QuoteSpiLv2
{
public:
    typedef struct ConfigInfo
    {
        std::string szL1Host;           //level 1 host
        int nL1Port;                    //level 1 port

        std::string szL1Account;
        std::string szL1Pass;
        
        std::string szL2Host;           //level 2 host
        int nL2Port;                    //level 2 port

        std::string szL2MulticastDevice;    //组播行情接收网卡地址

        std::string szL2Account;
        std::string szL2Pass;
    } ConfigInfo;
protected:
    // inherit from EMQ::API::QuoteSpiLv2
    // 深交所快照行情
    void OnLv2SnapSze(EMQSzeSnap *snap);
    // 深交所逐笔合并行情
    void OnLv2TickSze(EMQSzeTick *tick);
    // 深交所指数行情
    void OnLv2IndexSze(EMQSzeIdx *idx);
    // 深交所债券快照行情
    void OnLv2BondSnapSze(EMQSzeBondSnap *bond_snap);
    // 深交所债券逐笔合并行情
    void OnLv2BondTickSze(EMQSzeBondTick *bond_tick);
    // 深交所建树行情
    void OnLv2TreeSze(EMQSzeTree *tree);

    // 上交所快照行情
    void OnLv2SnapSse(EMQSseSnap *snap);
    // 上交所逐笔合并行情
    void OnLv2TickSse(EMQSseTick *tick);
    // 上交所指数行情
    void OnLv2IndexSse(EMQSseIdx *idx);
    // 上交所债券快照行情
    void OnLv2BondSnapSse(EMQSseBondSnap *bond_snap);
    // 上交所债券逐笔行情
    void OnLv2BondTickSse(EMQSseBondTick *bond_tick);
    // 上交所建树行情
    void OnLv2TreeSse(EMQSseTree *tree);

private:
    ConfigInfo* m_config;
    TiQuoteCallback* m_cb;
    unsigned int m_trading_day;
    
    TiQuoteSnapshotStockField   m_snapStockCash;
    TiQuoteSnapshotIndexField   m_snapIndexCash;
    TiQuoteOrderField           m_orderCash;
    TiQuoteMatchesField         m_matchCash;
    
    TiDfQuoteL1Client* m_quoteL1Client;
    EMQ::API::QuoteApiLv2 *m_quote_api;
private:
    int loadConfig(std::string iniFileName);
    void formatQuoteUpdatetime(unsigned long long quote_update_time, int32_t &date, int32_t &time, int64_t &timestamp);
public:
    TiDfQuoteClient(std::string configPath, TiQuoteCallback* userCb);
    ~TiDfQuoteClient();
public:
    void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif