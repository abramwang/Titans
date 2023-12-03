#include <iostream>
#include <unistd.h>

#include "ia_etf_xtp_trader.h"
#include "iniFile.h"
#include "datetime.h"
#include <glog/logging.h>
#include <nlohmann/json.hpp>
using namespace nlohmann;

IaETFXtpTrader::IaETFXtpTrader(uv_loop_s* loop, std::string configPath)
    : RedisCommander(loop)
{
    m_client = new TiZtTraderClient(configPath, this);
    m_config = NULL;
    loadConfig(configPath);
    if(m_config){
        connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        
        bool flag = m_redis.connect(m_config->szIp.c_str(), m_config->nPort, m_config->szAuth.c_str());
        LOG(INFO) << "[IaETFFactorCenter] flag: " << flag;
    }
}
IaETFXtpTrader::~IaETFXtpTrader(){
    if(m_client){
        delete m_client;
        m_client = NULL;
    }
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
};

void IaETFXtpTrader::init_redis_command_work(uv_work_t* req)
{
    IaETFXtpTrader* trader = (IaETFXtpTrader*)req->data;
    auto iter = trader->m_etf_info_map.begin();
    for(; iter != trader->m_etf_info_map.end(); iter++){
        std::cout << "init_redis_command_work: " << iter->first << std::endl;
        try
        {
            trader->m_redis.hmset(
                trader->m_config->szConfigETFDisclosureInfoKey.c_str(), 
                iter->first.c_str(), 
                iter->second.dump().c_str());
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    char datetime[128];
    datetime::get_format_now_time_ms(datetime, 128);
    trader->m_redis.hmset(
        "task_worked_time", 
        "ia_etf_disclosure_info_xtp", 
        datetime);
};
void IaETFXtpTrader::after_redis_command_work(uv_work_t* req, int status)
{
    std::cout << "after_redis_command_work" << std::endl;
    IaETFXtpTrader* trader = (IaETFXtpTrader*)req->data;
    delete req;
    trader->disconnect();
    uv_stop(trader->loop);
    uv_loop_close(trader->loop);
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////
void IaETFXtpTrader::OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str)
{
    if (err != 0)
    {
        std::cout << "OnCommonJsonRespones: " << err_str << std::endl;
        return;
    }
    if ((*rspData)["type"].get<std::string>() == "XtpQueryETFRsp")
    {
        std::string etf_symbol = (*rspData)["data"]["symbol"].get<std::string>();
        m_etf_info_map[etf_symbol] = (*rspData)["data"];
        m_etf_info_map[etf_symbol]["component_list"]= json::array();
        m_etf_basket_task[etf_symbol] = (*rspData)["data"]["exchange"].get<std::string>();
    }
    if ((*rspData)["type"].get<std::string>() == "XtpQueryETFBasketRsp")
    {
        std::string etf_symbol = (*rspData)["data"]["etf_symbol"].get<std::string>();
        auto iter = m_etf_info_map.find(etf_symbol);
        if (iter != m_etf_info_map.end())
        {
             m_etf_info_map[etf_symbol]["component_list"][m_etf_info_map[etf_symbol]["component_list"].size()] = (*rspData)["data"];
        }
    }
    if (isLast)
    {        
        std::cout << "OnCommonJsonRespones: " << rspData->dump() << std::endl;
        std::cout << m_etf_basket_task.size() << std::endl;
        if(!m_etf_basket_task.empty()){
            TI_ExchangeType exchange_type;
            strcpy(exchange_type, m_etf_basket_task.begin()->second.c_str());
            m_client->QueryETFBasketList(exchange_type, m_etf_basket_task.begin()->first.c_str());
            m_etf_basket_task.erase(m_etf_basket_task.begin());
        }else{
            uv_work_t* req = new uv_work_t();
            req->data = this;
            uv_queue_work(loop, req, init_redis_command_work, after_redis_command_work);
        }
        std::cout << m_etf_basket_task.size() << std::endl;
        return;
    }
};   

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

/// @brief 
/// @param err 
/// @param errStr 
void IaETFXtpTrader::onAuth(int err, const char* errStr){
    if(m_config){
        /*
        subStream(m_config->szStreamGroup.c_str(),
            m_config->szStreamKey.c_str(),
            m_config->szConsummerId.c_str(), 
            m_config->nBlock);
        */
    }
    if(m_client){
        m_client->connect();
        m_client->QueryAllETFBaseInfo();
    }
}

void IaETFXtpTrader::onCommand(int err, std::vector<std::string> *rsp)
{
    std::cout << "onCommand:" << err << " " << rsp->size() <<  std::endl;
};

void IaETFXtpTrader::onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){
    printf("[TradeCommander::onXreadgroupMsg]%s, %s, %s, %s \n", streamKey, id, type, msg);

    std::string command = std::string(type);
    auto j = json::parse(msg);
    std::cout << "test: " << j << std::endl;
    
    /*
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
    */

};


////////////////////////////////////////////////////////////////////////
// 内部方法
////////////////////////////////////////////////////////////////////////

/// @brief 加载配置文件
/// @param iniFilePath 
int IaETFXtpTrader::loadConfig(std::string iniFileName){
    if(m_config){
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();

    m_config->szIp        = string(_iniFile["ia_etf_xtp_trader"]["ip"]);
    m_config->nPort       = _iniFile["ia_etf_xtp_trader"]["port"];
    m_config->szAuth      = string(_iniFile["ia_etf_xtp_trader"]["auth"]);

    m_config->nBlock          = _iniFile["ia_etf_xtp_trader"]["block"];
    m_config->szStreamKey     = string(_iniFile["ia_etf_xtp_trader"]["command_stream_key"]);
    m_config->szStreamGroup   = string(_iniFile["ia_etf_xtp_trader"]["command_stream_group"]);
    m_config->szConsummerId   = string(_iniFile["ia_etf_xtp_trader"]["consummer_id"]);

    m_config->szConfigETFDisclosureInfoKey   = string(_iniFile["ia_etf_xtp_trader"]["etf_disclosure_info_key"]);
    
    if( m_config->szIp.empty() |
        !m_config->nPort |
        m_config->szStreamGroup.empty() |
        m_config->szStreamKey.empty() | 
        m_config->szStreamGroup.empty() |
        m_config->szConsummerId.empty() |
        m_config->szConfigETFDisclosureInfoKey.empty() )
    {
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};