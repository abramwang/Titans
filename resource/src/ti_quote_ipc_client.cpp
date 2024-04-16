#include "ti_quote_ipc_client.h"
#include <glog/logging.h>
#include "iniFile.h"

TiQuoteIpcClient* TiQuoteIpcClient::g_client = NULL;

void TiQuoteIpcClient::quote_cb(const char* topic_, void* data_, size_t len_)
{
    if (g_client)
    {
        void* pData = malloc(TI_QUOTE_CACHE_MAX_LEN);
        memcpy(pData, data_, len_);
        g_client->m_queue.enqueue(pData);
        uv_async_send(g_client->m_recv_signal);
        return;
    }
};

void TiQuoteIpcClient::init_sub_quote_work(uv_work_t* req)
{
    SubQuoteReqInfo* repInfo = (SubQuoteReqInfo*)req->data;
    ti_ipc_sub(repInfo->m_topic.c_str(), repInfo->m_cb_ptr);
};

void TiQuoteIpcClient::after_sub_quote_work(uv_work_t* req, int status)
{
    SubQuoteReqInfo* repInfo = (SubQuoteReqInfo*)req->data;
    delete repInfo;
    req->data = NULL;
    delete req;
};

void TiQuoteIpcClient::recv_data_cb(uv_async_t* handle)
{
    TiQuoteIpcClient* client = (TiQuoteIpcClient*)handle->data;
    void* buff = NULL;
    while (client->m_queue.try_dequeue(buff))
    {
        if (client->m_cb)
        {
            client->m_cache->reset_buffer(buff, TI_QUOTE_CACHE_MAX_LEN);
            TiQuoteCache::DataPtr* dataPtr = NULL;
            while (client->m_cache->try_getNext(dataPtr))
            {
                switch (dataPtr->dataType)
                {
                case TI_QUOTE_DATA_TYPE_SNAPSHOT_INDEX:
                {
                    TiQuoteSnapshotIndexField* pData = (TiQuoteSnapshotIndexField*)dataPtr->data;
                    if (client->isSubscribed(pData->exchange, pData->symbol))
                    {
                        client->m_cb->OnL2IndexSnapshotRtn(pData);
                    }
                    break;
                }
                case TI_QUOTE_DATA_TYPE_SNAPSHOT_FUTURE:
                {
                    TiQuoteSnapshotFutureField* pData = (TiQuoteSnapshotFutureField*)dataPtr->data;
                    if (client->isSubscribed(pData->exchange, pData->symbol))
                    {
                        client->m_cb->OnL2FutureSnapshotRtn(pData);
                    }
                    break;
                }
                case TI_QUOTE_DATA_TYPE_SNAPSHOT_STOCK:
                {
                    TiQuoteSnapshotStockField* pData = (TiQuoteSnapshotStockField*)dataPtr->data;
                    if (client->isSubscribed(pData->exchange, pData->symbol))
                    {
                        client->m_cb->OnL2StockSnapshotRtn(pData);
                    }
                    break;
                }
                case TI_QUOTE_DATA_TYPE_MATCH:
                {
                    TiQuoteMatchesField* pData = (TiQuoteMatchesField*)dataPtr->data;
                    if (client->isSubscribed(pData->exchange, pData->symbol))
                    {
                        client->m_cb->OnL2StockMatchesRtn(pData);
                    }
                    break;
                }
                case TI_QUOTE_DATA_TYPE_ORDER:
                {
                    TiQuoteOrderField* pData = (TiQuoteOrderField*)dataPtr->data;
                    if (client->isSubscribed(pData->exchange, pData->symbol))
                    {
                        client->m_cb->OnL2StockOrderRtn(pData);
                    }
                    break;
                }
                case TI_QUOTE_DATA_TYPE_ORDERBOOK_LEVEL:
                {
                    TiQuoteOrderBookField* pData = (TiQuoteOrderBookField*)dataPtr->data;
                    if (client->isSubscribed(pData->exchange, pData->symbol))
                    {
                        if (client->m_depth_cb)
                        {
                            client->m_depth_cb->OnDepthOrderBookLevelRtn(pData);
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
        if (buff)
        {
            free(buff);
            buff = NULL;
        }
    }
};

void TiQuoteIpcClient::signal_handler(int signal) {
    if (TiQuoteIpcClient::g_client)
    {
        g_client->stop();
    }
    exit(signal);
}

TiQuoteIpcClient::TiQuoteIpcClient(std::string configPath, uv_loop_t* loop, TiQuoteCallback* userCb)
{
    if(TiQuoteIpcClient::g_client){
        LOG(INFO) << "[TiQuoteIpcClient] Only one client is allowed";
        return;
    }
    TiQuoteIpcClient::g_client = this;
    m_config = NULL;
    m_loop = loop;
    m_recv_signal = new uv_async_t();
    m_recv_signal->data = this;
    uv_async_init(m_loop, m_recv_signal, recv_data_cb);
    m_cb = userCb;
    m_depth_cb = NULL;
    m_cache = new TiQuoteCache();
    loadConfig(configPath);
    signal(SIGINT, TiQuoteIpcClient::signal_handler);
};

TiQuoteIpcClient::TiQuoteIpcClient(std::string configPath, uv_loop_t* loop, TiQuoteCallback* userCb, TiQuoteDepthCallback* depthCb)
{
    if(TiQuoteIpcClient::g_client){
        LOG(INFO) << "[TiQuoteIpcClient] Only one client is allowed";
        return;
    }
    TiQuoteIpcClient::g_client = this;
    m_config = NULL;
    m_loop = loop;
    m_recv_signal = new uv_async_t();
    m_recv_signal->data = this;
    uv_async_init(m_loop, m_recv_signal, recv_data_cb);
    m_cb = userCb;
    m_depth_cb = depthCb;
    m_cache = new TiQuoteCache();
    loadConfig(configPath);
    signal(SIGINT, TiQuoteIpcClient::signal_handler);
};

bool TiQuoteIpcClient::isSubscribed(const char* exchangeName, const char* symbol)
{
    if(!strcmp(exchangeName, "SH"))
    {
        if (m_sh_symbol_set.empty())
        {
            return true;
        }else{
            return m_sh_symbol_set.find(symbol) != m_sh_symbol_set.end();
        }
    }
    if(!strcmp(exchangeName, "SZ"))
    {
        if (m_sz_symbol_set.empty())
        {
            return true;
        }else{
            return m_sz_symbol_set.find(symbol) != m_sz_symbol_set.end();
        }
    }
    if(!strcmp(exchangeName, "CF"))
    {
        if (m_cf_symbol_set.empty())
        {
            return true;
        }else{
            return m_cf_symbol_set.find(symbol) != m_cf_symbol_set.end();
        }
    }
    return false;
};

int TiQuoteIpcClient::loadConfig(std::string iniFileName)
{
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szQuoteTopic                  = string(_iniFile["ti_quote_ipc_client"]["quote_topic"]);

    std::cout << "szQuoteTopic: " << m_config->szQuoteTopic << std::endl;

    parseTopics();

    
    if( m_config->szQuoteTopic.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};

void TiQuoteIpcClient::parseTopics()
{
    std::istringstream iss(m_config->szQuoteTopic);
    m_config->szQuoteCodeVec.clear();
    std::string topic;
    while (std::getline(iss, topic, ',')) {
        std::cout << "topic:" << topic << std::endl;
        m_config->szQuoteCodeVec.push_back(topic);
    }
};

void TiQuoteIpcClient::subData(const char* exchangeName, char* codeList[], size_t len)
{
    if(!strcmp(exchangeName, "SH"))
    {
        for (size_t i = 0; i < len; i++)
        {
            m_sh_symbol_set.insert(codeList[i]);
        }
        return; 
    }
    
    if(!strcmp(exchangeName, "SZ"))
    {
        for (size_t i = 0; i < len; i++)
        {
            m_sz_symbol_set.insert(codeList[i]);
        }
        return;
    }

    if(!strcmp(exchangeName, "CF"))
    {
        for (size_t i = 0; i < len; i++)
        {
            m_cf_symbol_set.insert(codeList[i]);
        }
        return;
    }
};

void TiQuoteIpcClient::run(const char* topic)
{
    if (topic)
    {
        m_config->szQuoteTopic = topic;
        parseTopics();
    }
    for (size_t i = 0; i < m_config->szQuoteCodeVec.size(); i++)
    {
        std::string topic = m_config->szQuoteCodeVec[i];
        SubQuoteReqInfo* reqInfo = NULL;
        uv_work_t* req = NULL;
        //SH
        reqInfo = new SubQuoteReqInfo();

        reqInfo->m_topic = topic;
        reqInfo->m_cb_ptr = quote_cb;
        reqInfo->m_client = this;
        req = new uv_work_t();
        req->data = reqInfo;
        uv_queue_work(m_loop, req, init_sub_quote_work, after_sub_quote_work);
    }
    /*
    SubQuoteReqInfo* reqInfo = NULL;
    uv_work_t* req = NULL;
    //SH
    reqInfo = new SubQuoteReqInfo();

    reqInfo->m_topic = m_config->szQuoteTopic;
    reqInfo->m_cb_ptr = quote_cb;
    reqInfo->m_client = this;
    req = new uv_work_t();
    req->data = reqInfo;
    uv_queue_work(m_loop, req, init_sub_quote_work, after_sub_quote_work);
    return;
    */
};

void TiQuoteIpcClient::stop()
{
    if (m_config)
    {
        ti_ipc_sub_stop(m_config->szQuoteTopic.c_str());
    }
};