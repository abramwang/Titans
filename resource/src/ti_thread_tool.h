#ifndef __TI_THREAD_TOOL_H__
#define __TI_THREAD_TOOL_H__

class TiThreadTool
{
public:
    static void set_thread_affinity(int core_id);
};

#endif