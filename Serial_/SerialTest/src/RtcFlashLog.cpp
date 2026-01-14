// 1. 包含核心库
#include "RtcFlashLog.h"

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
            uint8_t initHour, uint8_t initMin, uint8_t initSec) {
  // 设置初始时间（首次上电或无备用电池时生效）
  rtc.setTime(initSec, initMin, initHour, initDay, initMonth, initYear);
  
  // 验证RTC是否正常工作（打印当前时间）
  String currentTime = rtc.getTime("%Y-%m-%d %H:%M:%S");
  Serial.print("RTC初始化完成，当前时间：");
  Serial.println(currentTime);
  
  // 提示：若需断电保持时间，需在模组VDD3P3_RTC管脚外接3V纽扣电池（如CR2032）
}

// 4. 生成唯一键名（二选一：时间戳/计数器，适配不同场景）
/**
 * 方案1：生成时间戳键名（格式：log_20240601_103000）
 * 优点：带时间属性，便于追溯日志生成时间
 */
String generateTimestampKey() {
  // 格式化时间戳（年4位+月2位+日2位_时2位+分2位+秒2位）
  return rtc.getTime("log_%Y%m%d_%H%M%S");
}

/**
 * 方案2：生成计数器键名（格式：log_001、log_002...）
 * 优点：无需依赖RTC，适合无备用电池场景
 */
String generateCounterKey() {
  String key = "";
  flashStorage.begin(STORAGE_NAMESPACE, false); // 以读写模式打开命名空间
  
  // 读取当前计数器（默认从1开始）
  int32_t counter = flashStorage.getInt("log_counter", 1); 
  // 生成3位固定长度键名（避免键名长度不一致）
  char keyBuf[20];
  snprintf(keyBuf, sizeof(keyBuf), "log_%03d", counter); 
  key = String(keyBuf);
  
  // 计数器递增（下次使用）
  flashStorage.putInt("log_counter", counter + 1); 
  flashStorage.end(); // 关闭存储，释放资源
  
  return key;
}

// 5. 日志存储到Flash（支持字符串/整数/浮点数据）
/**
 * @brief 存储字符串类型日志（如飞行状态描述）
 * @param logContent 日志内容（如"飞行中：电机转速5000"）
 * @param useTimestampKey true=时间戳键名，false=计数器键名
 */
void saveLogToFlash(String logContent, bool useTimestampKey) {
  flashStorage.begin(STORAGE_NAMESPACE, false); // 读写模式打开存储
  
  // 生成唯一键名
  String key = useTimestampKey ? generateTimestampKey() : generateCounterKey();
  
  // 写入Flash并验证
  if (flashStorage.putString(key.c_str(), logContent)) {
    Serial.printf("? 日志存储成功 | 键名：%s | 内容：%s\n", key.c_str(), logContent.c_str());
  } else {
    Serial.printf("? 日志存储失败 | 键名：%s\n", key.c_str());
  }
  
  flashStorage.end(); // 关闭存储，避免资源泄漏
}

/**
 * @brief 存储整数类型数据（如电机转速、PID参数）
 * @param keySuffix 键名后缀（如"motor1_speed"）
 * @param value 整数数据（如5100）
 */
void saveIntToFlash(String keySuffix, int32_t value) {
  flashStorage.begin(STORAGE_NAMESPACE, false);
  
  // 生成带时间戳的键名（如"motor1_speed_20240601_103000"）
  String key = generateTimestampKey() + "_" + keySuffix;
  flashStorage.putInt(key.c_str(), value);
  
  Serial.printf("? 整数存储成功 | 键名：%s | 数值：%d\n", key.c_str(), value);
  flashStorage.end();
}

/**
 * @brief 存储浮点类型数据（如电池电压、温度）
 * @param keySuffix 键名后缀（如"batt_volt"）
 * @param value 浮点数据（如3.72）
 */
void saveFloatToFlash(String keySuffix, float value) {
  flashStorage.begin(STORAGE_NAMESPACE, false);
  
  String key = generateTimestampKey() + "_" + keySuffix;
  flashStorage.putFloat(key.c_str(), value);
  
  Serial.printf("? 浮点存储成功 | 键名：%s | 数值：%.2f\n", key.c_str(), value);
  flashStorage.end();
}

// 6. 读取Flash中的日志（调试用）
/**
 * @brief 读取指定键名的日志内容
 * @param key 要读取的键名（如"log_20240601_103000"）
 * @return 读取到的日志内容（空字符串表示读取失败）
 */
String readLogFromFlash(String key) {
  flashStorage.begin(STORAGE_NAMESPACE, true); // 只读模式打开存储
  String logContent = flashStorage.getString(key.c_str(), ""); // 读取，默认空字符串
  flashStorage.end();
  
  if (logContent != "") {
    Serial.printf("? 读取日志成功 | 键名：%s | 内容：%s\n", key.c_str(), logContent.c_str());
  } else {
    Serial.printf("? 读取日志失败 | 键名：%s（不存在或损坏）\n", key.c_str());
  }
  return logContent;
}

// 7. 初始化配置（只执行一次）
// void setup() {
//   Serial.begin(115200); // 打开串口（波特率115200，用于查看日志）
//   delay(1000); // 等待串口初始化完成
  
//   // 初始化RTC（首次上电设置初始时间：2024年6月1日 10:00:00）
//   initRTC(2024, 6, 1, 10, 0, 0);
  
//   // 初始化Flash存储（验证命名空间是否正常）
//   if (flashStorage.begin(STORAGE_NAMESPACE, true)) {
//     Serial.println("? Flash存储初始化成功（命名空间：FlightLog）");
//     flashStorage.end();
//   } else {
//     Serial.println("? Flash存储初始化失败，可能是分区损坏");
//     while (1); // 初始化失败时暂停，避免后续错误
//   }
  
//   // 模拟首次日志存储（实际飞控中在飞行循环中调用）
//   saveLogToFlash("飞行启动：电机初始化完成，电池电压3.75V", true); // 时间戳键名
//   saveIntToFlash("motor1_speed", 5100); // 电机1转速（整数）
//   saveFloatToFlash("batt_volt", 3.75f); // 电池电压（浮点）
// }

// // 8. 主循环（模拟飞控实时存储）
// void loop() {
//   // 每5秒存储一次日志（实际飞控根据需要调整频率）
//   static unsigned long lastSaveTime = 0;
//   if (millis() - lastSaveTime >= 5000) {
//     lastSaveTime = millis();
    
//     // 模拟飞行数据（实际需替换为传感器采集值）
//     float randomVolt = 3.65f + random(0, 11) * 0.01f; // 3.65~3.75V随机电压
//     int randomSpeed = 4900 + random(0, 201); // 4900~5100随机转速
    
//     // 存储实时数据
//     saveLogToFlash(String("飞行中：姿态稳定，电压") + randomVolt + "V", false); // 计数器键名
//     saveIntToFlash("motor2_speed", randomSpeed);
//     saveFloatToFlash("batt_volt", randomVolt);
    
//     // 读取最新日志（调试用，可选）
//     String latestKey = generateTimestampKey(); // 获取当前时间戳键名
//     readLogFromFlash(latestKey);
//   }
// }