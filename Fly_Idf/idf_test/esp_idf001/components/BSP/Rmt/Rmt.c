// #include <stdio.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/rmt_tx.h"
// #include "driver/gpio.h"
// #include "esp_log.h"
// #include "esp_check.h"

// // ====================== 硬件配置 ======================
// #define TAG             "WS2812_V55_FINAL"
// // WS2812 核心时序（v5.5 copy encoder 需匹配该时序）
// #define RMT_RESOLUTION  10 * 1000 * 1000  // 10MHz 分辨率（必须和WS2812时序匹配）
// #define WS2812_T0H      350     // 0码高电平时间 (ns)
// #define WS2812_T0L      800     // 0码低电平时间 (ns)
// #define WS2812_T1H      700     // 1码高电平时间 (ns)
// #define WS2812_T1L      600     // 1码低电平时间 (ns)
// // 灯带配置（时分复用通道0）
// #define LED_STRIP1_PIN  GPIO_NUM_18
// #define LED_STRIP2_PIN  GPIO_NUM_19
// #define LED_STRIP_LEN   10      // 单条灯带LED数量
// #define LED_BYTES_PER_PIXEL 3   // WS2812：GRB 共3字节（G/R/B）
// #define PIXEL_BUF_LEN (LED_STRIP_LEN * LED_BYTES_PER_PIXEL)  // 单灯带像素缓冲区长度

// // ====================== 全局变量 ======================
// static rmt_channel_handle_t rmt_chan = NULL;  // RMT发送通道（动态重建切换引脚）
// static rmt_encoder_handle_t copy_encoder = NULL; // v5.5 官方推荐的copy encoder
// static uint8_t pixel_buf1[PIXEL_BUF_LEN] = {0}; // 灯带1像素缓冲区（GRB格式）
// static uint8_t pixel_buf2[PIXEL_BUF_LEN] = {0}; // 灯带2像素缓冲区（GRB格式）
// static gpio_num_t current_strip_pin = LED_STRIP1_PIN; // 当前驱动的灯带引脚

// /**
//  * @brief 初始化RMT通道（绑定指定引脚，v5.5 引脚仅能在创建时指定）
//  */
// static esp_err_t rmt_channel_init(gpio_num_t gpio_num)
// {
//     esp_err_t ret = ESP_OK;

//     // 1. 如果通道已存在，先销毁
//     if (rmt_chan) {
//         rmt_disable(rmt_chan);
//         rmt_del_channel(rmt_chan);
//         rmt_chan = NULL;
//     }

//     // 2. 配置RMT发送通道（仅通道0支持DMA）
//     rmt_tx_channel_config_t tx_chan_config = {
//         .gpio_num           = gpio_num,        // 绑定指定引脚
//         .clk_src            = RMT_CLK_SRC_APB, // 稳定时钟源
//         .resolution_hz      = RMT_RESOLUTION,  // 10MHz分辨率（必须和WS2812时序匹配）
//         .mem_block_symbols  = 64,              // 内存块大小（足够容纳WS2812数据即可）
//         .trans_queue_depth  = 2,               // 传输队列深度（非阻塞）
//         .flags.with_dma     = true,            // 启用DMA（仅通道0支持）
//     };
//     ESP_GOTO_ON_ERROR(rmt_new_tx_channel(&tx_chan_config, &rmt_chan), err, TAG, "创建RMT通道失败");

//     // 3. 启用通道
//     ESP_GOTO_ON_ERROR(rmt_enable(rmt_chan), err, TAG, "启用RMT通道失败");

//     return ESP_OK;
// err:
//     if (rmt_chan) {
//         rmt_del_channel(rmt_chan);
//         rmt_chan = NULL;
//     }
//     return ret;
// }

// /**
//  * @brief 填充WS2812像素缓冲区（GRB格式，copy encoder 直接传输该数据）
//  */
// static void ws2812_fill_pixels(uint8_t *pixel_buf, uint8_t hue, uint32_t strip_len)
// {
//     for (int i = 0; i < strip_len; i++) {
//         // WS2812 是 GRB 顺序：第0字节G，第1字节R，第2字节B
//         pixel_buf[i*3 + 0] = (hue + i * 5) % 255;  // G
//         pixel_buf[i*3 + 1] = (hue + i * 10) % 255; // R
//         pixel_buf[i*3 + 2] = (hue + i * 15) % 255; // B
//     }
// }

// /**
//  * @brief RMT DMA传输完成中断回调：切换灯带引脚并发送下一组数据
//  */
// static bool rmt_tx_done_callback(rmt_channel_handle_t channel, const rmt_tx_done_event_data_t *edata, void *user_data)
// {
//     // 1. 切换当前驱动的灯带引脚
//     current_strip_pin = (current_strip_pin == LED_STRIP1_PIN) ? LED_STRIP2_PIN : LED_STRIP1_PIN;
    
//     // 2. 重新初始化RMT通道（绑定新引脚，v5.5 唯一切换引脚的方式）
//     ESP_ERROR_CHECK(rmt_channel_init(current_strip_pin));

//     // 3. 配置传输参数（匹配WS2812时序）
//     rmt_transmit_config_t tx_config = {
//         .loop_count = 0,  // 仅发送一次
//         .flags.eot_level = 0, // 传输结束后引脚拉低（WS2812要求）
//     };
    
//     // 4. 发送对应灯带的数据
//     if (current_strip_pin == LED_STRIP1_PIN) {
//         ESP_ERROR_CHECK(rmt_transmit(rmt_chan, copy_encoder, pixel_buf1, sizeof(pixel_buf1), &tx_config));
//     } else {
//         ESP_ERROR_CHECK(rmt_transmit(rmt_chan, copy_encoder, pixel_buf2, sizeof(pixel_buf2), &tx_config));
//     }

//     return false; // 不需要保留回调
// }

// /**
//  * @brief 初始化RMT + DMA + Copy Encoder（v5.5 官方推荐的WS2812驱动方式）
//  */
// static esp_err_t rmt_dma_init(void)
// {
//     esp_err_t ret = ESP_OK;

//     // 1. 配置GPIO为输出（v5.5 必须手动配置）
//     gpio_config_t io_conf = {
//         .pin_bit_mask = (1ULL << LED_STRIP1_PIN) | (1ULL << LED_STRIP2_PIN),
//         .mode = GPIO_MODE_OUTPUT,
//         .pull_up_en = GPIO_PULLUP_DISABLE,
//         .pull_down_en = GPIO_PULLDOWN_DISABLE,
//         .intr_type = GPIO_INTR_DISABLE,
//     };
//     ESP_GOTO_ON_ERROR(gpio_config(&io_conf), err, TAG, "GPIO配置失败");

//     // 2. 初始化RMT通道（绑定初始引脚LED_STRIP1_PIN）
//     ESP_GOTO_ON_ERROR(rmt_channel_init(LED_STRIP1_PIN), err, TAG, "RMT通道初始化失败");

//     // ========== 关键修改点1：初始化copy encoder配置（v5.5 必须传） ==========
//     rmt_copy_encoder_config_t copy_encoder_config = {}; // 空配置即可（v5.5要求必须传）
//     // ========== 关键修改点2：正确调用rmt_new_copy_encoder（两个参数） ==========
//     ESP_GOTO_ON_ERROR(rmt_new_copy_encoder(&copy_encoder_config, &copy_encoder), err, TAG, "创建Copy Encoder失败");

//     // 4. 注册传输完成中断回调（v5.5 正确API）
//     rmt_tx_event_callbacks_t cbs = {
//         .on_trans_done = rmt_tx_done_callback,
//     };
//     ESP_GOTO_ON_ERROR(rmt_tx_register_event_callbacks(rmt_chan, &cbs, NULL), err, TAG, "注册回调失败");

//     ESP_LOGI(TAG, "RMT + DMA + Copy Encoder 初始化完成（v5.5 官方API）");
//     return ESP_OK;

// err:
//     // 错误清理
//     if (rmt_chan) {
//         rmt_del_channel(rmt_chan);
//         rmt_chan = NULL;
//     }
//     if (copy_encoder) {
//         rmt_del_encoder(copy_encoder);
//         copy_encoder = NULL;
//     }
//     return ret;
// }

// /**
//  * @brief WS2812多灯带演示任务（非阻塞，DMA传输由硬件完成）
//  */
// // static void ws2812_demo_task(void *arg)
// // {
//     // uint8_t hue1 = 0, hue2 = 128; // 两个灯带的色相偏移
//     // rmt_transmit_config_t tx_config = {
//     //     .loop_count = 0,
//     //     .flags.eot_level = 0,
//     // };

//     // // 1. 初始化首次像素数据
//     // ws2812_fill_pixels(pixel_buf1, hue1, LED_STRIP_LEN);
//     // ws2812_fill_pixels(pixel_buf2, hue2, LED_STRIP_LEN);
    
//     // // 2. 启动首次DMA传输（灯带1）
//     // ESP_ERROR_CHECK(rmt_transmit(rmt_chan, copy_encoder, pixel_buf1, sizeof(pixel_buf1), &tx_config));

//     // // 3. 循环更新颜色（非阻塞，DMA传输由硬件完成）
//     // while (1) {
//     //     // 更新灯带1颜色缓冲区
//     //     hue1 = (hue1 + 1) % 255;
//     //     ws2812_fill_pixels(pixel_buf1, hue1, LED_STRIP_LEN);

//     //     // 更新灯带2颜色缓冲区
//     //     hue2 = (hue2 + 2) % 255;
//     //     ws2812_fill_pixels(pixel_buf2, hue2, LED_STRIP_LEN);

//     //     // CPU空闲：可处理其他任务
//     //     vTaskDelay(pdMS_TO_TICKS(50));
//     // }
// // }

// void app_main(void)
// {
//     // // 1. 初始化RMT + DMA + Copy Encoder（v5.5 官方API）
//     // ESP_ERROR_CHECK(rmt_dma_init());

//     // // 2. 创建演示任务（栈大小4096足够）
//     // xTaskCreate(ws2812_demo_task, "ws2812_demo", 4096, NULL, 5, NULL);

//     // ESP_LOGI(TAG, "WS2812 v5.5 驱动启动完成（双灯带时分复用）");
// }












//LEDPWM
// #include <stdio.h>
// #include "driver/gptimer.h"
// #include "driver/ledc.h"
// #include "freertos/FreeRTOS.h"
// void  ws2812_demo_task(void *arg);

// // ====================== 硬件配置 ======================
// void app_main(void)
// {

//     // // 2. 创建演示任务（栈大小4096足够）
//     xTaskCreate(ws2812_demo_task, "ws2812_demo", 4096, NULL, 5, NULL);

// }

// void  ws2812_demo_task(void *arg)
// {
// ledc_timer_config_t ledc_timer = {
//         .speed_mode       = LEDC_LOW_SPEED_MODE,
//         .timer_num        = LEDC_TIMER_0,
//         .duty_resolution  = LEDC_TIMER_13_BIT,
//         .freq_hz          = 5000,  // Frequency in Hertz. Set frequency at 5 kHz
//         .clk_cfg          = LEDC_AUTO_CLK
//     };
//     // Set configuration of timer0 for high speed channels
//     ledc_timer_config(&ledc_timer);

//     ledc_channel_config_t ledc_channel = {
//         .speed_mode     = LEDC_LOW_SPEED_MODE,
//         .channel        = LEDC_CHANNEL_0,
//         .timer_sel      = LEDC_TIMER_0,
//         .intr_type      = LEDC_INTR_DISABLE,
//         .gpio_num       = 16,
//         .duty           = 1000, // Set duty to 50%
//         .hpoint         = 0
//     };
//     ledc_channel_config(&ledc_channel);
// }