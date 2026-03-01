#ifndef __MYTIME_H__
#define __MYTIME_H__
#include "driver/gptimer.h"
#include "esp_log.h"
#include "esp_err.h"

void time_init(void);

// 获取当前微秒数（1?s 精度）
uint64_t time_get_us(void);

uint64_t time_get_ms(void);
// 核心：获取 dt（秒），PID 直接用
float time_get_dt(void);

// 阻塞延时 us
void mydelay_us(uint64_t us);

// 阻塞延时 ms
void mydelay_ms(uint32_t ms);


#endif