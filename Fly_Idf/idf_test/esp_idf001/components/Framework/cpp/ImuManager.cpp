#include "ImuManager.hpp"
#include "ImuFactoryRegistry.hpp"
#include "ImuBase.hpp"
#include "PubSub.hpp"
#include "../pubsub/pubsub_bridge.h"
#include "utils/log.h"
#include <cstring>

ImuManager& ImuManager::instance(){ static ImuManager inst; return inst; }
ImuManager::ImuManager(){}
ImuManager::~ImuManager(){ deinit(); }

bool ImuManager::init(const char* driver_name){
    if(m_driver) return true;
    // use registry to create requested IMU implementation
    m_driver = ImuFactoryRegistry::instance().create(driver_name);
    if(!m_driver) return false;
    if(!m_driver) return false;
    if(!m_driver->open()){ m_driver.reset(); return false; }
    // register C bridge so legacy C drivers can publish to this instance
    pubsub_register_for_c(&m_pubsub);
    FLOGI("ImuMgr","initialized with %s", driver_name);
    return true;
}

void ImuManager::deinit(){ if(m_driver){ m_driver->close(); m_driver.reset(); } pubsub_unregister_for_c(); }

bool ImuManager::poll_once(){
    if(!m_driver) return false;
    imu_sample_t s; memset(&s,0,sizeof(s));
    if(!m_driver->read(&s)) return false;
    m_pubsub.publish("imu/raw", &s, sizeof(s));
    return true;
}

int ImuManager::subscribe(SampleCallback cb){
    if(!cb) return 0;
    auto wrapper = [cb](const void* data, size_t len){ if(data && len==sizeof(imu_sample_t)){ imu_sample_t s; memcpy(&s, data, sizeof(s)); cb(s); } };
    return m_pubsub.subscribe("imu/raw", wrapper);
}

void ImuManager::unsubscribe(int sub_id){ m_pubsub.unsubscribe("imu/raw", sub_id); }
