#include "quote_responser.h"
#include "log.h"
#include "iniFile.h"

QuoteResponser::QuoteResponser(std::string configPath){
    config = NULL;
    msgContainer = NULL;
    loadConfig(configPath);
    if(config){
        bool flag = connect(config->szIp.c_str(), config->nPort, config->szAuth.c_str());
        LOG_DEBUG("[%d]", flag);
    }
    msgContainer = cJSON_CreateObject();
};


QuoteResponser::~QuoteResponser(){
    if(config){
        delete config;
        config = NULL;
    }
    if(msgContainer){
        cJSON_Delete(msgContainer);
        msgContainer = NULL;
    }
};



////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int QuoteResponser::loadConfig(std::string iniFileName){
    if(config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    config = new ConfigInfo();

    config->szIp        = string(_iniFile["quote_responser"]["ip"]);
    config->nPort       = _iniFile["quote_responser"]["port"];
    config->szAuth      = string(_iniFile["quote_responser"]["auth"]);
    
    config->szStreamKey     = string(_iniFile["quote_responser"]["response_stream_key"]);
    
    if( config->szIp.empty() |
        !config->nPort |
        config->szStreamKey.empty() )
    {
        LOG_ERROR("Not enough parameters in inifile");
        delete config;
        config = NULL;
        return -1;
    }
    return 0;
};

////////////////////////////////////////////////////////////////////////
// 外部调用方法
////////////////////////////////////////////////////////////////////////


void QuoteResponser::updateSymbolInfo(cJSON* info){
    if(!config){
        return;
    }
    char _symbolSetKey[256] = {0};
    sprintf(_symbolSetKey, "%s.symbol_set", config->szStreamKey.c_str());
    
    char* j = cJSON_Print(info);
    cJSON_Minify(j);

    sadd(_symbolSetKey, j);
};

void QuoteResponser::updateSnapshot(const char* ex, std::string symbol, cJSON *data){
    if(!config){
        return;
    }
    
    cJSON_AddStringToObject(msgContainer, "type", "snapshot");
    cJSON_AddItemReferenceToObject(msgContainer, "data", data);

    char _streamKey[256] = {0};
    sprintf(_streamKey, "%s.data.%s.%s", config->szStreamKey.c_str(), ex, symbol.substr(0, 3).c_str());

    char* j = cJSON_Print(msgContainer);
    cJSON_Minify(j);
    xadd(_streamKey,j);

    cJSON_DeleteItemFromObject(msgContainer, "type");
    cJSON_DeleteItemFromObject(msgContainer, "data");
};