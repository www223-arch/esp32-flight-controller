
#include "RCFactory.hpp"
#include "RCAdapter.hpp"
#include "../drivers/rc/rc.h"
#include <string>

extern "C" rc_dev_t* rc_factory_create(const char* name);

std::unique_ptr<RCBase> RCFactory::create(const char* name){
    if(!name) return nullptr;
    rc_dev_t* d = rc_factory_create(name);
    if(!d) return nullptr;
    return std::unique_ptr<RCBase>(new RCAdapter(d));
}
