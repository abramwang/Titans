#include <iostream>
#include <unistd.h>
#include <uv.h>
#include <glog/logging.h>

#include "ia_etf_follow_trade_bot_gt.h"
#include "ti_mysql_client.h"
#include "ia_etf_info_mysql.h"

int main(int argc, char* argv[]) {
#if 1
    IaEtfInfoMysql sql_client(
        "119.13.76.95",
        20000,
        "ustc",
        "72666db4-a446-4db5-9f85-d60b83dfba93",
        "pcf"
    );

    /*
    json j;

    sql_client.query("SELECT * FROM etf_info ORDER BY m_tradeDate DESC LIMIT 100;", j);

    //std::cout << j << std::endl;

    */
   
    std::vector<std::map<std::string, std::string>> result;
    sql_client.query("SELECT * FROM etf_info ORDER BY m_tradeDate DESC LIMIT 100;", result);

    std::cout << result.size() << std::endl;

    for (auto& row : result) {
        for (auto& kv : row) {
            std::cout << kv.first << ": " << kv.second << std::endl;
        }
        break;
    }

    std::vector<IaEtfInfo> etfInfoList;
    sql_client.QueryEtfInfoList(etfInfoList);

    std::cout << etfInfoList.size() << std::endl;

    for (auto& etfInfo : etfInfoList) {
        std::cout << "etfInfo.m_tradeDate:" << etfInfo.m_tradeDate << std::endl;
        std::cout << "etfInfo.m_fundId:" << etfInfo.m_fundId << std::endl;
        std::cout << "etfInfo.m_exchange:" << etfInfo.m_exchange << std::endl;
        std::cout << "etfInfo.m_company:" << etfInfo.m_company << std::endl;
        std::cout << "etfInfo.m_fundName:" << etfInfo.m_fundName << std::endl;
        std::cout << "etfInfo.m_minUnit:" << etfInfo.m_minUnit << std::endl;
        std::cout << "etfInfo.m_preMinUnitTurnover:" << etfInfo.m_preMinUnitTurnover << std::endl;
        std::cout << "etfInfo.m_publicEstimatedCashDifference:" << etfInfo.m_publicEstimatedCashDifference << std::endl;
        std::cout << "etfInfo.m_realEstimatedCashDifference:" << etfInfo.m_realEstimatedCashDifference << std::endl;
        std::cout << "etfInfo.m_allowCreation:" << etfInfo.m_allowCreation << std::endl;
        std::cout << "etfInfo.m_allowRedemption:" << etfInfo.m_allowRedemption << std::endl;
        std::cout << "etfInfo.m_maxCashReplacePercentage:" << etfInfo.m_maxCashReplacePercentage << std::endl;
        std::cout << "etfInfo.m_maxCreationVol:" << etfInfo.m_maxCreationVol << std::endl;
        std::cout << "etfInfo.m_maxRedemptionVol:" << etfInfo.m_maxRedemptionVol << std::endl;
        std::cout << "etfInfo.m_requiredToDiscloseIOPV:" << etfInfo.m_requiredToDiscloseIOPV << std::endl;
        std::cout << "etfInfo.m_constituentStockNum:" << etfInfo.m_constituentStockNum << std::endl;


        break;
    }

    return 0;
#endif

	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);

    uv_loop_t* loop = uv_default_loop();

    IaEtfFollowTradeBotGt* commander = new IaEtfFollowTradeBotGt(loop, "./config.ini");

    commander->listen();
}
