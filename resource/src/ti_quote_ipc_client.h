#ifndef TI_QUOTE_IPC_CLIENT_LOCAL_H
#define TI_QUOTE_IPC_CLIENT_LOCAL_H

#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <uv.h>
#include <csignal>
#include "concurrentqueue.h"
#include "ti_quote_callback.h"
#include "ti_quote_depth_callback.h"
#include "ti_quote_cache.h"
#include "ti_quote_client.h"
#include "ti_ipc_api.h"

class TiQuoteIpcClient:
    public TiQuoteClient
{
public:
    typedef struct ConfigInfo
    {
        std::string szQuoteTopic;
        std::vector<std::string> szQuoteCodeVec;
    } ConfigInfo;

    static void quote_cb(const char* topic_, void* data_, size_t len_);
    
    struct SubQuoteReqInfo
    { 
        std::string                   m_topic;      //
        ti_ipc_callback_fun_ptr       m_cb_ptr;     //
        TiQuoteIpcClient*             m_client;     //
    };

    static void init_sub_quote_work(uv_work_t* req);
    static void after_sub_quote_work(uv_work_t* req, int status);

    static void recv_data_cb(uv_async_t* handle);

    static void signal_handler(int signal);

    static TiQuoteIpcClient* g_client;
private:
    ConfigInfo* m_config;
    uv_loop_t* m_loop;
    uv_async_t* m_recv_signal;
    TiQuoteCallback* m_cb;
    TiQuoteDepthCallback* m_depth_cb;
    std::set<std::string>          m_sh_symbol_set;       //
    std::set<std::string>          m_sz_symbol_set;       //
    std::set<std::string>          m_cf_symbol_set;       //    期货
protected:
    TiQuoteCache* m_cache;
    moodycamel::ConcurrentQueue<void*> m_queue;
    bool isSubscribed(const char* exchangeName, const char* symbol);
private:
    int loadConfig(std::string iniFileName);
    void parseTopics();
public:
	void connect(){};
    void subData(const char* exchangeName, char* codeList[], size_t len);
    void run(const char* topic);
    void stop();
public:
    TiQuoteIpcClient(std::string configPath, uv_loop_t* loop, TiQuoteCallback* userCb);
    TiQuoteIpcClient(std::string configPath, uv_loop_t* loop, TiQuoteCallback* userCb, TiQuoteDepthCallback* depthCb);
    virtual ~TiQuoteIpcClient(){};
};

#endif 