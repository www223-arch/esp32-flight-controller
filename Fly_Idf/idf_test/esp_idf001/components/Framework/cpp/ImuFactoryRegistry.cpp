#include "ImuFactoryRegistry.hpp"
#include <unordered_map>
#include <mutex>
#include "../drivers/imu/imu_factory.h"
#include "ImuAdapter.hpp"

struct ImuFactoryRegistry::Impl {
    std::mutex mtx;
    std::unordered_map<std::string, ImuCreator> map;
};

ImuFactoryRegistry& ImuFactoryRegistry::instance(){ static ImuFactoryRegistry r; return r; }
ImuFactoryRegistry::ImuFactoryRegistry(){ pimpl_ = new Impl();
    // register built-in factories
    pimpl_->map["bmi088"] = []()->std::unique_ptr<ImuBase>{
        imu_dev_t* d = imu_create_by_name("bmi088"); if(!d) return nullptr; return std::unique_ptr<ImuBase>(new ImuAdapter(d));
    };
    pimpl_->map["mpu6050"] = []()->std::unique_ptr<ImuBase>{
        imu_dev_t* d = imu_create_by_name("mpu6050"); if(!d) return nullptr; return std::unique_ptr<ImuBase>(new ImuAdapter(d));
    };
}
ImuFactoryRegistry::~ImuFactoryRegistry(){ delete pimpl_; }

void ImuFactoryRegistry::register_factory(const std::string& name, ImuCreator c){ std::lock_guard<std::mutex> lk(pimpl_->mtx); pimpl_->map[name]=c; }

std::unique_ptr<ImuBase> ImuFactoryRegistry::create(const std::string& name){ std::lock_guard<std::mutex> lk(pimpl_->mtx); auto it=pimpl_->map.find(name); if(it==pimpl_->map.end()) return nullptr; return it->second(); }
