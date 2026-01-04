#ifndef USERSPI_H
#define USERSPI_H

#include <Arduino.h>
#include <SPI.h>
// 1. 定义 SPI 默认引脚（ESP32 VSPI 标准引脚）
#define DEFAULT_SPI_SCK  18
#define DEFAULT_SPI_MISO 19
#define DEFAULT_SPI_MOSI 23
#define DEFAULT_SPI_SS   5

class USERSPI {
    private:
    SPIClass* _spi; // 持有 SPI 端口实例（VSPI/HSPI）
    uint8_t _currentSCK;
    uint8_t _currentMISO;
    uint8_t _currentMOSI;
    uint8_t _currentSS;

  public:
    USERSPI(SPIClass* spi = &SPI);
    ~USERSPI();

    // 2. 声明 SPI 初始化函数（带默认参数，支持用户自定义引脚）
void spiInit(uint8_t sckPin = DEFAULT_SPI_SCK,
             uint8_t misoPin = DEFAULT_SPI_MISO,
             uint8_t mosiPin = DEFAULT_SPI_MOSI,
             uint8_t ssPin = DEFAULT_SPI_SS);

// 3. 声明 SPI 数据收发函数（主机模式，单字节收发）
uint8_t spiTransferData(uint8_t ssPin, uint8_t sendData);

};


#endif // USERSPI_H