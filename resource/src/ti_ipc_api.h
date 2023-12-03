#ifndef _TI_IPC_API_
#define _TI_IPC_API_

#ifdef WIN32
#define _API_DLLEXPORT_ __declspec( dllexport )  //宏定义
#else
#define _API_DLLEXPORT_  //宏定义
#endif // WIN32

#include <stddef.h>

typedef void (*ti_ipc_callback_fun_ptr)(const char*, void* , size_t);

#define TI_IPC_SUB      1   // 订阅
#define TI_IPC_PUB      2   // 发布
#define TI_IPC_STORE_LIST    3   // 存储

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

_API_DLLEXPORT_ int ti_ipc_sub_once(const char* topic_, ti_ipc_callback_fun_ptr callback_);
_API_DLLEXPORT_ int ti_ipc_sub(const char* topic_, ti_ipc_callback_fun_ptr callback_);
_API_DLLEXPORT_ int ti_ipc_sub_stop(const char* topic_);

_API_DLLEXPORT_ int ti_ipc_pub(const char* topic_, void* data_, size_t data_size_);

_API_DLLEXPORT_ void* ti_ipc_get_publisher(const char* topic_);
_API_DLLEXPORT_ int ti_ipc_pub_by_publisher(void* publisher_, void* data_, size_t data_size_);

_API_DLLEXPORT_ int ti_ipc_list_append(const char* topic_, void* data_, size_t data_size_);
_API_DLLEXPORT_ int ti_ipc_list_front(const char* topic_, ti_ipc_callback_fun_ptr callback_);
_API_DLLEXPORT_ int ti_ipc_list_back(const char* topic_, ti_ipc_callback_fun_ptr callback_);
_API_DLLEXPORT_ size_t ti_ipc_list_length(const char* topic_);
_API_DLLEXPORT_ int ti_ipc_list_index(const char* topic_, int index_, ti_ipc_callback_fun_ptr callback_);

_API_DLLEXPORT_ void ti_ipc_destroy(const char* topic_);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif