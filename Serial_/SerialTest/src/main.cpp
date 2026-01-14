#include <WiFi.h>
#include "ps2.h"
#include "RtcFlashLog.h"
// 创建PS2控制器实例（可使用默认引脚或自定义引脚）
PS2Controller ps2;  // 默认引脚
//PS2Controller ps2(GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5); // 自定义引脚
void setup() {
  Serial.begin(115200);
      ps2.init(); 
}
void loop() {
    xTaskCreatePinnedToCore(
        ps2Task,          // 任务函数
        "PS2 Task",       // 任务名称
        4096,             // 堆栈大小
        NULL,             // 任务参数
        1,                // 任务优先级
        NULL,             // 任务句柄
        1                 // 运行在核心1上
    );
        // 获取按下的按键
        uint8_t key = ps2.getPressedKey();
        if (key != 0) {
            printf("Pressed Key: %d\n", key);
        }
        
        // 获取左右摇杆值
        uint8_t lx = ps2.getAnalogValue(PS2AnalogStick::LX);
        uint8_t ly = ps2.getAnalogValue(PS2AnalogStick::LY);
        uint8_t rx = ps2.getAnalogValue(PS2AnalogStick::RX);
        uint8_t ry = ps2.getAnalogValue(PS2AnalogStick::RY);
}

void ps2Task(void *parameter) {
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(100)); // 每100ms读取一次
    }
}
// // ===================== 主函数测试（飞控实际使用时按需调用）=====================
// void app_main(void) {
//     // 1. 初始化NVS（Flash操作基础）
//     esp_err_t err = nvs_flash_init();
//     if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//         nvs_flash_erase();  // 分区满或版本不匹配时擦除
//         nvs_flash_init();
//     }

//     // 2. 初始化RTC时钟（时间戳键名必需）
//     rtc_clock_init();

//     // 3. 模拟飞控数据存储（实际使用时在飞行循环中调用）
//     vTaskDelay(pdMS_TO_TICKS(1000));  // 延时1秒
//     save_log_string("飞行开始：电机启动，电池电压3.72V", true);  // 时间戳键名

//     vTaskDelay(pdMS_TO_TICKS(2000));
//     save_data_int("motor1_speed", 5100);  // 电机1转速（整数）
//     save_data_float("batt_volt", 3.70f);  // 电池电压（浮点）

//     vTaskDelay(pdMS_TO_TICKS(2000));
//     save_log_string("飞行中：姿态稳定，转速5050", false);  // 计数器键名

//     vTaskDelay(pdMS_TO_TICKS(2000));
//     save_data_int("motor2_speed", 4980);  // 电机2转速（整数）
//     save_data_float("temp", 42.5f);       // 温度（浮点）

//     vTaskDelay(pdMS_TO_TICKS(2000));
//     save_log_string("飞行结束：总时长8秒，无故障", true);  // 时间戳键名

//     // 4. 读取示例（可选，调试用）
//     vTaskDelay(pdMS_TO_TICKS(1000));
//     printf("\n===== 读取测试 =====\n");
//     char test_key[TIME_KEY_BUF_SIZE];
//     generate_timestamp_key(test_key, sizeof(test_key));  // 读取最新时间戳日志
//     read_log_example(test_key);
// }