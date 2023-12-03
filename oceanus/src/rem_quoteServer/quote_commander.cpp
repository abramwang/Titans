#include <iostream>
#include <unistd.h>

#include "quote_commander.h"
#include "log.h"
#include "iniFile.h"

#include <nlohmann/json.hpp>
using namespace nlohmann;

QuoteCommander::QuoteCommander(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    client = new EESQuoteClient(configPath);
    config = NULL;
	loadConfig(configPath);
    if(config){
        connect(config->szIp.c_str(), config->nPort, config->szAuth.c_str());
    }
}
QuoteCommander::~QuoteCommander(){
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
void QuoteCommander::onAuth(int err, const char* errStr){
    LOG_DEBUG("[%d, %s]", err, errStr);
	//test();

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


void QuoteCommander::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    LOG_DEBUG("%s, %s, %s, %s", streamKey, id, type, msg);
    printf("[QuoteCommander::onXreadgroupMsg]%s, %s, %s, %s \n", streamKey, id, type, msg);

    std::string command = std::string(type);
    auto j = json::parse(msg);
    std::cout << "test: " << j << std::endl;
    
    if(command == "querySymbolList"){
        client->querySymbolList();
    }
    if(command == "registerSymbol"){
        client->registerSymbol(
            j["type"].get<int>(),
            j["symbol"].get<std::string>().c_str());
    }
    if(command == "unregisterSymbol"){
        client->unregisterSymbol(
            j["type"].get<int>(),
            j["symbol"].get<std::string>().c_str());
    }


};

////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int QuoteCommander::loadConfig(std::string iniFileName){
    if(config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    config = new ConfigInfo();

    config->szIp        = string(_iniFile["quote_commander"]["ip"]);
    config->nPort       = _iniFile["quote_commander"]["port"];
    config->szAuth      = string(_iniFile["quote_commander"]["auth"]);

    config->nBlock          = _iniFile["quote_commander"]["block"];
    config->szStreamKey     = string(_iniFile["quote_commander"]["command_stream_key"]);
    config->szStreamGroup   = string(_iniFile["quote_commander"]["command_stream_group"]);
    config->szConsummerId   = string(_iniFile["quote_commander"]["consummer_id"]);
    
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
