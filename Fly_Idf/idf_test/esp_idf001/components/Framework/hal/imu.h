#ifndef FRAMEWORK_IMU_H
#define FRAMEWORK_IMU_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    float accel[3];
    float gyro[3];
    float mag[3];
    uint64_t timestamp_us;
} imu_sample_t;

typedef struct imu_dev imu_dev_t;
typedef struct imu_dev {
    bool (*init)(imu_dev_t *dev);
    bool (*read)(imu_dev_t *dev, imu_sample_t *out);
    void (*deinit)(imu_dev_t *dev);
    const char *name;
    void *priv;
} imu_dev_t;

/* Factory: create by name ("bmi088", "mpu6050", etc.) */
imu_dev_t* imu_create_by_name(const char* name);
void imu_destroy(imu_dev_t* dev);

#endif // FRAMEWORK_IMU_H
