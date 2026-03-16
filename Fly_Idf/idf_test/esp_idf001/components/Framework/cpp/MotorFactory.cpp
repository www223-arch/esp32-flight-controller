#include "MotorFactory.hpp"

MotorFactory& MotorFactory::instance(){
    static MotorFactory f; return f;
}

void MotorFactory::register_creator(const std::string& name, Creator c){
    std::lock_guard<std::mutex> lk(mtx_);
    creators_[name] = c;
}

motor_dev_t* MotorFactory::create(const std::string& name){
    std::lock_guard<std::mutex> lk(mtx_);
    auto it = creators_.find(name);
    if(it==creators_.end()) return nullptr;
    return it->second();
}
