#pragma once
#include "ImuBase.hpp"
#include "../hal/imu.h"

// Adapter: wrap C `imu_dev_t` into C++ `ImuBase`
class ImuAdapter : public ImuBase {
public:
    explicit ImuAdapter(imu_dev_t* dev);
    ~ImuAdapter();
    const char* name() const override;
    bool open() override;
    bool read(imu_sample_t* out) override;
    void close() override;
private:
    imu_dev_t* dev_ = nullptr;
};
