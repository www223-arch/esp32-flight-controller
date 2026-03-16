#ifndef FRAMEWORK_PID_H
#define FRAMEWORK_PID_H

typedef struct {
    float kp, ki, kd;
    float integrator;
    float last_error;
    float i_limit;
} pid_t;

void pid_init(pid_t* p, float kp, float ki, float kd, float i_limit);
float pid_update(pid_t* p, float setpoint, float measurement, float dt);

#endif
