#include "RCAdapter.hpp"
#include "../drivers/rc/rc.h"
#include <cstring>

RCAdapter::RCAdapter(rc_dev_t* dev): dev_(dev){}
RCAdapter::~RCAdapter(){ if(dev_){ if(dev_->deinit) dev_->deinit(dev_); /* ownership: do not free here */ } }

const char* RCAdapter::name() const { return dev_ ? dev_->name : ""; }
bool RCAdapter::init(){ if(!dev_) return false; if(dev_->init) return dev_->init(dev_); return true; }
int RCAdapter::read_channels(float* out, int max_chans){ if(!dev_ || !dev_->read_channels) return 0; return dev_->read_channels(dev_, out, max_chans); }
void RCAdapter::deinit(){ if(dev_ && dev_->deinit) dev_->deinit(dev_); }
