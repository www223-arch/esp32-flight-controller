#ifndef FRAMEWORK_LOG_H
#define FRAMEWORK_LOG_H

#include <stdarg.h>
#include <stdio.h>

// 框架日志：不依赖 ESP-IDF，默认使用 `printf` 输出，可注册后端以重定向
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*framework_log_backend_t)(const char* level, const char* tag, const char* msg);
void framework_log_set_backend(framework_log_backend_t backend);

// 简洁宏：使用固定 buffer 拼接后交给后端（避免在路径中多次格式化）
void framework_log_printf(const char* level, const char* tag, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#define FLOGI(tag, fmt, ...) framework_log_printf("I", tag, fmt, ##__VA_ARGS__)
#define FLOGW(tag, fmt, ...) framework_log_printf("W", tag, fmt, ##__VA_ARGS__)
#define FLOGE(tag, fmt, ...) framework_log_printf("E", tag, fmt, ##__VA_ARGS__)

#endif // FRAMEWORK_LOG_H
