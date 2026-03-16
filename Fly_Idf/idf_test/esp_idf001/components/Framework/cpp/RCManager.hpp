#pragma once
#include <mutex>
#include <memory>

#include "RCBase.hpp"

// RCManager: C++ 单例，封装底层 RCBase 并提供上层接口
class RCManager {
public:
    static RCManager& instance();
    bool init(const char* driver_name);
    void deinit();

    // 读取通道到数组（归一化），返回通道数
    int read_channels(float* out, int max_chans);

private:
    RCManager(); ~RCManager();
    std::unique_ptr<RCBase> dev_;
    std::mutex mtx_;
};
