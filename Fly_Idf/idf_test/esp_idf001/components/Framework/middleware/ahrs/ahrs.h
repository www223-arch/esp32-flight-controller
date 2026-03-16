#ifndef FRAMEWORK_AHRS_H
#define FRAMEWORK_AHRS_H

#include "../../hal/imu.h"

typedef struct {
    float q[4]; // 四元数
} ahrs_state_t;

// 初始化 AHRS，绑定一个已打开的 imu 设备
bool ahrs_init(ahrs_state_t* st, imu_dev_t* imu);
// 从 imu 样本更新滤波器（非阻塞）
bool ahrs_update(ahrs_state_t* st, const imu_sample_t* sample, float dt);

#endif
