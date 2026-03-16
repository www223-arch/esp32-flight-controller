// C++ implementation of motor factory. Provides C API wrappers for compatibility.
#include "motor.h"
#include "../../cpp/MotorFactory.hpp"
#include <string>
#include <cstring>
#include <cstdlib>

// Simple PWM motor creator (kept as example driver)
static bool pwm_init(motor_dev_t* m){ (void)m; return true; }
static bool pwm_set_speed(motor_dev_t* m, uint8_t id, float v){ (void)m; (void)id; (void)v; return true; }
static void pwm_deinit(motor_dev_t* m){ (void)m; }

static motor_dev_t* create_pwm(){
    motor_dev_t* m = (motor_dev_t*)malloc(sizeof(*m));
    if(!m) return nullptr;
    memset(m,0,sizeof(*m));
    m->init = pwm_init; m->set_speed = pwm_set_speed; m->deinit = pwm_deinit; m->name = "pwm";
    return m;
}

// Register into C++ MotorFactory at load time
static bool __motor_reg_pwm = (MotorFactory::instance().register_creator("pwm", create_pwm), true);

extern "C" {

motor_dev_t* motor_factory_create(const char* name){
    if(!name) return nullptr;
    motor_dev_t* m = MotorFactory::instance().create(std::string(name));
    return m;
}

void motor_factory_destroy(motor_dev_t* m){
    if(!m) return;
    if(m->deinit) m->deinit(m);
    free(m);
}

} // extern "C"
