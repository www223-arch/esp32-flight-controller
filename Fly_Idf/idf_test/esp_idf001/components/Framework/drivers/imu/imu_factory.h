#ifndef IMU_FACTORY_H
#define IMU_FACTORY_H

#include "../../hal/imu.h"

#ifdef __cplusplus
extern "C" {
#endif

// 旧有 C 工厂（保持兼容）
imu_dev_t* imu_create_by_name(const char* name);
void imu_destroy(imu_dev_t* dev);

// 新的 ImuBase 工厂（C++ 风格封装）
// 如果使用 ImuBase，调用 ImuFactory_create/ImuFactory_destroy

struct ImuBase;
struct ImuBase* ImuFactory_create(const char* name);
void ImuFactory_destroy(struct ImuBase* dev);

#ifdef __cplusplus
}
#endif

#endif
