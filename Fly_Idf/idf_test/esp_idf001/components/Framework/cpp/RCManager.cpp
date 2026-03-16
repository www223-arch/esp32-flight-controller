#include "RCManager.hpp"
#include "RCFactory.hpp"
#include "utils/log.h"
#include <cstring>

RCManager& RCManager::instance(){ static RCManager r; return r; }
RCManager::RCManager(){}
RCManager::~RCManager(){ deinit(); }

bool RCManager::init(const char* driver_name){
    std::lock_guard<std::mutex> lk(mtx_);
    if(dev_) return true;
    dev_ = RCFactory::create(driver_name);
    if(!dev_) return false;
    if(!dev_->init()){ dev_.reset(); return false; }
    FLOGI("RCMgr","initialized %s", driver_name);
    return true;
}

void RCManager::deinit(){ std::lock_guard<std::mutex> lk(mtx_); if(dev_){ dev_->deinit(); dev_.reset(); } }

int RCManager::read_channels(float* out, int max_chans){
    std::lock_guard<std::mutex> lk(mtx_);
    if(!dev_) return 0;
    return dev_->read_channels(out, max_chans);
}
