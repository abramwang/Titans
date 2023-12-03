#ifndef __REDIS_UV_H__
#define __REDIS_UV_H__
#include <uv.h>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>

typedef struct redisLibuvEvents {
  redisAsyncContext* context;
  uv_poll_t          handle;
  int                events;
} redisLibuvEvents;

void redisLibuvPoll(uv_poll_t* handle, int status, int events);
void redisLibuvAddRead(void *privdata);
void redisLibuvDelRead(void *privdata);
void redisLibuvAddWrite(void *privdata);
void redisLibuvDelWrite(void *privdata);
void on_close(uv_handle_t* handle);
void redisLibuvCleanup(void *privdata);
int redisLibuvAttach(redisAsyncContext* ac, uv_loop_t* loop);

#endif
