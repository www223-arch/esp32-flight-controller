#pragma once
#include <functional>
#include <vector>
#include <mutex>

// 全局单例注册器：用于管理项目中创建的单例（初始化/反初始化顺序控制）
class SingletonRegistry {
public:
    static SingletonRegistry& instance();
    // 注册初始化函数（在系统启动时调用）
    void register_init(std::function<void()> init_fn);
    void register_deinit(std::function<void()> deinit_fn);
    void run_inits();
    void run_deinits();
private:
    SingletonRegistry(){}
    std::mutex mtx_;
    std::vector<std::function<void()>> inits_;
    std::vector<std::function<void()>> deinits_;
};
