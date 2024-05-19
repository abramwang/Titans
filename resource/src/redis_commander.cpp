#include "redis_commander.h"
#include <iostream>
#include <unistd.h>
#include <glog/logging.h>

#define __ENABLE_REDIS_COMMANDER_LOG__ 0

RedisCommander::RedisCommander(uv_loop_s* loop){
    this->loop = loop;
    auvc = NULL;
    m_command_signal = (uv_async_t*)malloc(sizeof(uv_async_t));
    m_command_signal->data = this;
    uv_async_init(this->loop, m_command_signal, RedisCommander::onCommandAsync);
};

RedisCommander::~RedisCommander(){
    this->loop = NULL;
};

void RedisCommander::parseReply(redisReply *reply, vector<string> &array)
{
    if(!reply){
        return;
    }
    if(reply->len){
        array.push_back(reply->str);
    }
    for (size_t i = 0; i < reply->elements; i++)
    {
        parseReply(reply->element[i], array);
    }
};    
////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void RedisCommander::onConnect(redisAsyncUVContext* c, int status){
    LOG(INFO) << "status: " << status ;
    if (status != REDIS_OK) {
        LOG(ERROR) << "status: " << status ;
        return;
    }
    RedisCommander* commander = (RedisCommander*)c->data;
    commander->auth();
}

void RedisCommander::onDisconnect(redisAsyncUVContext* c, int status){
    LOG(INFO) << "status: " << status ;
    if (status != REDIS_OK) {
        LOG(ERROR) << "status: " << status ;
        return;
    }
    RedisCommander* commander = (RedisCommander*)c->data;
    commander->onDisconnect(status);
}

/// @brief 登陆回调
/// @param c 
/// @param r 
/// @param data 
void RedisCommander::onAuth(redisAsyncUVContext* c, void* r, void* data){
    if(!r){
        return;
    }
    redisReply* reply = (redisReply*)r;
    
    RedisCommander* commander = (RedisCommander*)data;
    commander->onAuth(reply->type, reply->str);
};

/// @brief 创建xgroup 消费组
/// @param c 
/// @param r 
/// @param data 
void RedisCommander::onCreatXGroup(redisAsyncUVContext* c, void* r, void* data){
    if(!r){
        return;
    }
    redisReply* reply = (redisReply*)r;
    
    RedisCommander* commander = (RedisCommander*)data;
    commander->onCreatXGroup(reply->type, reply->str);
};

/// @brief xreadgroup 数据推送回调
/// @param auvc 
/// @param streamKey 
/// @param id 
/// @param type 
/// @param msg 
void RedisCommander::onXreadgroup(redisAsyncUVContext* auvc, const char* streamKey, const char* id, const char* type, const char* msg, void* data){
#if __ENABLE_REDIS_COMMANDER_LOG__
    LOG(INFO) << "streamKey: " << streamKey << "id: " << id << "type: " << type << "msg: " << msg;
#endif
    RedisCommander* commander = (RedisCommander*)auvc->data;
    commander->onXreadgroupMsg(streamKey, id, type, msg);
};

void RedisCommander::onCommandRsp(redisAsyncUVContext* auvc, void* r, void* data)
{
#if __ENABLE_REDIS_COMMANDER_LOG__
    LOG(INFO)<< "RedisCommander::onCommandRsp" << std::endl;
#endif
    if(!r){
        return;
    }
    redisReply* reply = (redisReply*)r;

    RedisCommander* commander = (RedisCommander*)data;

    vector<string> array;
    RedisCommander::parseReply(reply, array);
    commander->onCommand(reply->type, &array);
};


void RedisCommander::onCommandAsync(uv_async_t* handle){
#if __ENABLE_REDIS_COMMANDER_LOG__
    LOG(INFO) << "[" << std::this_thread::get_id() <<  "] RedisCommander::onCommandAsync" << std::endl;
#endif
    RedisCommander* commander = (RedisCommander*)handle->data;

    std::string command;
    if (commander->m_command_queue.try_dequeue(command))
    {
        int flag = redisAsyncUVCommand(commander->auvc, RedisCommander::onCommandRsp, commander, command.c_str());
        LOG(WARNING) << "flag: " << flag << " command: " << command;
    }
};

////////////////////////////////////////////////////////////////////////
// 内部调用方法
////////////////////////////////////////////////////////////////////////

/// @brief 登陆redis
void RedisCommander::auth(){
    if(!this->szPass.empty()){
        char command[256] = {0};
        sprintf(command, "AUTH %s", this->szPass.c_str());
        int flag = redisAsyncUVCommand(this->auvc, RedisCommander::onAuth, this, command);
        LOG(INFO) << "flag: " << flag << "command: " << command;
        return;
    }
};

void RedisCommander::createXGroup(const char* group, const char* streamKey){
    char command[512] = {0};
    sprintf(command, "xgroup create %s %s $ MKSTREAM", streamKey, group);
    int flag = redisAsyncUVCommand(this->auvc, RedisCommander::onCreatXGroup, this, command);
    LOG(INFO) << "flag: " << flag << "command: " << command;
};

////////////////////////////////////////////////////////////////////////
// 外部调用方法
////////////////////////////////////////////////////////////////////////

/// @brief 链接redis服务器
/// @param ip 
/// @param port 
/// @param pass 
/// @return 
int RedisCommander::connect(const char* ip, int port, const char* pass){
    if(this->auvc){
        return -1;
    }
    this->auvc = redisAsyncUVConnent(loop, ip, port, this);
    this->szPass = pass;
    redisAsyncUVSetConnectCallback(this->auvc, RedisCommander::onConnect);
    redisAsyncUVSetDisconnectCallback(this->auvc, RedisCommander::onDisconnect);
    return 0;
};

int RedisCommander::disconnect(){
    if(!this->auvc){
        return -1;
    }
    redisAsyncUVDisconnect(this->auvc);
    return 0;
};

int RedisCommander::command(std::string command){
    if(!this->auvc){
        return -1;
    }
    m_command_queue.enqueue(command);
    return uv_async_send(m_command_signal);
};

void RedisCommander::subStream(const char* group, const char* streamKey, const char* consummerId, int block){
    if(!this->auvc){
        return;
    }
    createXGroup(group, streamKey);
    redisAsyncUVXreadgroupSub(auvc, RedisCommander::onXreadgroup, group, consummerId, streamKey, block, this);
}; 

/// @brief 监听
/// @return 
int RedisCommander::listen(){
    if(!this->auvc){
        return -1;
    }
    return redisAsyncUVRun(this->auvc);
};