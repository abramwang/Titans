#ifndef __IA_ETF_INFO_STRUCT_H__
#define __IA_ETF_INFO_STRUCT_H__

#include <string>

/*
CREATE TABLE `account` (
  `security_account` varchar(255) NOT NULL,
  `exchange` varchar(8) DEFAULT NULL,
  `funding_account` varchar(255) DEFAULT NULL,
  `uni_account` varchar(255) DEFAULT NULL,
  `broker` varchar(255) DEFAULT NULL,
  `broker_branch` varchar(255) DEFAULT NULL,
  `product_name` varchar(255) DEFAULT NULL,
  `tafundcode` varchar(255) DEFAULT NULL,
  `account_open_date` date DEFAULT NULL,
  `last_update_timestamp` datetime DEFAULT NULL,
  PRIMARY KEY (`security_account`),
  KEY `ix_account_exchange` (`exchange`),
  KEY `ix_account_uni_account` (`uni_account`),
  KEY `ix_account_funding_account` (`funding_account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
*/

struct IaAccountDBInfo {
    std::string security_account;
    std::string exchange;
    std::string funding_account;
    std::string uni_account;
    std::string broker;
    std::string broker_branch;
    std::string product_name;
    std::string tafundcode;
    std::string account_open_date;
    std::string last_update_timestamp;
};


/*
CREATE TABLE `etf_info` (
  `m_tradeDate` varchar(8) NOT NULL,
  `m_fundId` varchar(255) NOT NULL,
  `m_exchange` varchar(255) DEFAULT NULL,
  `m_company` varchar(255) DEFAULT NULL,
  `m_fundName` varchar(255) DEFAULT NULL,
  `m_minUnit` int DEFAULT NULL,
  `m_preMinUnitTurnover` float DEFAULT NULL,
  `m_publicEstimatedCashDifference` float DEFAULT NULL,
  `m_realEstimatedCashDifference` float DEFAULT NULL,
  `m_allowCreation` tinyint(1) DEFAULT NULL,
  `m_allowRedemption` tinyint(1) DEFAULT NULL,
  `m_maxCashReplacePercentage` float DEFAULT NULL,
  `m_maxCreationVol` float DEFAULT NULL,
  `m_maxRedemptionVol` float DEFAULT NULL,
  `m_requiredToDiscloseIOPV` tinyint(1) DEFAULT NULL,
  `m_constituentStockNum` int DEFAULT NULL,
  `m_isCrossMarket` tinyint(1) DEFAULT NULL,
  `m_hasRealityList` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`m_tradeDate`,`m_fundId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
*/

struct IaEtfInfo {
    std::string m_tradeDate;
    std::string m_fundId;
    std::string m_exchange;
    std::string m_company;
    std::string m_fundName;
    int m_minUnit;
    float m_preMinUnitTurnover;
    float m_publicEstimatedCashDifference;
    float m_realEstimatedCashDifference;
    bool m_allowCreation;
    bool m_allowRedemption;
    float m_maxCashReplacePercentage;
    float m_maxCreationVol;
    float m_maxRedemptionVol;
    bool m_requiredToDiscloseIOPV;
    int m_constituentStockNum;
    bool m_isCrossMarket;
    bool m_hasRealityList;
};

/*
CREATE TABLE `constituent_info` (
  `m_tradeDate` varchar(8) NOT NULL,
  `m_fundId` varchar(255) NOT NULL,
  `m_symbol` varchar(255) NOT NULL,
  `m_exchange` varchar(255) DEFAULT NULL,
  `m_name` varchar(255) DEFAULT NULL,
  `m_replace_flag` enum('ERT_CASH_FORBIDDEN','ERT_CASH_OPTIONAL','ERT_CASH_MUST','ERT_CASH_RECOMPUTE_INTER_SZ','ERT_CASH_MUST_INTER_SZ','ERT_CASH_RECOMPUTE_INTER_OTHER','ERT_CASH_MUST_INTER_OTHER','ERT_CASH_RECOMPUTE_INTER_HK','ERT_CASH_MUST_INTER_HK','EPT_INVALID') DEFAULT NULL,
  `m_replace_amount` float DEFAULT NULL,
  `m_creation_amount` float DEFAULT NULL,
  `m_redemption_amount` float DEFAULT NULL,
  `m_disclosure_vol` int DEFAULT NULL,
  `m_reality_vol` float DEFAULT NULL,
  `m_cash_replaced_creation_premium_rate` float DEFAULT NULL,
  `m_cash_replaced_redemption_discount_rate` float DEFAULT NULL,
  PRIMARY KEY (`m_tradeDate`,`m_fundId`,`m_symbol`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci;
*/

const char IA_ERT_CASH_FORBIDDEN             = 0;
const char IA_ERT_CASH_OPTIONAL              = 1;
const char IA_ERT_CASH_MUST                  = 2;
const char IA_ERT_CASH_RECOMPUTE_INTER_SZ    = 3;
const char IA_ERT_CASH_MUST_INTER_SZ         = 4;
const char IA_ERT_CASH_RECOMPUTE_INTER_OTHER = 5;
const char IA_ERT_CASH_MUST_INTER_OTHER      = 6;
const char IA_ERT_CASH_RECOMPUTE_INTER_HK    = 7;
const char IA_ERT_CASH_MUST_INTER_HK         = 8;
const char IA_EPT_INVALID                    = 9;
typedef char IA_ERT_CASH_TYPE;


struct IaEtfConstituentInfo {
    std::string m_tradeDate;
    std::string m_fundId;
    std::string m_symbol;
    std::string m_exchange;
    std::string m_name;
    IA_ERT_CASH_TYPE m_replace_flag;
    float m_replace_amount;
    float m_creation_amount;
    float m_redemption_amount;
    int m_disclosure_vol;
    float m_reality_vol;
    float m_cash_replaced_creation_premium_rate;
    float m_cash_replaced_redemption_discount_rate;
};

namespace IaEtfInfoStruct {
    inline IA_ERT_CASH_TYPE pares_ert_cash_type(std::string type_string){
        if (type_string == "ERT_CASH_FORBIDDEN") {
            return IA_ERT_CASH_FORBIDDEN;
        } else if (type_string == "ERT_CASH_OPTIONAL") {
            return IA_ERT_CASH_OPTIONAL;
        } else if (type_string == "ERT_CASH_MUST") {
            return IA_ERT_CASH_MUST;
        } else if (type_string == "ERT_CASH_RECOMPUTE_INTER_SZ") {
            return IA_ERT_CASH_RECOMPUTE_INTER_SZ;
        } else if (type_string == "ERT_CASH_MUST_INTER_SZ") {
            return IA_ERT_CASH_MUST_INTER_SZ;
        } else if (type_string == "ERT_CASH_RECOMPUTE_INTER_OTHER") {
            return IA_ERT_CASH_RECOMPUTE_INTER_OTHER;
        } else if (type_string == "ERT_CASH_MUST_INTER_OTHER") {
            return IA_ERT_CASH_MUST_INTER_OTHER;
        } else if (type_string == "ERT_CASH_RECOMPUTE_INTER_HK") {
            return IA_ERT_CASH_RECOMPUTE_INTER_HK;
        } else if (type_string == "ERT_CASH_MUST_INTER_HK") {
            return IA_ERT_CASH_MUST_INTER_HK;
        } else {
            return IA_EPT_INVALID;
        }
    };
}


#endif