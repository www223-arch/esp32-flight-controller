#include "ImuFactory.hpp"
#include "ImuAdapter.hpp"
#include "../hal/imu.h"

extern "C" imu_dev_t* imu_create_by_name(const char* name);

// Specialization for BMI088
std::unique_ptr<ImuBase> ImuFactory<int>::create(){
    imu_dev_t* d = imu_create_by_name("bmi088");
    if(!d) return nullptr;
    return std::unique_ptr<ImuBase>(new ImuAdapter(d));
}

// Specialization for MPU6050
std::unique_ptr<ImuBase> ImuFactory<long>::create(){
    imu_dev_t* d = imu_create_by_name("mpu6050");
    if(!d) return nullptr;
    return std::unique_ptr<ImuBase>(new ImuAdapter(d));
}

