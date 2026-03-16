#pragma once
#include <cstdint>

struct imu_sample_t; // forward (from C hal)

class ImuBase {
public:
    virtual ~ImuBase() {}
    virtual const char* name() const = 0;
    virtual bool open() = 0;
    virtual bool read(imu_sample_t* out) = 0;
    virtual void close() = 0;
};
