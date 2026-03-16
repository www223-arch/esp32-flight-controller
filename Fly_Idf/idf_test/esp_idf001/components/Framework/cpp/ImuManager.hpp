#pragma once

#include <memory>
#include <functional>
#include <string>

extern "C" {
#include "../hal/imu.h" // provides imu_sample_t
}

#include "ImuBase.hpp"

class ImuManager {
public:
    using SampleCallback = std::function<void(const imu_sample_t&)>;

    static ImuManager& instance();

    ImuManager(const ImuManager&) = delete;
    ImuManager& operator=(const ImuManager&) = delete;

    bool init(const char* driver_name);
    void deinit();

    int subscribe(SampleCallback cb);
    void unsubscribe(int sub_id);

    bool poll_once();

private:
    ImuManager(); ~ImuManager();
    std::unique_ptr<ImuBase> m_driver;
    PubSub m_pubsub{32};
};
