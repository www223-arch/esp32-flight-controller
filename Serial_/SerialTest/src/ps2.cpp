#include "ps2.h"
#include "esp_timer.h"

// 构造函数1：使用默认引脚
PS2Controller::PS2Controller()
    : diPin(PS2_DEFAULT_DI_PIN), doPin(PS2_DEFAULT_DO_PIN),
      csPin(PS2_DEFAULT_CS_PIN), clkPin(PS2_DEFAULT_CLK_PIN) {}

// 构造函数2：使用自定义引脚
PS2Controller::PS2Controller(gpio_num_t diPin, gpio_num_t doPin, gpio_num_t csPin, gpio_num_t clkPin)
    : diPin(diPin), doPin(doPin), csPin(csPin), clkPin(clkPin) {}

// 私有静态函数：ESP32微秒级延时（基于esp_timer）
void PS2Controller::delayUs(uint32_t us) {
    uint64_t start = esp_timer_get_time();
    while (esp_timer_get_time() - start < us);
}

// 私有成员函数：初始化ESP32 GPIO引脚
void PS2Controller::initializePins() {
    // 配置输出引脚（DO, CS, CLK）：推挽输出，无上/下拉，禁用中断
    gpio_config_t outConfig = {
        .pin_bit_mask = (1ULL << doPin) | (1ULL << csPin) | (1ULL << clkPin),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&outConfig);

    // 配置输入引脚（DI）：上拉输入，禁用中断
    gpio_config_t inConfig = {
        .pin_bit_mask = (1ULL << diPin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&inConfig);

    // 初始化引脚默认状态
    setDoLevel(true);
    setClkLevel(true);
    setCsLevel(true);
}

// 私有成员函数：读取DI引脚电平
bool PS2Controller::getDiLevel() const {
    return gpio_get_level(diPin) == 1;
}

// 私有成员函数：设置DO引脚电平
void PS2Controller::setDoLevel(bool high) {
    gpio_set_level(doPin, high ? 1 : 0);
}

// 私有成员函数：设置CS引脚电平
void PS2Controller::setCsLevel(bool high) {
    gpio_set_level(csPin, high ? 1 : 0);
}

// 私有成员函数：设置CLK引脚电平
void PS2Controller::setClkLevel(bool high) {
    gpio_set_level(clkPin, high ? 1 : 0);
}

// 私有成员函数：读取PS2手柄单字节数据
uint8_t PS2Controller::readData(uint8_t command) {
    uint8_t result = 0;
    uint8_t bitMask = 1;

    // 逐位发送命令并读取响应（8位数据）
    for (uint8_t i = 0; i < 8; i++) {
        // 发送当前最低位命令
        setDoLevel((command & 0x01) != 0);
        command >>= 1;
        
        // 时钟信号时序控制
        delayUs(10);
        setClkLevel(false);
        delayUs(10);
        
        // 读取当前位数据
        if (getDiLevel()) {
            result |= bitMask;
        }
        bitMask <<= 1;
        
        // 时钟拉高，准备下一位
        setClkLevel(true);
        delayUs(10);
    }
    
    // 恢复数据线默认状态，延时稳定
    setDoLevel(true);
    delayUs(50);
    return result;
}

// 公共成员函数：初始化PS2手柄（对外暴露的初始化接口）
void PS2Controller::init() {
    initializePins();
}

// 公共成员函数：清空数据缓冲区
void PS2Controller::clearData() {
    for (auto& d : data) {
        d = 0x00;
    }
}

// 公共成员函数：获取按下的按键
uint8_t PS2Controller::getPressedKey() {
    clearData();
    
    // 选中PS2设备，开始读取数据
    setCsLevel(false);
    for (uint8_t i = 0; i < 9; i++) {
        data[i] = readData(scan[i]);
    }
   // setCsLevel(true);

    // 组合按键数据（高8位+低8位）
    uint16_t handkey = (data[4] << 8) | data[3];
    
    // 检测按键是否按下，返回对应按键值
    for (uint8_t index = 0; index < 16; index++) {
        if ((handkey & (1 << (mask[index] - 1))) == 0) {
            return index + 1;
        }
    }
    return 0;  // 无按键按下
}

// 公共成员函数：获取摇杆模拟值
uint8_t PS2Controller::getAnalogValue(PS2AnalogStick stick) {
    return data[static_cast<uint8_t>(stick)];
}