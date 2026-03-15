#include "myuart.h"
// Setup UART buffered IO with event queue
// 构建ESP32平台的VOFA HAL句柄
vofa_hal_handle_t vofa_esp32_hal = {
    .uart_init = vofa_esp32_uart_init,
    .uart_send = vofa_esp32_uart_send,
};
const int uart_buffer_size = (1024 * 2);
QueueHandle_t uart_queue;
// ESP32串口配置（可根据硬件修改）
#define VOFA_UART_NUM    UART_NUM_0
#define VOFA_TX_PIN      43// GPIO43
#define VOFA_RX_PIN      44// GPIO44
#define VOFA_BUFFER_SIZE 1024

/**
 * @brief ESP32平台串口初始化实现
 */
static void vofa_esp32_uart_init(uint32_t baudrate) {

    // Install UART driver using an event queue here
(uart_driver_install(VOFA_UART_NUM, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 122,
};
// Configure UART parameters
(uart_param_config(VOFA_UART_NUM, &uart_config));

// Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19, DTR: UNUSED, DSR: UNUSED)
(uart_set_pin(VOFA_UART_NUM, VOFA_TX_PIN, VOFA_RX_PIN, -1, -1));

}

/**
 * @brief ESP32平台串口发送实现
 */
static void vofa_esp32_uart_send(const uint8_t* data, size_t len) {
    if (data == NULL || len == 0) return;
    uart_write_bytes(VOFA_UART_NUM, (const char*)data, len);
}

