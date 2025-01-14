#include "ti_thread_tool.h"
#include <iostream>
#include <pthread.h>
#include <sched.h>

void TiThreadTool::set_thread_affinity(int core_id) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);         // 清空 CPU 集合
    CPU_SET(core_id, &cpuset); // 将 core_id 加入 CPU 集合

    pthread_t thread = pthread_self();
    if (pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Failed to set thread affinity for core " << core_id << std::endl;
    } else {
        std::cout << "Thread bound to core " << core_id << std::endl;
    }
}