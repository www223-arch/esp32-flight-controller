#ifndef IMU_PUBSUB_H
#define IMU_PUBSUB_H

#include "../hal/imu.h"
#include <stdbool.h>

typedef void (*imu_sub_cb_t)(const imu_sample_t* sample, void* user);

void imu_pubsub_init(void);
void imu_pubsub_deinit(void);

bool imu_pubsub_subscribe(imu_sub_cb_t cb, void* user);
bool imu_pubsub_unsubscribe(imu_sub_cb_t cb, void* user);
bool imu_pubsub_publish(const imu_sample_t* sample);

#endif
