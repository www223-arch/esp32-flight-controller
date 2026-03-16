#ifndef FRAMEWORK_RC_H
#define FRAMEWORK_RC_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rc_dev rc_dev_t;
struct rc_dev {
    const char* name;
    bool (*init)(rc_dev_t* r);
    // 读取通道值到 buffer，返回通道数量
    int  (*read_channels)(rc_dev_t* r, float* chans, int max_chans);
    void (*deinit)(rc_dev_t* r);
    void* priv;
};

rc_dev_t* rc_factory_create(const char* name);
void rc_factory_destroy(rc_dev_t* r);

#ifdef __cplusplus
}
#endif

#endif
