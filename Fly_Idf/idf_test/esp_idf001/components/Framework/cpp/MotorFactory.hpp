#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <mutex>
#include "../drivers/motor/motor.h"

/*
 * MotorFactory: 注册表实现
 * - 驱动通过注册函数注册自己的创建函数
 * - 创建时直接查表，不使用 if/else
 */
class MotorFactory {
public:
    using Creator = std::function<motor_dev_t*(void)>;
    static MotorFactory& instance();
    void register_creator(const std::string& name, Creator c);
    motor_dev_t* create(const std::string& name);

private:
    MotorFactory(){}
    std::mutex mtx_;
    std::unordered_map<std::string, Creator> creators_;
};

// helper macro for drivers to register
#define MOTOR_REGISTER(name, func) static bool __motor_reg_##name = (MotorFactory::instance().register_creator(#name, func), true)
