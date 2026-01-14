#ifndef RTC_FLASH_LOG_H
#define RTC_FLASH_LOG_H
// 1. 包含核心库
#include <Arduino.h>
#include <ESP32Time.h>   // RTC时钟库（处理时间戳生成）
#include <Preferences.h> // Flash存储库（替代ESP-IDF的NVS API）

// 2. 全局对象初始化（避免重复创建）
static ESP32Time rtc;                // RTC时钟对象
static Preferences flashStorage;     // Flash存储对象（操作NVS）
static const char* STORAGE_NAMESPACE = "FlightLog"; // 存储命名空间（避免键名冲突）

// 3. RTC时钟初始化（确保时间戳准确）
/**
 * @brief 初始化RTC时钟，支持断电后外接纽扣电池保持时间
 * @param initYear 初始年份（如2024）
 * @param initMonth 初始月份（1-12）
 * @param initDay 初始日期（1-31）
 * @param initHour 初始小时（0-23）
 * @param initMin 初始分钟（0-59）
 * @param initSec 初始秒（0-59）
 */
void initRTC(uint16_t initYear, uint8_t initMonth, uint8_t initDay, 
            uint8_t initHour, uint8_t initMin, uint8_t initSec);
// 4. 生成唯一键名（二选一：时间戳/计数器，适配不同场景）
/**
 * 方案1：生成时间戳键名（格式：log_20240601_103000）
 * 优点：带时间属性，便于追溯日志生成时间
 */
String generateTimestampKey() ;
/**
 * 方案2：生成计数器键名（格式：log_001、log_002...）
 * 优点：无需依赖RTC，适合无备用电池场景
 */
String generateCounterKey() ;
// 5. 日志存储到Flash（支持字符串/整数/浮点数据）
/**
 * @brief 存储字符串类型日志（如飞行状态描述）
 * @param logContent 日志内容（如"飞行中：电机转速5000"）
 * @param useTimestampKey true=时间戳键名，false=计数器键名
 */
void saveLogToFlash(String logContent, bool useTimestampKey) ;

/**
 * @brief 存储整数类型数据（如电机转速、PID参数）
 * @param keySuffix 键名后缀（如"motor1_speed"）
 * @param value 整数数据（如5100）
 */
void saveIntToFlash(String keySuffix, int32_t value) ;

/**
 * @brief 存储浮点类型数据（如电池电压、温度）
 * @param keySuffix 键名后缀（如"batt_volt"）
 * @param value 浮点数据（如3.72）
 */
void saveFloatToFlash(String keySuffix, float value) ;
// 6. 读取Flash中的日志（调试用）
/**
 * @brief 读取指定键名的日志内容
 * @param key 要读取的键名（如"log_20240601_103000"）
 * @return 读取到的日志内容（空字符串表示读取失败）
 */
String readLogFromFlash(String key) ;

#endif 