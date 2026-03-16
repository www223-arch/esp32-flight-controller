#ifndef IMU_BASE_H
#define IMU_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../hal/imu.h"

// ImuBase：面向上层的抽象，所有具体驱动需实现此接口
typedef struct ImuBase ImuBase;
struct ImuBase {
    const char* name;
    bool (*open)(ImuBase* self);
    bool (*close)(ImuBase* self);
    // 非阻塞读取（将样本推送到 Manager），实现可直接填充 sample 并返回 true
    bool (*read)(ImuBase* self, imu_sample_t* sample);
    void* priv;
};

// 工厂接口（返回实现 ImuBase 的实例）
// C factory for creating ImuBase instances (implemented in C drivers/adapters)
ImuBase* ImuFactory_create(const char* name);
void ImuFactory_destroy(ImuBase* dev);

#ifdef __cplusplus
}
#endif

#endif
