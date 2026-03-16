#include "SingletonRegistry.hpp"

SingletonRegistry& SingletonRegistry::instance(){
    static SingletonRegistry r; return r;
}

void SingletonRegistry::register_init(std::function<void()> init_fn){
    std::lock_guard<std::mutex> lk(mtx_);
    inits_.push_back(init_fn);
}

void SingletonRegistry::register_deinit(std::function<void()> deinit_fn){
    std::lock_guard<std::mutex> lk(mtx_);
    deinits_.push_back(deinit_fn);
}

void SingletonRegistry::run_inits(){
    std::lock_guard<std::mutex> lk(mtx_);
    for(auto &f: inits_) if(f) f();
}

void SingletonRegistry::run_deinits(){
    std::lock_guard<std::mutex> lk(mtx_);
    for(auto &f: deinits_) if(f) f();
}
