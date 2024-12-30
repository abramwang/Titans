#ifndef TI_DF_QUOTE_CLIENT_H
#define TI_DF_QUOTE_CLIENT_H

#include <string>
#include "ti_quote_callback.h"
#include "quote_api_lv2.h"

using namespace EMQ::API;

class TiDfQuoteClient: public EMQ::API::QuoteSpiLv2
{
public:
    typedef struct ConfigInfo
    {
        std::string szL1Host;        //level 1 host

        std::string szL2ShHost;
        std::string szL2SzHost;

        bool bIsMulticast;                  //是否组播行情
        std::string szL2Multicast;          //组播行情地址
        std::string szL2MulticastInterface; //组播行情接收网卡地址

        std::string szProductInfo;
        std::string szAccount;
        std::string szPass;
    } ConfigInfo;
    
    typedef struct LoginStatus
    {
        int nReqId;
        bool bLoginSuccess;
    } LoginStatus;

private:
    int nReqId;
    ConfigInfo* m_config;
    QuoteApiLv2 *m_quote_api;

    TiQuoteCallback* m_cb;
    TiQuoteSnapshotStockField   m_snapStockCash;
    TiQuoteSnapshotIndexField   m_snapIndexCash;
    TiQuoteOrderField           m_orderCash;
    TiQuoteMatchesField         m_matchCash;

public:
    TiDfQuoteClient(std::string configPath, TiQuoteCallback* userCb);
    virtual ~TiDfQuoteClient();

protected: // inherit from EMQ::API::QuoteSpiLv2
    // 深交所快照行情
    void OnLv2SnapSze(EMQSzeSnap *snap) {};
    // 深交所逐笔合并行情
    void OnLv2TickSze(EMQSzeTick *tick) {};
    // 深交所指数行情
    void OnLv2IndexSze(EMQSzeIdx *idx) {};
    // 深交所债券快照行情
    //void OnLv2BondSnapSze(EMQSzeBondSnap *bond_snap) {};
    // 深交所债券逐笔合并行情
    //void OnLv2BondTickSze(EMQSzeBondTick *bond_tick) {};
    // 深交所建树行情
    void OnLv2TreeSze(EMQSzeTree *tree) {};

    // 上交所快照行情
    void OnLv2SnapSse(EMQSseSnap *snap) {};
    // 上交所逐笔合并行情
    void OnLv2TickSse(EMQSseTick *tick) {};
    // 上交所指数行情
    void OnLv2IndexSse(EMQSseIdx *idx) {};
    // 上交所债券快照行情
    //void OnLv2BondSnapSse(EMQSseBondSnap *bond_snap) {};
    // 上交所债券逐笔行情
    void OnLv2BondTickSse(EMQSseBondTick *bond_tick) {};
    // 上交所建树行情
    void OnLv2TreeSse(EMQSseTree *tree) {};
private:
    int loadConfig(std::string iniFileName);
public:
	void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
};

#endif