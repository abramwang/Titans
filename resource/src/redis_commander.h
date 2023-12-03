#ifndef __REDIS_COMMANDER_H__
#define __REDIS_COMMANDER_H__

#include "redis_async_uv.h"
#include <string>
#include <vector>
#include "concurrentqueue.h"

using namespace std;

class RedisCommander
{
public:
	RedisCommander(uv_loop_s* loop);
	virtual ~RedisCommander();

protected:
    uv_loop_s* loop;                //event loop
    redisAsyncUVContext* auvc;      //异步链接对象
    moodycamel::ConcurrentQueue<std::string> m_command_queue;    //命令队列
    uv_async_t* m_command_signal;   //命令信号

    std::string szPass;  
private:
    static void parseReply(redisReply *reply, vector<string> &array);           
public:
    static void onConnect(redisAsyncUVContext* c, int status);
    static void onDisconnect(redisAsyncUVContext* c, int status);
    static void onAuth(redisAsyncUVContext* c, void* r, void* data);
    static void onCreatXGroup(redisAsyncUVContext* c, void* r, void* data);
    static void onXreadgroup(redisAsyncUVContext* auvc, const char* streamKey, const char* id, const char* type, const char* msg, void* data);
    
    static void onCommandRsp(redisAsyncUVContext* auvc, void* r, void* data);

    static void onCommandAsync(uv_async_t* handle);
private:        //纯虚函数，必须重载
    virtual void onConnect(int status) = 0;
    virtual void onDisconnect(int status) = 0;
    virtual void onAuth(int err, const char* errStr) = 0;
    virtual void onCommand(int err, std::vector<std::string> *rsp) = 0;
protected:      //可选重载虚函数
    virtual void onCreatXGroup(int err, const char* errStr){};
    virtual void onXreadgroupMsg(const char* streamKey, const char* id, const char* type, const char* msg){};
private:
    void auth();
    void createXGroup(const char* group, const char* streamKey);
   
public:
    int connect(const char* ip, int port, const char* pass);
    int disconnect();
    int command(std::string command);
    void subStream(const char* group, const char* streamKey, const char* consummerId, int block);

    int listen();

};


#endif