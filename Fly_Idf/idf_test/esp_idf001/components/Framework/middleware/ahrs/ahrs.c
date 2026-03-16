#include "ahrs.h"
#include <string.h>

bool ahrs_init(ahrs_state_t* st, imu_dev_t* imu){
    if(!st || !imu) return false;
    // 初始化四元数为单位
    st->q[0]=1.0f; st->q[1]=0.0f; st->q[2]=0.0f; st->q[3]=0.0f;
    (void)imu;
    return true;
}

bool ahrs_update(ahrs_state_t* st, const imu_sample_t* sample, float dt){
    if(!st || !sample) return false;
    // placeholder: 真实实现应使用 Madgwick/Mahony/EKF
    (void)dt; (void)sample;
    return true;
}
