#include "trade_responser.h"
#include "log.h"
#include "iniFile.h"

TradeResponser::TradeResponser(std::string configPath){
    m_config = NULL;
    msgContainer = NULL;
    loadConfig(configPath);
    if(m_config){
        bool flag = connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG_DEBUG("[%d]", flag);
    }
    msgContainer = cJSON_CreateObject();
};


TradeResponser::~TradeResponser(){
    if(m_config){
        delete m_config;
        m_config = NULL;
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
int TradeResponser::loadConfig(std::string iniFileName){
if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["trade_responser"]["ip"]);
    m_config->nPort       = _iniFile["trade_responser"]["port"];
    m_config->szAuth      = string(_iniFile["trade_responser"]["auth"]);
    
    m_config->szStreamKey     = string(_iniFile["trade_responser"]["response_stream_key"]);
    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szStreamKey.empty() )
    {
        LOG_ERROR("Not enough parameters in inifile");
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};



////////////////////////////////////////////////////////////////////////
// 外部调用方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
void TradeResponser::response(const char* type, cJSON *msg){
    if(!m_config){
        return;
    }
    cJSON_AddStringToObject(msgContainer, "type", type);
    cJSON_AddItemReferenceToObject(msgContainer, "data", msg);

    char* j = cJSON_Print(msgContainer);
    cJSON_Minify(j);
    xadd(m_config->szStreamKey.c_str(),j);

    cJSON_DeleteItemFromObject(msgContainer, "type");
    cJSON_DeleteItemFromObject(msgContainer, "data");
};