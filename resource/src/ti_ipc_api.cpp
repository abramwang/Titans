#include "ti_ipc_api.h"
#include "shadesmar/pubsub/publisher.h"
#include "shadesmar/pubsub/subscriber.h"
#include "shadesmar/pubsub/topic.h"
#include "shadesmar/memory/copier.h"

#include <iostream>
#include <unistd.h>

#include <functional>
#include <string>
#include <unordered_map>

class s_ti_ipc_class
{
public:
  std::string topic_name;
  void (*user_cb_fun)(const char*, void*, size_t);
	shm::pubsub::Publisher*  publisher;
	shm::pubsub::Subscriber* subscriber;
  shm::pubsub::Topic* topic;
  std::function<void(shm::memory::Memblock*)> cb_fun;
  void callback(shm::memory::Memblock *msg){
    if(user_cb_fun){
      user_cb_fun(this->topic_name.c_str(), msg->ptr, msg->size);
    }
  };
} ;

static std::unordered_map<std::string, s_ti_ipc_class*>* g_session_map = nullptr;

int ti_ipc_create(int worker_type_, const char* topic_){
  if(g_session_map == nullptr){
    g_session_map = new std::unordered_map<std::string, s_ti_ipc_class*>();
  }
  s_ti_ipc_class* session = nullptr;
  auto it = g_session_map->find(topic_);
  if (it == g_session_map->end())
  {
    session = new s_ti_ipc_class();
    session->topic_name = topic_;
    session->user_cb_fun = nullptr;
    session->cb_fun = std::bind(&s_ti_ipc_class::callback, session, std::placeholders::_1);
    session->subscriber = nullptr;
    session->publisher = nullptr;
    session->topic = nullptr;
    g_session_map->insert(std::pair<std::string, s_ti_ipc_class*>(topic_, session));
  }else{
    session = it->second;
  }
  if(session == nullptr){
    return -1;
  }
  if(worker_type_ == TI_IPC_SUB){
    if(!session->subscriber){
      session->subscriber = new shm::pubsub::Subscriber(topic_, session->cb_fun);
    }
  }else if(worker_type_ == TI_IPC_PUB){
    if(!session->publisher){
      session->publisher = new shm::pubsub::Publisher(topic_);
    }
  }else if(worker_type_ == TI_IPC_STORE_LIST){
    if(!session->topic){
      session->topic = new shm::pubsub::Topic(topic_);
    }
  }else{
    return -1;
  }
  return 0;
}; 

int ti_ipc_sub_once(const char* topic_, ti_ipc_callback_fun_ptr callback_){
  ti_ipc_create(TI_IPC_SUB, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  it->second->user_cb_fun = callback_;
  if(it->second->subscriber){
    it->second->subscriber->spin_once();
  }
  it->second->user_cb_fun = nullptr;
  return 0;
};

int ti_ipc_sub(const char* topic_, ti_ipc_callback_fun_ptr callback_){
  ti_ipc_create(TI_IPC_SUB, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  it->second->user_cb_fun = callback_;
  if(it->second->subscriber){
    it->second->subscriber->spin();
  }
  it->second->user_cb_fun = nullptr;
  return 0;
};

int ti_ipc_sub_stop(const char* topic_){
  //ti_ipc_create(TI_IPC_SUB, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  if(it->second->subscriber){
    it->second->subscriber->stop();
  }
  return 0;
};

int ti_ipc_pub(const char* topic_, void* data_, size_t data_size_){
  ti_ipc_create(TI_IPC_PUB, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  if(it->second->publisher){
    it->second->publisher->publish(data_, data_size_);
  }
  return 0;
};

void* ti_ipc_get_publisher(const char* topic_)
{
  ti_ipc_create(TI_IPC_PUB, topic_);
  if(g_session_map == nullptr){
    return nullptr;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return nullptr;
  }
  return it->second->publisher;
};

int ti_ipc_pub_by_publisher(void* publisher_, void* data_, size_t data_size_)
{
  if (!publisher_) {
    return -1;
  }
  ((shm::pubsub::Publisher*)publisher_)->publish(data_, data_size_);
  return 0;
};

int ti_ipc_list_append(const char* topic_, void* data_, size_t data_size_){
  ti_ipc_create(TI_IPC_STORE_LIST, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  if(it->second->topic){
    shm::memory::Memblock memblock(data_, data_size_);
    bool status = it->second->topic->write(memblock);
    usleep(15);
    return status;
  }
  return 0;
};

int ti_ipc_list_front(const char* topic_, ti_ipc_callback_fun_ptr callback_){
  ti_ipc_create(TI_IPC_STORE_LIST, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  if(it->second->topic){
    shm::memory::Memblock memblock;
    memblock.free = true;
    std::atomic<uint32_t> index(0);
    if (!it->second->topic->read(&memblock, &index)) {
      return -1;
    }
    it->second->user_cb_fun = callback_;
    it->second->callback(&memblock);
    it->second->user_cb_fun = nullptr;
    if (memblock.free) {
      it->second->topic->copier()->dealloc(memblock.ptr);
      memblock.ptr = nullptr;
      memblock.size = 0;
    }
    return 0;
  }
  return -1;
};

int ti_ipc_list_back(const char* topic_, ti_ipc_callback_fun_ptr callback_){
  ti_ipc_create(TI_IPC_STORE_LIST, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  if(it->second->topic){
    shm::memory::Memblock memblock;
    memblock.free = true;
    std::atomic<uint32_t> index(it->second->topic->counter() - 1);
    if (!it->second->topic->read(&memblock, &index)) {
      return -1;
    }
    it->second->user_cb_fun = callback_;
    it->second->callback(&memblock);
    it->second->user_cb_fun = nullptr;
    if (memblock.free) {
      it->second->topic->copier()->dealloc(memblock.ptr);
      memblock.ptr = nullptr;
      memblock.size = 0;
    }
    return 0;
  }
  return -1;
};

size_t ti_ipc_list_length(const char* topic_){
  ti_ipc_create(TI_IPC_STORE_LIST, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  if(it->second->topic){
    return it->second->topic->counter();
  }
  return -1;
};

int ti_ipc_list_index(const char* topic_, int index_, ti_ipc_callback_fun_ptr callback_){
  ti_ipc_create(TI_IPC_STORE_LIST, topic_);
  if(g_session_map == nullptr){
    return -1;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return -1;
  }
  if(it->second->topic){
    shm::memory::Memblock memblock;
    memblock.free = true;
    std::atomic<uint32_t> index(index_);
    if (!it->second->topic->read(&memblock, &index)) {
      return -1;
    }
    it->second->user_cb_fun = callback_;
    it->second->callback(&memblock);
    it->second->user_cb_fun = nullptr;
    if (memblock.free) {
      it->second->topic->copier()->dealloc(memblock.ptr);
      memblock.ptr = nullptr;
      memblock.size = 0;
    }
    return 0;
  }
  return -1;
};

void ti_ipc_destroy(const char* topic_){
  if(g_session_map == nullptr){
    return;
  }
  auto it = g_session_map->find(topic_);
  if(it == g_session_map->end()){
    return;
  }
  if(it->second->subscriber){
    delete it->second->subscriber;
    it->second->subscriber = nullptr;
  }
  if(it->second->publisher){
    delete it->second->publisher;
    it->second->publisher = nullptr;
  }
  if(it->second->topic){
    delete it->second->topic;
    it->second->topic = nullptr;
  }
  delete it->second;
  g_session_map->erase(it);
};