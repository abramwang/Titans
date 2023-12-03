#ifndef __TI_THREAD_H__
#define __TI_THREAD_H__
#include <uv.h>

class TiThread
{
private:
    static void thread_func(void* arg);
    uv_thread_t m_thread;
protected:
    uv_loop_t* m_loop;
public:
    TiThread();
    virtual ~TiThread();
    void start();
    void join();
    uv_loop_t* get_loop() { return m_loop; }
};

#endif