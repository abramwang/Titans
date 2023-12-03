#include "ti_thread.h"

TiThread::TiThread()
{
    m_loop = uv_loop_new();
}

TiThread::~TiThread()
{
    uv_loop_delete(m_loop);
}

void TiThread::start()
{
    uv_thread_create(&m_thread, thread_func, this);
}

void TiThread::join()
{
    uv_thread_join(&m_thread);
}

void TiThread::thread_func(void* arg)
{
    TiThread* p = (TiThread*)arg;
    uv_run(p->m_loop, UV_RUN_DEFAULT);
}