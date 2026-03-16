#pragma once
#include <functional>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <string>

/*
 * C++ PubSub
 * - 支持多主题（string 或 uint32 id）订阅/发布
 * - 线程安全，内部使用 mutex 保护订阅表
 * - 提供 C 接口桥接，方便 C 驱动调用 `pubsub_c_publish`。
 */
class PubSub {
public:
    using Topic = std::string;
    using Callback = std::function<void(const void* data, size_t len)>;

    // 构造一个带分发队列与任务的 PubSub。queue_len 为内部消息队列长度。
    PubSub(size_t queue_len = 32);
    ~PubSub();

    // 订阅指定主题，返回订阅 id（用于退订）
    int subscribe(const Topic& topic, Callback cb);
    void unsubscribe(const Topic& topic, int sub_id);

    // 发布：会将消息拷贝入内部队列，由分发任务异步转发给回调
    bool publish(const Topic& topic, const void* data, size_t len, uint32_t timeout_ms = 0);

    // register/unregister a PubSub instance for C bridge usage
    static void register_for_c(PubSub* inst);
    static void unregister_for_c();

private:
private:
    struct SubEntry { int id; Callback cb; };
    struct Message { Topic topic; uint8_t* data; size_t len; };

    // non-copyable
    PubSub(const PubSub&) = delete; PubSub& operator=(const PubSub&) = delete;

    // internal dispatcher
    void dispatch_task();

    PubSub();
    ~PubSub();

    std::mutex mtx_;
    std::unordered_map<Topic, std::vector<SubEntry>> subs_;
    int next_id_ = 1;

    // FreeRTOS primitives
    void* queue_handle_ = nullptr; // QueueHandle_t
    void* task_handle_ = nullptr;  // TaskHandle_t
    size_t queue_len_ = 0;

    static PubSub* s_c_target; // for C bridge
};

// C桥接接口，供 C 驱动调用
extern "C" {
    void pubsub_c_publish(const char* topic, const void* data, unsigned short len);
}
