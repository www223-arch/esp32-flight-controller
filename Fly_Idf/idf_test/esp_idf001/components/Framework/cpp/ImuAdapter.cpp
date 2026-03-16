#include "ImuAdapter.hpp"
#include "../hal/imu.h"
#include <cstring>

ImuAdapter::ImuAdapter(imu_dev_t* dev): dev_(dev){}
ImuAdapter::~ImuAdapter(){ if(dev_){ if(dev_->deinit) dev_->deinit(dev_); /* do not free dev_ - keep C factory ownership semantics */ } }

const char* ImuAdapter::name() const { return dev_ ? dev_->name : ""; }
bool ImuAdapter::open(){ if(!dev_) return false; if(dev_->init) return dev_->init(dev_); return true; }
bool ImuAdapter::read(imu_sample_t* out){ if(!dev_ || !dev_->read) return false; return dev_->read(dev_, out); }
void ImuAdapter::close(){ if(dev_ && dev_->deinit) dev_->deinit(dev_); }
