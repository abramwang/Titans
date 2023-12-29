#ifndef REDIS_SYNC_HANDLE_H
#define REDIS_SYNC_HANDLE_H

#include <hiredis/hiredis.h>
#include <map>
#include <vector>
#include <string>
using namespace std;

class RedisSyncHandle
{
private:
    // hiredis同步上下文对象，负责publish消息
    redisContext *_context;
    
public:
    static void parseReply(redisReply *reply, vector<string> &array);
public:
    RedisSyncHandle();
    ~RedisSyncHandle();


    // 连接redis服务器 
    bool connect(const char* ip, int port);
    bool connect(const char* ip, int port, const char* pass);
    bool disconnect();
    

    // 向redis指定的通道channel发布消息
    bool publish(const char* channel, const char* message);
    
    // 删除键值
    bool del(const char* key);

    // LIST
    bool rpush(const char* key, const char* message);

    // SET
    bool set(const char* key, const char* message);
    bool get(const char* key, string &result);
    bool sadd(const char* key, const char* message);
    bool smembers(const char* key, vector<string> &result);

    // HASH
    bool hset(const char* key, const char* field, const char* message);
    bool hmset(const char* key, const char* field, const char* message);
    bool hkeys(const char* key, vector<string> &result);
    bool hget(const char* key, const char* field, string &result);
    bool hgetall(const char* key, map<string, string> &result);

    /// STREAM
    bool xtrim(const char* key, int count);
    bool xadd(const char* key, const char* message);

    bool xadd_binary(const char* key, const char* message, size_t len);


};

#endif
