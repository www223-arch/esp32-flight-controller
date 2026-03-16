#include "PubSub.hpp"
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

PubSub* PubSub::s_c_target = nullptr;

// internal task trampoline
static void pubsub_task_trampoline(void* arg){
    PubSub* ps = reinterpret_cast<PubSub*>(arg);
    if(ps) ps->dispatch_task();
    vTaskDelete(NULL);
}

PubSub::PubSub(size_t queue_len): queue_len_(queue_len){
    queue_handle_ = xQueueCreateStatic ? nullptr : nullptr;
    // create dynamic queue using FreeRTOS API
    queue_handle_ = (void*)xQueueCreate((UBaseType_t)queue_len_, sizeof(void*));
    // create dispatcher task
    xTaskCreate(pubsub_task_trampoline, "PubSubDisp", 4096/sizeof(StackType_t), this, tskIDLE_PRIORITY+1, (TaskHandle_t*)&task_handle_);
}

PubSub::~PubSub(){
    // stop task by sending nullptr and then cleanup
    if(queue_handle_){
        void* msg = nullptr;
        xQueueSend((QueueHandle_t)queue_handle_, &msg, 0);
    }
    // no reliable way to join FreeRTOS task here; let it exit on nullptr
    // free remaining queued messages
}

int PubSub::subscribe(const Topic& topic, Callback cb){
    std::lock_guard<std::mutex> lk(mtx_);
    int id = next_id_++;
    subs_[topic].push_back(SubEntry{.id = id, .cb = cb});
    return id;
}

void PubSub::unsubscribe(const Topic& topic, int sub_id){
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = subs_.find(topic);
    if(it==subs_.end()) return;
    auto &vec = it->second;
    vec.erase(std::remove_if(vec.begin(), vec.end(), [&](const SubEntry& e){ return e.id==sub_id; }), vec.end());
}

bool PubSub::publish(const Topic& topic, const void* data, size_t len, uint32_t timeout_ms){
    if(!queue_handle_) return false;
    Message* m = new Message();
    m->topic = topic;
    m->len = len;
    m->data = nullptr;
    if(len>0){ m->data = (uint8_t*)malloc(len); if(m->data) memcpy(m->data, data, len); }
    BaseType_t res = xQueueSend((QueueHandle_t)queue_handle_, &m, pdMS_TO_TICKS(timeout_ms));
    if(res==pdFALSE){ // queue full or timeout
        if(m->data) free(m->data); delete m; return false;
    }
    return true;
}

void PubSub::dispatch_task(){
    if(!queue_handle_) return;
    for(;;){
        Message* m = nullptr;
        if(xQueueReceive((QueueHandle_t)queue_handle_, &m, portMAX_DELAY) != pdTRUE) continue;
        if(m==nullptr) break; // shutdown signal

        std::vector<SubEntry> cbs;
        {
            std::lock_guard<std::mutex> lk(mtx_);
            auto it = subs_.find(m->topic);
            if(it!=subs_.end()) cbs = it->second;
        }
        for(auto &e: cbs){ if(e.cb) e.cb(m->data, m->len); }

        if(m->data) free(m->data);
        delete m;
    }
}

void PubSub::register_for_c(PubSub* inst){ s_c_target = inst; }
void PubSub::unregister_for_c(){ s_c_target = nullptr; }

extern "C" void pubsub_c_publish(const char* topic, const void* data, unsigned short len){
    if(!topic) return;
    if(!PubSub::s_c_target) return;
    PubSub::s_c_target->publish(std::string(topic), data, len);
}

extern "C" void pubsub_register_for_c(void* inst){ PubSub::register_for_c(reinterpret_cast<PubSub*>(inst)); }
extern "C" void pubsub_unregister_for_c(void){ PubSub::unregister_for_c(); }
