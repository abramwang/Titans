#include <cstdlib>
#include <cstring>
#include <vector>
#include "redis_async_uv.h"

///////////////////////////////////////////
// tool fun
///////////////////////////////////////////
void redisAsyncUvToolParseReplay(int level, redisReply *reply, std::vector<char*> &msgList){
    if(reply->str){
        //printf("parseReplay %d) [%d] %s [%d] %lld\n", level, reply->type, reply->str, reply->len, reply->integer);
        msgList.push_back(reply->str);
    }
    for (size_t i = 0; i < reply->elements; i++)
    {
        redisAsyncUvToolParseReplay(level + 1, reply->element[i], msgList);
    }
}


///////////////////////////////////////////
// redisAsyncUVContext
///////////////////////////////////////////

void redisAsyncUVContext::onConnect(const redisAsyncContext* c, int status){
    redisAsyncUVContext* auvc = (redisAsyncUVContext*)c->data;
    if (auvc->ev.onConnectFun){
        auvc->ev.onConnectFun(auvc, status);
    }
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
};

void redisAsyncUVContext::onDisconnect(const redisAsyncContext* c, int status){
    redisAsyncUVContext* auvc = (redisAsyncUVContext*)c->data;
    if (auvc->ev.onDisconnectFun){
        auvc->ev.onDisconnectFun(auvc, status);
    }
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
};

void redisAsyncUVContext::onAsyncCommand(redisAsyncContext* c, void *r, void *privdata){
    redisAsyncUVContext* auvc = (redisAsyncUVContext*)c->data;
    redisAsyncUVCommandInfo* info = (redisAsyncUVCommandInfo*)privdata;
    if(info->callbackFun){
        info->callbackFun(auvc, r, info->data);
    }
    free(info);
};


void onXACK(redisAsyncContext *c, void *r, void *privdata){
    redisAsyncUVXreadgroupInfo* info = (redisAsyncUVXreadgroupInfo*)privdata;
    redisAsyncCommand(c, redisAsyncUVContext::onXreadgroupSub, info, info->command);
}

void redisAsyncUVContext::onXreadgroupSub(redisAsyncContext *c, void *r, void *privdata){
    redisAsyncUVXreadgroupInfo* info = (redisAsyncUVXreadgroupInfo*)privdata;
    
    redisReply *reply = (redisReply *)r;
    if (reply == NULL) return;

    std::vector<char*> msgList;
    redisAsyncUvToolParseReplay(0, reply, msgList);
    /*
    printf("-----------------parse over-----------------\n");
    for (size_t i = 0; i < msgList.size(); i++)
    {
        printf("%ld) %s\n", i, msgList[i]);
    }
    printf("-----------------print over-----------------\n");
    */
    if(msgList.size() >= 4){
        if(info->callbackFun){
            info->callbackFun((redisAsyncUVContext*)c->data, msgList[0], msgList[1], msgList[2], msgList[3], info->data);
        }

        char ackCommand[128];
        memset(ackCommand, 0, 128);
        sprintf(ackCommand, "XACK %s %s %s", msgList[0], info->group, msgList[1]);
        //printf("%s\n", ackCommand);
        redisAsyncCommand(c, onXACK, info, ackCommand);
    }else{
        redisAsyncCommand(c, redisAsyncUVContext::onXreadgroupSub, info, info->command);
    }
    return;
};


///////////////////////////////////////////
// redisAsyncUV
///////////////////////////////////////////
/// @brief 创建异步链接
/// @param loop uv_eventloop
/// @param ip   ip
/// @param port port
/// @return redisAsyncUVContext object
redisAsyncUVContext* redisAsyncUVConnent(uv_loop_t* loop, const char *ip, int port, void * data){
    redisAsyncUVContext* auvc = (redisAsyncUVContext*)malloc(sizeof(redisAsyncUVContext));
    memset(auvc, 0, sizeof(redisAsyncUVContext));

    redisAsyncContext *c = redisAsyncConnect(ip, port);
    c->data = auvc;
    auvc->ac = c;
    auvc->loop = loop;
    auvc->data = data;
    if (c->err) {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        redisAsyncFree(c);
        free(auvc);
        return NULL;
    }
    
    redisLibuvAttach(c, loop);
    redisAsyncSetConnectCallback(c, redisAsyncUVContext::onConnect);
    redisAsyncSetDisconnectCallback(c, redisAsyncUVContext::onDisconnect);
    return auvc;
};

int redisAsyncUVRun(redisAsyncUVContext* auvc){
    return uv_run(auvc->loop, UV_RUN_DEFAULT);
};

bool redisAsyncUVSetConnectCallback(redisAsyncUVContext *auvc, redisAsyncUVConnectCallbackFn fn){
    if(! auvc->ev.onConnectFun){
        auvc->ev.onConnectFun = fn;
        return true;
    }
    return false;
};

bool redisAsyncUVSetDisconnectCallback(redisAsyncUVContext *auvc, redisAsyncUVConnectCallbackFn fn){
    if(! auvc->ev.onDisconnectFun){
        auvc->ev.onDisconnectFun = fn;
        return true;
    }
    return false;
};


void redisAsyncUVDisconnect(redisAsyncUVContext *auvc){

};

void redisAsyncUVFree(redisAsyncUVContext* auvc){

};

int redisAsyncUVCommand(redisAsyncUVContext* auvc ,redisAsyncUVCommandCallbackFn cb, void *data, const char *command){
    redisAsyncUVCommandInfo* info = (redisAsyncUVCommandInfo*)malloc(sizeof(redisAsyncUVCommandInfo));
    info->callbackFun = cb;
    info->data = data;

    return redisAsyncCommand(auvc->ac, redisAsyncUVContext::onAsyncCommand, info, command);
};

void redisAsyncUVXreadgroupSub(redisAsyncUVContext* auvc, redisAsyncUVXreadgroupCallbackFn cb, const char* group,  const char* consumer, const char* streamkey, const unsigned int block, void* data){
    redisAsyncUVXreadgroupInfo* info = (redisAsyncUVXreadgroupInfo*)malloc(sizeof(redisAsyncUVXreadgroupInfo));
    memset(info, 0, sizeof(redisAsyncUVXreadgroupInfo));
    sprintf(info->command, "xreadgroup GROUP %s %s COUNT 1 BLOCK %d STREAMS %s >", group, consumer, block, streamkey);
    sprintf(info->group, "%s", group);
    sprintf(info->consumer, "%s", consumer);
    sprintf(info->streamkey, "%s", streamkey);

    info->callbackFun = cb;
    info->data = data;

    redisAsyncCommand(auvc->ac, redisAsyncUVContext::onXreadgroupSub, info, info->command);
};
