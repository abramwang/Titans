#ifndef TI_GF_QUOTE_CLIENT_H
#define TI_GF_QUOTE_CLIENT_H

#include <string>
#include <iostream>
#include <atomic>

#include "quote/ama.h"
#include "quote/ama_tools.h"

#include "ti_quote_client.h"
#include "ti_quote_callback.h"


#include <nlohmann/json.hpp>
using namespace nlohmann;

class TiGfQuoteClient: 
    public amd::ama::IAMDSpi, public TiQuoteClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szIp;
        int nPort;
        std::string szLocalIp;
        std::string szModel;

        std::string szUser;
        std::string szPass;
    } ConfigInfo;
    
private:
    ConfigInfo* m_config;
    amd::ama::Cfg m_cfg;
    unsigned int m_trading_day;


    TiQuoteCallback* m_cb;
public:
    TiGfQuoteClient(std::string configPath, TiQuoteCallback* userCb);
    virtual ~TiGfQuoteClient();

public:
    // 定义日志回调处理方法
    virtual void OnLog(const int32_t& level,
                       const char* log,
                       uint32_t len);

    // 定义监控回调处理方法
    virtual void OnIndicator(const char* indicator,
                             uint32_t len);

    // 定义事件回调处理方法  level 对照 EventLevel 数据类型 code 对照 EventCode 数据类型
    virtual void OnEvent(uint32_t level, uint32_t code, const char* event_msg, uint32_t len);

    // 定义快照数据回调处理方法
    virtual void OnMDSnapshot(amd::ama::MDSnapshot* snapshot,
                              uint32_t cnt);

    //定义指数快照数据回调处理方法
    virtual void OnMDIndexSnapshot(amd::ama::MDIndexSnapshot* snapshots, uint32_t cnt);

    //定义逐笔委托数据回调处理方法
    virtual void OnMDTickOrder(amd::ama::MDTickOrder* ticks, uint32_t cnt);

    // 定义逐笔成交数据回调处理方法
    virtual void OnMDTickExecution(amd::ama::MDTickExecution* tick, uint32_t cnt);

    //定义委托簿数据回调处理方法(本地构建模式下非单线程递交, cfg.thread_num为递交委托簿数据线程数, 服务端推送模式下为单线程递交)
    virtual void OnMDOrderBook(std::vector<amd::ama::MDOrderBook>& order_book);
    

private:
    void onLogin();
private:
    int loadConfig(std::string iniFileName);
    void init_ama_config();
public:
	void connect();
    void subData(const char* exchangeName, char* codeList[], size_t len);
    
    bool getETFCodeList(json &out);
    bool getCodeList();
};

#endif

