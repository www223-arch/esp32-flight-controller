#include "vofa.h"
//#include "driver/gpio.h"
#include <string.h>
// 全局变量：保存硬件抽象层句柄和协议格式
static vofa_hal_handle_t* g_vofa_hal = NULL;
static vofa_format_t     g_vofa_format = VOFA_FORMAT_JUSTFLOAT;

// JustFloat格式结束符（VOFA协议要求）
static const uint8_t vofa_justfloat_end[4] = {0x00, 0x00, 0x80, 0x7f};


// 新增：预分配固定大小的float数组（VOFA最大通道数建议≤16）
#define VOFA_MAX_CHANNEL 10  // 可根据需求调整（如8/16/32）
static float g_vofa_fixed_buf[VOFA_MAX_CHANNEL]; // 静态固定数组，全局仅一份
/**
 * @brief VOFA模块初始化
 */
void vofa_init(vofa_hal_handle_t* hal_handle, vofa_format_t format, uint32_t baudrate) {
    if (hal_handle == NULL || hal_handle->uart_init == NULL || hal_handle->uart_send == NULL) {
        return; // 入参校验
    }
    g_vofa_hal = hal_handle;
    g_vofa_format = format;
    // 调用硬件层串口初始化函数
    g_vofa_hal->uart_init(baudrate);
}

/**
 * @brief 发送JustFloat格式数据（VOFA协议）
 */
void vofa_send_justfloat(float* data, uint8_t len) {
    if (g_vofa_hal == NULL || g_vofa_hal->uart_send == NULL || data == NULL || len == 0 || len > 8) {
        return; // 入参校验
    }
    // 1. 发送浮点数据
    g_vofa_hal->uart_send((uint8_t*)data, len * sizeof(float));
    // 2. 发送协议结束符
    g_vofa_hal->uart_send(vofa_justfloat_end, 4);
}

/************************ 实现vofa_send_fmt（推荐） ************************/
void vofa_send_fmt(const char* fmt, ...) {
    if (g_vofa_hal == NULL || fmt == NULL || g_vofa_hal->uart_send == NULL) {
        return;
    }

    // 1. 解析格式串，统计变量数量
    uint8_t var_count = 0;
    const char* p = fmt;
    while (*p != '\0') {
        if (*p == '%') {
            p++;
            if (*p == 'd' || *p == 'f' || *p == 'u') {
                var_count++;
            }
        }
        p++;
    }
    if (var_count == 0) return;
    if(var_count > VOFA_MAX_CHANNEL) {
        var_count = VOFA_MAX_CHANNEL; // 超出最大通道数限制，截断处理
    }

    // // 2. 分配内存存储转换后的float数据
    // float* float_data = (float*)malloc(var_count * sizeof(float));
    // if (float_data == NULL) return;

    // 3. 可变参数解析 + 类型转换
    va_list args;
    va_start(args, fmt);
    p = fmt;
    uint8_t idx = 0;
    while (*p != '\0' && idx < var_count) {
        if (*p == '%') {
            p++;
            switch (*p) {
                case 'd': // int类型
                    g_vofa_fixed_buf[idx++] = (float)va_arg(args, int);
                    break;
                case 'f': // float/double类型（double会自动转float）
                    g_vofa_fixed_buf[idx++] = (float)va_arg(args, double);
                    break;
                case 'u': // uint32_t类型
                    g_vofa_fixed_buf[idx++] = (float)va_arg(args, uint32_t);
                    break;
                default:
                    break;
            }
        }
        p++;
    }
    va_end(args);

    // 4. 发送数据 + 结束符
    g_vofa_hal->uart_send((uint8_t*)g_vofa_fixed_buf, idx  * sizeof(float));
    g_vofa_hal->uart_send(vofa_justfloat_end, 4);

    // 5. 释放内存
    // free(float_data);
}
/**
 * @brief 发送RawData格式原始字节
 */
void vofa_send_rawdata(uint8_t* data, size_t len) {
    if (g_vofa_hal == NULL || g_vofa_hal->uart_send == NULL || data == NULL || len == 0) {
        return; // 入参校验
    }
    g_vofa_hal->uart_send(data, len);
}