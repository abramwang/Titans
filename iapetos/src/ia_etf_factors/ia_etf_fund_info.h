#ifndef IA_ETF_FUND_INFO_H
#define IA_ETF_FUND_INFO_H

#include "redis_sync_handle.h"
#include <vector>
#include <map>
#include <memory>
#include <set>

class IaETFFundInfo
{
public:
/*
    ERT_CASH_FORBIDDEN = 0,             ///<禁止现金替代
    ERT_CASH_OPTIONAL,                  ///<可以现金替代
    ERT_CASH_MUST,                      ///<必须现金替代
    ERT_CASH_RECOMPUTE_INTER_SZ,        ///<深市退补现金替代
    ERT_CASH_MUST_INTER_SZ,             ///<深市必须现金替代
    ERT_CASH_RECOMPUTE_INTER_OTHER,     ///<非沪深市场成分证券退补现金替代（不适用于跨沪深港ETF产品）
    ERT_CASH_MUST_INTER_OTHER,          ///<表示非沪深市场成份证券必须现金替代（不适用于跨沪深港ETF产品）
    ERT_CASH_RECOMPUTE_INTER_HK,	    ///港市退补现金替代（仅适用于跨沪深港ETF产品）
    ERT_CASH_MUST_INTER_HK,		        ///港市必须现金替代（仅适用于跨沪深港ETF产品）
    EPT_INVALID                         ///<无效值
*/
    struct ConstituentStockInfo
    {
        std::string m_symbol;             // 代码
        std::string m_exchange;           // 交易所
        std::string m_name;               // 名称
        int         m_replace_flag;       // 替代标志
        double      m_replace_amount;     // 成分股替代标识为必须现金替代时候的总金额
        double      m_creation_amount;    // 申购时，成分股替代标识为必须现金替代时候的总金额
        double      m_redemption_amount;  // 赎回时，成分股替代标识为必须现金替代时候的总金额
        int         m_disclosure_vol;     // 披露数量
        double      m_reality_vol;        // 真实数量
    };

    struct FundInfo
    {
        std::string                       m_fundId;                         // 基金代码
        std::string                       m_exchange;                       // 交易所
        std::string                       m_company;                        // 基金公司
        int                               m_minUnit;                        // 最小申购赎回单位
        double                            m_preMinUnitTurnover;             // 上一日最小申购赎回单位净值
        double                            m_publicEstimatedCashDifference;  // 公布预估现金差额
        double                            m_realEstimatedCashDifference;    // 实际预估现金差额
        double                            m_constituentStockNum;            // 成分股数量
        double                            m_IOPV;                           // 份额净值
        double                            m_sh_commission;                  // 上海交易所手续费（除去印花税外的，包含佣金和过户费，本市场 万1，跨市场万8）
        double                            m_sz_commission;                  // 深圳交易所手续费（除去印花税外的，包含佣金和过户费，本市场 万1，跨市场万8）
        std::vector<ConstituentStockInfo> m_stockList;                      // 成分股详情列表
    };
private:
    RedisSyncHandle* m_redis;
    std::vector<std::string> m_etfList;
    std::map<std::string, std::shared_ptr<FundInfo>> m_fundInfos;
public:
    IaETFFundInfo(RedisSyncHandle* redis, std::string etf_list_key, std::string reality_key, std::string disclosure_key);
    virtual ~IaETFFundInfo(){};
private:
    void parseRedisETFListData(std::string etf_list_key, std::string reality_key, std::string disclosure_key);
    void parseRedisDisclosureInfoData(std::string disclosure_key);
    void parseRedisRealityInfoData(std::string reality_key);
public:
    std::map<std::string, std::shared_ptr<IaETFFundInfo::FundInfo>>* GetFundMap();
    void GetSymbolList(const char* exchange, std::set<std::string> &out);
};

#endif