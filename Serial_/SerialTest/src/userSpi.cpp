#include "USERSPI.h"
//#include <USERSPI.h>

USERSPI::USERSPI(SPIClass* spi)

{
  _spi=spi;
  _currentSCK = DEFAULT_SPI_SCK;
  _currentMISO = DEFAULT_SPI_MISO;
  _currentMOSI = DEFAULT_SPI_MOSI;
  _currentSS = DEFAULT_SPI_SS;
  spiInit();
}

/**
 * @brief SPI 初始化函数（支持自定义引脚，默认使用 ESP32 VSPI 引脚）
 * @param sckPin  SCK 时钟引脚
 * @param misoPin MISO 主机输入引脚
 * @param mosiPin MOSI 主机输出引脚
 * @param ssPin   SS 片选引脚
 */
void USERSPI::spiInit(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t ssPin) {

  // 配置 SS 引脚为输出模式（主机模式下，片选由主机主动控制）
  pinMode(ssPin, OUTPUT);
  digitalWrite(ssPin, HIGH); // 初始化为高电平，取消选中从机

  // ESP32 专属：初始化 SPI 并指定自定义引脚（标准 Arduino SPI.begin() 无引脚参数）
  // 格式：SPI.begin(ssPin, sckPin, mosiPin, misoPin)
  _spi->begin(ssPin, sckPin, mosiPin, misoPin);

  // 配置 SPI 通信参数（可根据从机设备修改）
  _spi->beginTransaction(SPISettings(
    1000000,  // 通信时钟频率：1MHz（可调整为 100k~20M）
    MSBFIRST, // 数据传输顺序：高位优先
    SPI_MODE0 // 通信模式：MODE0（CPOL=0, CPHA=0，最常用）
  ));
}

/**
 * @brief SPI 主机模式：单字节收发（同时发送和接收，SPI 全双工特性）
 * @param ssPin    从机片选引脚
 * @param sendData 待发送的字节数据
 * @return 从从机接收的字节数据
 */
uint8_t USERSPI::spiTransferData(uint8_t ssPin, uint8_t sendData) {
  uint8_t recvData;

  // 选中从机（拉低 SS 引脚）
  digitalWrite(ssPin, LOW);
  delayMicroseconds(1); // 短暂延时，确保从机响应

  // SPI 全双工收发：发送一个字节的同时，接收一个字节
  recvData = SPI.transfer(sendData);

  delayMicroseconds(1); // 短暂延时，确保数据传输完成
  digitalWrite(ssPin, HIGH); // 取消选中从机（拉高 SS 引脚）

  return recvData;
}