#ifndef __REDIS_ASYNC_UV_H__
#define __REDIS_ASYNC_UV_H__

#include "redis_uv.h"
#include "concurrentqueue.h"
#define RAUV_STRLEN 256
#define RAUV_STRLEN_SHORT 16

struct redisAsyncUVContext;

typedef void (*redisAsyncUVConnectCallbackFn)(struct redisAsyncUVContext* auvc, int status);

typedef void (*redisAsyncUVCommandCallbackFn)(struct redisAsyncUVContext* auvc, void* r, void* data);
typedef struct redisAsyncUVCommandInfo
{
    redisAsyncUVCommandCallbackFn callbackFun;
    void* data;
} redisAsyncUVCommandInfo;

typedef void (*redisAsyncUVXreadgroupCallbackFn)(struct redisAsyncUVContext* auvc, const char* streamKey, const char* id, const char* type, const char* msg, void* data);
typedef struct redisAsyncUVXreadgroupInfo
{
    char command[RAUV_STRLEN];
    char group[RAUV_STRLEN_SHORT];
    char consumer[RAUV_STRLEN_SHORT];
    char streamkey[RAUV_STRLEN_SHORT];
    redisAsyncUVXreadgroupCallbackFn callbackFun;
    void* data;
} redisAsyncUVXreadgroupInfo;

typedef struct redisAsyncUVContext
{
    static void onConnect(const redisAsyncContext *c, int status);
    static void onDisconnect(const redisAsyncContext *c, int status);

    static void onAsyncCommand(redisAsyncContext* c, void *r, void *privdata);

    static void onXreadgroupSub(redisAsyncContext *c, void *r, void *privdata);
    
    redisAsyncContext*  ac;
    uv_loop_t*          loop;
    void*               data;
    /* user callback */
    struct {
        redisAsyncUVConnectCallbackFn onConnectFun;
        redisAsyncUVConnectCallbackFn onDisconnectFun;
    } ev;
} redisAsyncUVContext;
 

redisAsyncUVContext* redisAsyncUVConnent(uv_loop_t* loop, const char *ip, int port, void *data);
int redisAsyncUVRun(redisAsyncUVContext* auvc);
bool redisAsyncUVSetConnectCallback(redisAsyncUVContext *auvc, redisAsyncUVConnectCallbackFn fn);
bool redisAsyncUVSetDisconnectCallback(redisAsyncUVContext *auvc, redisAsyncUVConnectCallbackFn fn); 
void redisAsyncUVDisconnect(redisAsyncUVContext *auvc);
void redisAsyncUVFree(redisAsyncUVContext* auvc);


int redisAsyncUVCommand(redisAsyncUVContext* auvc, redisAsyncUVCommandCallbackFn cb, void *data, const char *command);

void redisAsyncUVXreadgroupSub(redisAsyncUVContext* auvc, redisAsyncUVXreadgroupCallbackFn cb, const char* group, const char* consumer, const char* streamkey, const unsigned int block, void* data);


#endif
