#ifndef __IA_ETF_INFO_STRUCT_H__
#define __IA_ETF_INFO_STRUCT_H__

#include <string>

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


struct IaEtfConstituentInfo {
    std::string m_tradeDate;
    std::string m_fundId;
    std::string m_symbol;
    std::string m_exchange;
    std::string m_name;
    std::string m_replace_flag;
    float m_replace_amount;
    float m_creation_amount;
    float m_redemption_amount;
    int m_disclosure_vol;
    float m_reality_vol;
    float m_cash_replaced_creation_premium_rate;
    float m_cash_replaced_redemption_discount_rate;
};


#endif