#ifndef PS2_H
#define PS2_H

#include "driver/gpio.h"
#include <cstdint>

// ESP32默认引脚定义（可根据实际接线修改，也可通过构造函数传入自定义引脚）
#define PS2_DEFAULT_DI_PIN     GPIO_NUM_13
#define PS2_DEFAULT_DO_PIN     GPIO_NUM_12
#define PS2_DEFAULT_CS_PIN     GPIO_NUM_19
#define PS2_DEFAULT_CLK_PIN    GPIO_NUM_5

// 按键枚举（替代宏定义，提高类型安全性）
enum class PS2Button : uint8_t {
    SELECT = 1,
    L3,
    R3,
    START,
    PAD_UP,
    PAD_RIGHT,
    PAD_DOWN,
    PAD_LEFT,
    L2,
    R2,
    L1,
    R1,
    GREEN,
    RED,
    BLUE,
    PINK,
    TRIANGLE = GREEN,
    CIRCLE = RED,
    CROSS = BLUE,
    SQUARE = PINK
};

// 摇杆轴枚举
enum class PS2AnalogStick : uint8_t {
    RX = 5,
    RY = 6,
    LX = 7,
    LY = 8
};

// PS2手柄控制器类
class PS2Controller {
public:
    // 构造函数：支持默认引脚和自定义引脚
    PS2Controller();
    PS2Controller(gpio_num_t diPin, gpio_num_t doPin, gpio_num_t csPin, gpio_num_t clkPin);
    
    // 析构函数
    ~PS2Controller() = default;
    
    // 公共接口：供外部调用
    void init();  // 初始化引脚和状态
    uint8_t getPressedKey();  // 获取按下的按键（返回对应PS2Button的数值，无按键返回0）
    uint8_t getAnalogValue(PS2AnalogStick stick);  // 获取摇杆模拟值（0~255）
    void clearData();  // 清空数据缓冲区

private:
    // 私有成员变量：引脚配置（封装，外部不可修改）
    gpio_num_t diPin;
    gpio_num_t doPin;
    gpio_num_t csPin;
    gpio_num_t clkPin;
    
    // 私有成员变量：数据缓冲区和命令集
    uint8_t data[9] = {0};
    uint8_t scan[9] = {0x01, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t mask[16] = {
        static_cast<uint8_t>(PS2Button::SELECT), static_cast<uint8_t>(PS2Button::L3),
        static_cast<uint8_t>(PS2Button::R3), static_cast<uint8_t>(PS2Button::START),
        static_cast<uint8_t>(PS2Button::PAD_UP), static_cast<uint8_t>(PS2Button::PAD_RIGHT),
        static_cast<uint8_t>(PS2Button::PAD_DOWN), static_cast<uint8_t>(PS2Button::PAD_LEFT),
        static_cast<uint8_t>(PS2Button::L2), static_cast<uint8_t>(PS2Button::R2),
        static_cast<uint8_t>(PS2Button::L1), static_cast<uint8_t>(PS2Button::R1),
        static_cast<uint8_t>(PS2Button::GREEN), static_cast<uint8_t>(PS2Button::RED),
        static_cast<uint8_t>(PS2Button::BLUE), static_cast<uint8_t>(PS2Button::PINK)
    };
    
    // 私有静态函数：微秒级延时（ESP32适配，无类实例也可调用）
    static void delayUs(uint32_t us);
    
    // 私有成员函数：引脚操作封装（内部使用，隐藏实现细节）
    void initializePins();
    bool getDiLevel() const;
    void setDoLevel(bool high);
    void setCsLevel(bool high);
    void setClkLevel(bool high);
    
    // 私有成员函数：核心数据读取（内部调用，不暴露给外部）
    uint8_t readData(uint8_t command);
};

#endif // PS2_CONTROLLER_H