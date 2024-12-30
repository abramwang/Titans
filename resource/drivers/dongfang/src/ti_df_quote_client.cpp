#include <iostream>
#include <iniFile.h>
#include <glog/logging.h>
#include "ti_encoding_tool.h"
#include "ti_df_quote_client.h"
#include "ti_quote_struct.h"
#include "datetime.h"

using namespace std;

TiDfQuoteClient::TiDfQuoteClient(std::string configPath, TiQuoteCallback* userCb)
{
    m_config = NULL;
    nReqId = 0;
    m_quote_api = NULL;
    m_cb = userCb;

    memset(&m_snapStockCash, 0, sizeof(TiQuoteSnapshotStockField));
    memset(&m_snapIndexCash, 0, sizeof(TiQuoteSnapshotIndexField));
    memset(&m_orderCash, 0, sizeof(TiQuoteOrderField));
    memset(&m_matchCash, 0, sizeof(TiQuoteMatchesField));

    loadConfig(configPath);
    if(!m_config){
        return;
    }

    m_quote_api = EMQ::API::QuoteApiLv2::CreateQuoteApiLv2("./emq_api_log/emq.log");
    m_quote_api->RegisterSpi(this);

    std::cout << "Login: " << m_config->szL2ShHost << ", " << m_config->szAccount << ", " << m_config->szPass << std::endl;
    auto ret = m_quote_api->Login("61.152.230.216", 8093, "510100025168", "OW4273");
    if (ret != 0) {
        printf("Login Failed.\n");
    }
    //m_quote_api->Start();
};

TiDfQuoteClient::~TiDfQuoteClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    m_cb = NULL;
    nReqId = 0;
};



int TiDfQuoteClient::loadConfig(std::string iniFileName)
{
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szL1Host = string(_iniFile["ti_df_quote_client"]["l1_host"]);
    m_config->szL2ShHost = string(_iniFile["ti_df_quote_client"]["l2_sh_host"]);
    m_config->szL2SzHost = string(_iniFile["ti_df_quote_client"]["l2_sz_host"]);
    m_config->bIsMulticast = _iniFile["ti_df_quote_client"]["is_multicast"];
    m_config->szL2Multicast = string(_iniFile["ti_df_quote_client"]["l2_multicast"]);
    m_config->szL2MulticastInterface = string(_iniFile["ti_df_quote_client"]["l2_multicast_interface"]);
    m_config->szProductInfo = string(_iniFile["ti_df_quote_client"]["product_info"]);
    m_config->szAccount = string(_iniFile["ti_df_quote_client"]["account"]);
    m_config->szPass = string(_iniFile["ti_df_quote_client"]["pass"]);
    
    return 0;
    if( m_config->szL1Host.empty() |
        m_config->szL2ShHost.empty() |
        m_config->szL2SzHost.empty() |
        m_config->szProductInfo.empty() |
        m_config->szAccount.empty() |
        m_config->szPass.empty() )
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void TiDfQuoteClient::connect()
{
    if(!m_config){
        LOG(INFO) << "[connect] Do not have config info";
        return;
    }

    if(m_config->bIsMulticast){
        // connect multicast
    }else{
        // connect tcp
    }
};

void TiDfQuoteClient::subData(const char* exchangeName, char* codeList[], size_t len)
{
    if(!m_config){
        LOG(INFO) << "[subData] Do not have config info";
        return;
    }

    if(m_config->bIsMulticast){
        // sub multicast
    }else{
        // sub tcp
    }
};