#ifndef __MYUART_H__
#define __MYUART_H__

#include <stdint.h>
#include <stddef.h>
#include "driver/gpio.h"
#include "soc/rtc.h"  // 获取APB时钟频率
#include "esp_system.h"  // ESP-IDF 5.x 官方微秒延时函数
#include "driver/uart.h"
#include <stdio.h>
#include "vofa.h"

extern vofa_hal_handle_t vofa_esp32_hal;

static void vofa_esp32_uart_init(uint32_t baudrate) ;
static void vofa_esp32_uart_send(const uint8_t* data, size_t len);


#endif // __MYUART_H__