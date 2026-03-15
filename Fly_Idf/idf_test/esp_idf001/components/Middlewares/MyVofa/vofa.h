#ifndef __VOFA_H__
#define __VOFA_H__

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/************************ 硬件抽象层（HAL）接口定义 ************************/
// 串口初始化函数类型（由具体平台实现）
typedef void (*vofa_uart_init_t)(uint32_t baudrate);
// 串口发送函数类型（由具体平台实现）
typedef void (*vofa_uart_send_t)(const uint8_t* data, size_t len);

// VOFA硬件抽象层句柄（绑定平台相关的串口操作函数）
typedef struct {
    vofa_uart_init_t  uart_init;  // 串口初始化函数
    vofa_uart_send_t  uart_send;  // 串口发送函数
} vofa_hal_handle_t;

/************************ VOFA协议类型定义 ************************/
typedef enum {
    VOFA_FORMAT_RAWDATA,    // 原始字节格式
    VOFA_FORMAT_JUSTFLOAT   // 浮点型格式（VOFA常用）
} vofa_format_t;

/************************ 全局接口（业务逻辑层） ************************/
/**
 * @brief 初始化VOFA模块
 * @param hal_handle 硬件抽象层句柄（绑定平台串口函数）
 * @param format VOFA数据格式
 * @param baudrate 串口波特率（如115200）
 */
void vofa_init(vofa_hal_handle_t* hal_handle, vofa_format_t format, uint32_t baudrate);

/**
 * @brief 发送JustFloat格式数据（VOFA最常用）
 * @param data 浮点数组（如{temp, humi, speed}）
 * @param len  数组长度（最大支持8个浮点数）
 */
void vofa_send_justfloat(float* data, uint8_t len);

/**
 * @brief 发送RawData格式原始字节
 * @param data 原始字节数据
 * @param len  数据长度
 */
void vofa_send_rawdata(uint8_t* data, size_t len);

void vofa_send_fmt(const char* fmt, ...) ;// 推荐使用，支持格式化字符串发送（如"%f,%f,%f"）

#endif // __VOFA_H__
