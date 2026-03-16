#include "../../hal/imu.h"
#include <stdlib.h>
#include <string.h>

// 简单的两个驱动示例（stub），演示工厂如何返回不同实现
static bool bmi088_init(imu_dev_t* dev){ dev->priv = NULL; return true; }
static bool bmi088_read(imu_dev_t* dev, imu_sample_t* out){
    memset(out,0,sizeof(*out));
    out->timestamp_us = 0;
    return true;
}
static void bmi088_deinit(imu_dev_t* dev){ }

static bool mpu6050_init(imu_dev_t* dev){ dev->priv = NULL; return true; }
static bool mpu6050_read(imu_dev_t* dev, imu_sample_t* out){
    memset(out,0,sizeof(*out));
    out->timestamp_us = 0;
    return true;
}
static void mpu6050_deinit(imu_dev_t* dev){ }

// 创建并返回指定名称的 imu_dev_t 实例（简单堆分配）
imu_dev_t* imu_create_by_name(const char* name){
    if(!name) return NULL;
    imu_dev_t* d = (imu_dev_t*)malloc(sizeof(imu_dev_t));
    if(!d) return NULL;
    memset(d,0,sizeof(*d));
    if(strcmp(name,"bmi088")==0){
        d->init = bmi088_init;
        d->read = bmi088_read;
        d->deinit = bmi088_deinit;
        d->name = "bmi088";
    }else if(strcmp(name,"mpu6050")==0){
        d->init = mpu6050_init;
        d->read = mpu6050_read;
        d->deinit = mpu6050_deinit;
        d->name = "mpu6050";
    }else{
        free(d);
        return NULL;
    }
    return d;
}

void imu_destroy(imu_dev_t* dev){
    if(!dev) return;
    if(dev->deinit) dev->deinit(dev);
    free(dev);
}

// 新增：ImuBase 适配层（为 C++ 上层提供统一 ImuBase 接口）
#include "../hal/imu_base.h"

static bool imu_open_adapter(ImuBase* self){
    if(!self || !self->priv) return false;
    imu_dev_t* d = (imu_dev_t*)self->priv;
    return d->init(d);
}
static bool imu_read_adapter(ImuBase* self, imu_sample_t* sample){
    if(!self || !self->priv) return false;
    imu_dev_t* d = (imu_dev_t*)self->priv;
    return d->read(d, sample);
}
static bool imu_close_adapter(ImuBase* self){
    if(!self || !self->priv) return false;
    imu_dev_t* d = (imu_dev_t*)self->priv;
    if(d->deinit) d->deinit(d);
    return true;
}

ImuBase* ImuFactory_create(const char* name){
    imu_dev_t* d = imu_create_by_name(name);
    if(!d) return NULL;
    ImuBase* ib = (ImuBase*)malloc(sizeof(ImuBase));
    if(!ib){ imu_destroy(d); return NULL; }
    memset(ib,0,sizeof(*ib));
    ib->name = d->name;
    ib->open = imu_open_adapter;
    ib->read = imu_read_adapter;
    ib->close = imu_close_adapter;
    ib->priv = d;
    return ib;
}

void ImuFactory_destroy(ImuBase* dev){
    if(!dev) return;
    if(dev->priv){ imu_dev_t* d = (imu_dev_t*)dev->priv; imu_destroy(d); }
    free(dev);
}
