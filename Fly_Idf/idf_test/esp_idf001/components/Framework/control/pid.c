#include "pid.h"

void pid_init(pid_t* p, float kp, float ki, float kd, float i_limit){
    p->kp = kp; p->ki = ki; p->kd = kd;
    p->integrator = 0.0f; p->last_error = 0.0f; p->i_limit = i_limit;
}

float pid_update(pid_t* p, float setpoint, float measurement, float dt){
    float err = setpoint - measurement;
    p->integrator += err * dt;
    if(p->integrator > p->i_limit) p->integrator = p->i_limit;
    if(p->integrator < -p->i_limit) p->integrator = -p->i_limit;
    float derivative = (err - p->last_error) / (dt > 0 ? dt : 1e-6f);
    p->last_error = err;
    return p->kp * err + p->ki * p->integrator + p->kd * derivative;
}
