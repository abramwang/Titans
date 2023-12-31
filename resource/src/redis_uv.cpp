#include "redis_uv.h"
#include <stdlib.h>
#include <string.h>

void redisLibuvPoll(uv_poll_t* handle, int status, int events) {
  redisLibuvEvents* p = (redisLibuvEvents*)handle->data;

  if (status != 0) {
    return;
  }

  if (events & UV_READABLE) {
    redisAsyncHandleRead(p->context);
  }
  if (events & UV_WRITABLE) {
    redisAsyncHandleWrite(p->context);
  }
}

void redisLibuvAddRead(void *privdata) {
  redisLibuvEvents* p = (redisLibuvEvents*)privdata;

  p->events |= UV_READABLE;

  uv_poll_start(&p->handle, p->events, redisLibuvPoll);
}


void redisLibuvDelRead(void *privdata) {
  redisLibuvEvents* p = (redisLibuvEvents*)privdata;

  p->events &= ~UV_READABLE;

  if (p->events) {
    uv_poll_start(&p->handle, p->events, redisLibuvPoll);
  } else {
    uv_poll_stop(&p->handle);
  }
}


void redisLibuvAddWrite(void *privdata) {
  redisLibuvEvents* p = (redisLibuvEvents*)privdata;

  p->events |= UV_WRITABLE;

  uv_poll_start(&p->handle, p->events, redisLibuvPoll);
}


void redisLibuvDelWrite(void *privdata) {
  redisLibuvEvents* p = (redisLibuvEvents*)privdata;

  p->events &= ~UV_WRITABLE;

  if (p->events) {
    uv_poll_start(&p->handle, p->events, redisLibuvPoll);
  } else {
    uv_poll_stop(&p->handle);
  }
}


void on_close(uv_handle_t* handle) {
  redisLibuvEvents* p = (redisLibuvEvents*)handle->data;

  free(p);
}


void redisLibuvCleanup(void *privdata) {
  redisLibuvEvents* p = (redisLibuvEvents*)privdata;

  uv_close((uv_handle_t*)&p->handle, on_close);
}


int redisLibuvAttach(redisAsyncContext* ac, uv_loop_t* loop) {
  redisContext *c = &(ac->c);

  if (ac->ev.data != NULL) {
    return REDIS_ERR;
  }

  ac->ev.addRead  = redisLibuvAddRead;
  ac->ev.delRead  = redisLibuvDelRead;
  ac->ev.addWrite = redisLibuvAddWrite;
  ac->ev.delWrite = redisLibuvDelWrite;
  ac->ev.cleanup  = redisLibuvCleanup;

  redisLibuvEvents* p = (redisLibuvEvents*)malloc(sizeof(*p));

  if (!p) {
    return REDIS_ERR;
  }

  memset(p, 0, sizeof(*p));

  if (uv_poll_init(loop, &p->handle, c->fd) != 0) {
    return REDIS_ERR;
  }

  ac->ev.data    = p;
  p->handle.data = p;
  p->context     = ac;

  return REDIS_OK;
}