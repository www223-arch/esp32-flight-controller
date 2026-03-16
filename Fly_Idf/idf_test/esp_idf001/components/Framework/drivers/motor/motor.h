#ifndef FRAMEWORK_MOTOR_H
#define FRAMEWORK_MOTOR_H

#include <stdint.h>
#include <stdbool.h>

typedef struct motor_dev motor_dev_t;
struct motor_dev {
    const char* name;
    bool (*init)(motor_dev_t* m);
    bool (*set_speed)(motor_dev_t* m, uint8_t motor_id, float value); // value: 0..1
    void (*deinit)(motor_dev_t* m);
    void* priv;
};

// ¹¤³§
motor_dev_t* motor_factory_create(const char* name);
void motor_factory_destroy(motor_dev_t* m);

#endif
