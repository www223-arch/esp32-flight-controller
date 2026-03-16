#pragma once
#include <memory>
#include <string>
#include "ImuBase.hpp"

// Template IMU factory: different specializations can create different drivers.
template<typename T>
struct ImuFactory {
    static std::unique_ptr<ImuBase> create();
};

// Specializations examples
template<>
struct ImuFactory<int> {
    static std::unique_ptr<ImuBase> create(); // BMI088
};

template<>
struct ImuFactory<long> {
    static std::unique_ptr<ImuBase> create(); // MPU6050
};
