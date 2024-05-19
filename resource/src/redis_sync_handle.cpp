#include "redis_sync_handle.h"
#include <glog/logging.h>
#include <iostream>
using namespace std;

#define __ENABLE_REDIS_SYNC_HANDEL_LOG__ 0

RedisSyncHandle::RedisSyncHandle()
    : _context(nullptr)
{
}

RedisSyncHandle::~RedisSyncHandle()
{
    if (_context != nullptr)
    {
        redisFree(_context);
    }
}
////////////////////////////////////////////////////////////////////////
// 静态工具方法
////////////////////////////////////////////////////////////////////////
void RedisSyncHandle::parseReply(redisReply *reply, vector<string> &array){
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
// 外部方法
////////////////////////////////////////////////////////////////////////

bool RedisSyncHandle::connect(const char* ip, int port)
{
    if (_context){
        return false;
    }
    timeval time_out = {
        15,
        0
    };
    _context = redisConnectWithTimeout(ip, port, time_out);
    if (nullptr == _context)
    {
        LOG(ERROR) << "connect redis failed!" << endl;
        return false;
    }

    cout << "connect redis-server success!" << endl;

    return true;
}

bool RedisSyncHandle::connect(const char* ip, int port, const char* pass)
{
    if (_context){
        return false;
    }
    
    timeval time_out = {
        15,
        0
    };
    _context = redisConnectWithTimeout(ip, port, time_out);
    
    if (nullptr == _context)
    {
        LOG(ERROR) << "connect redis failed!" << endl;
        return false;
    }

    redisReply *reply = 0;
	reply = (redisReply*)redisCommand(_context, "AUTH %s", pass);
	std::cout << "AUTH: " << reply << std::endl;
	freeReplyObject(reply);

    cout << "connect server success!" << endl;

    return true;
}

bool RedisSyncHandle::disconnect()
{
    if (_context)
    {
        redisFree(_context);
        _context = nullptr;
    }
    return true;
};

// 向redis指定的通道channel发布消息
bool RedisSyncHandle::publish(const char* channel, const char* message)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "channel: " << channel << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "PUBLISH %s %s", channel, message);
    if (nullptr == reply)
    {
        //redisReconnect()
        LOG(ERROR) << "publish command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}


bool RedisSyncHandle::del(const char* key)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key ;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "DEL %s", key);
    if (nullptr == reply)
    {
        LOG(ERROR) << "del command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;

};

bool RedisSyncHandle::rpush(const char* key, const char* message)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "RPUSH %s %s", key, message);
    if (nullptr == reply)
    {
        LOG(ERROR) << "rpush command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool RedisSyncHandle::set(const char* key, const char* message)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "SET %s %s", key, message);
    if (nullptr == reply)
    {
        LOG(ERROR) << "set command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool RedisSyncHandle::get(const char* key, string &result)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key ;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "GET %s", key);
    if (nullptr == reply)
    {
        LOG(ERROR) << "get command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    if(reply->str){
        result = reply->str;
    }
    freeReplyObject(reply);
    return true;

};

bool RedisSyncHandle::sadd(const char* key, const char* message)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "SADD %s %s", key, message);
    if (nullptr == reply)
    {
        LOG(ERROR) << "sadd command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool RedisSyncHandle::smembers(const char* key, vector<string> &result)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key ;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "SMEMBERS %s", key);
    if (nullptr == reply)
    {
        LOG(ERROR) << "smembers command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    parseReply(reply, result);
    freeReplyObject(reply);
    return true;
};

bool RedisSyncHandle::hset(const char* key, const char* field, const char* message)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "hset key: " << key << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "HSET %s %s %s", key, field, message);
    if (nullptr == reply)
    {
        LOG(ERROR) << "hset command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
};

bool RedisSyncHandle::hmset(const char* key, const char* field, const char* message)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "hmset key: " << key << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "HMSET %s %s %s", key, field, message);
    if (nullptr == reply)
    {
        LOG(ERROR) << "hmset command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
};


bool RedisSyncHandle::hkeys(const char* key, vector<string> &result)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key ;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "HKEYS %s", key);
    if (nullptr == reply)
    {
        LOG(ERROR) << "hkeys command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    parseReply(reply, result);
    freeReplyObject(reply);
    return true;
};


bool RedisSyncHandle::hget(const char* key, const char* field, string &result)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << " field: " << field << endl;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "HGET %s %s", key, field);
    if (nullptr == reply)
    {
        LOG(ERROR) << "hget command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    if(reply->str){
        result = reply->str;
    }
    freeReplyObject(reply);
    return true;
};

bool RedisSyncHandle::hgetall(const char* key, map<string, string> &result)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key;
#endif
    vector<string> _tmp_array;
    redisReply *reply = (redisReply *)redisCommand(_context, "HGETALL %s", key);
    if (nullptr == reply)
    {
        LOG(ERROR) << "HGETALL command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    parseReply(reply, _tmp_array);
    for (size_t i = 0; i < _tmp_array.size()/2; i++)
    {
        result[_tmp_array[i*2]] = _tmp_array[i*2 + 1];
    }
    freeReplyObject(reply);
    return true;
};

bool RedisSyncHandle::xtrim(const char* key, int count)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << ", message: " << count;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "XTRIM %s MAXLEN %d", key, count);
    if (nullptr == reply)
    {
        LOG(ERROR) << "xtrim command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
};

bool RedisSyncHandle::xadd(const char* key, const char* message)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "XADD %s * msg %s", key, message);
    if (nullptr == reply)
    {
        LOG(ERROR) << "xadd command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool RedisSyncHandle::xadd(const char* key, const char* message, int max_len)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << ", message: " << message;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "XADD %s MAXLEN %d * msg %s", key, max_len, message);
    if (nullptr == reply)
    {
        LOG(ERROR) << "xadd command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
};

bool RedisSyncHandle::xadd_binary(const char* key, const char* message, size_t len)
{
#if __ENABLE_REDIS_SYNC_HANDEL_LOG__
    LOG(INFO) << "key: " << key << ", message len: " << len;
#endif
    redisReply *reply = (redisReply *)redisCommand(_context, "XADD %s * msg %b", key, message, len);
    if (nullptr == reply)
    {
        LOG(ERROR) << "xadd command failed!:" << _context->err << " " << _context->errstr << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}