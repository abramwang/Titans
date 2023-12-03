#include <iostream>
#include <unistd.h>

#include "trade_commander.h"
#include "log.h"
#include "iniFile.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

TradeCommander::TradeCommander(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    client = new EESTraderClient(configPath);
    config = NULL;
    loadConfig(configPath);
    if(config){
        connect(config->szIp.c_str(), config->nPort, config->szAuth.c_str());
    }
}
TradeCommander::~TradeCommander(){
    if(client){
        delete client;
        client = NULL;
    }
    if(config){
        delete config;
        config = NULL;
    }
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void TradeCommander::onAuth(int err, const char* errStr){
    LOG_DEBUG("[%d, %s]", err, errStr);

    if(config){
        subCommand(config->szStreamGroup.c_str(),
            config->szStreamKey.c_str(),
            config->szConsummerId.c_str(), 
            config->nBlock);
    }
    if(client){
        client->connect();
    }
}

void TradeCommander::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    LOG_DEBUG("%s, %s, %s, %s", streamKey, id, type, msg);
    printf("[TradeCommander::onXreadgroupMsg]%s, %s, %s, %s \n", streamKey, id, type, msg);

    std::string command = std::string(type);
    auto j = json::parse(msg);
    std::cout << "test: " << j << std::endl;
    
    if(command == "querySymbolList"){
        client->querySymbolList();
    }
    if(command == "queryUserAccount"){
        client->queryUserAccount();
    }
    if(command == "queryAccountPosition"){
        client->queryAccountPosition(j["accountId"].get<std::string>().c_str(), j["nReqId"].get<int>());
    }
    if(command == "queryAccountBP"){
        client->queryAccountBP(j["accountId"].get<std::string>().c_str(), j["nReqId"].get<int>());
    }
    if(command == "queryAccountTradeMargin"){
        client->queryAccountTradeMargin(j["accountId"].get<std::string>().c_str());
    }
    if(command == "queryAccountTradeFee"){
        client->queryAccountTradeFee(j["accountId"].get<std::string>().c_str());
    }
    if(command == "enterOrder"){
        EES_EnterOrderField order;
        strcpy(order.m_Account, j["m_Account"].get<std::string>().c_str());
        order.m_Side = (EES_SideType)j["m_Side"].get<unsigned int>();
        order.m_Exchange = (EES_ExchangeID)j["m_Exchange"].get<unsigned int>();
        strcpy(order.m_Symbol, j["m_Symbol"].get<std::string>().c_str());
        order.m_SecType = (EES_SecType)j["m_SecType"].get<unsigned int>();
        order.m_Price = j["m_Price"].get<double>();
        order.m_Qty = j["m_Qty"].get<unsigned int>();
        //order.m_ClientOrderToken = j["m_ClientOrderToken"].get<unsigned int>();
        order.m_Tif = j["m_Tif"].get<unsigned int>();
        order.m_MinQty = j["m_MinQty"].get<unsigned int>();
        order.m_HedgeFlag = j["m_HedgeFlag"].get<unsigned int>();
        order.m_OrderType = (EES_OrderType)j["m_OrderType"].get<unsigned int>();
        
        client->getMaxToken(&order.m_ClientOrderToken);
        order.m_ClientOrderToken += 1;
        printf("order.m_ClientOrderToken : %d\n", order.m_ClientOrderToken);

        client->enterOrder(&order);
    }
    if(command == "cancelOrder"){
        EES_CancelOrder cxlOrder;
        cxlOrder.m_MarketOrderToken = j["m_MarketOrderToken"].get<long long>();
        strcpy(cxlOrder.m_Account, j["m_Account"].get<std::string>().c_str());
        client->cancelOrder(&cxlOrder);
    }

    if(command == "queryAccountOrder"){
        client->queryAccountOrder(j["accountId"].get<std::string>().c_str());
    }
    if(command == "queryAccountOrderExecution"){
        client->queryAccountOrderExecution(j["accountId"].get<std::string>().c_str());
    }
    if(command == "queryMarketMBLData"){
        client->queryMarketMBLData(
            j["nReqId"].get<int>(),
            (EES_ExchangeID)j["exchangeID"].get<int>(),
            j["nSide"].get<int>()
        );
    }

};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int TradeCommander::loadConfig(std::string iniFileName){
    if(config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    config = new ConfigInfo();

    config->szIp        = string(_iniFile["trade_commander"]["ip"]);
    config->nPort       = _iniFile["trade_commander"]["port"];
    config->szAuth      = string(_iniFile["trade_commander"]["auth"]);

    config->nBlock          = _iniFile["trade_commander"]["block"];
    config->szStreamKey     = string(_iniFile["trade_commander"]["command_stream_key"]);
    config->szStreamGroup   = string(_iniFile["trade_commander"]["command_stream_group"]);
    config->szConsummerId   = string(_iniFile["trade_commander"]["consummer_id"]);
    
    if( config->szIp.empty() |
        !config->nPort |
        config->szStreamGroup.empty() |
        config->szStreamKey.empty() | 
        config->szStreamGroup.empty() |
        config->szConsummerId.empty())
    {
        LOG_ERROR("Not enough parameters in inifile");
        delete config;
        config = NULL;
        return -1;
    }
    return 0;
};
