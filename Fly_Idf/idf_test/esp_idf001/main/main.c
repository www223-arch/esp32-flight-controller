/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"
#include "ps2.h"

// Set to 1 to use DMA for driving the LED strip, 0 otherwise
// Please note the RMT DMA feature is only available on chips e.g. ESP32-S3/P4
#define LED_STRIP_USE_DMA 1

#if LED_STRIP_USE_DMA
// Numbers of the LED in the strip
#define LED_STRIP_LED_COUNT 256
#define LED_STRIP_MEMORY_BLOCK_WORDS 1024 // this determines the DMA block size
#else
// Numbers of the LED in the strip
#define LED_STRIP_LED_COUNT 1
#define LED_STRIP_MEMORY_BLOCK_WORDS 0 // let the driver choose a proper memory block size automatically
#endif                                 // LED_STRIP_USE_DMA

// GPIO assignment
#define LED_STRIP_GPIO_PIN 48

// 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
#define LED_STRIP_RMT_RES_HZ (10 * 1000 * 1000)

static const char *TAG = "example";
    u8 key = 0, X1=0,Y1=0,X2=0,Y2=0; 
led_strip_handle_t configure_led(void)
{
    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_STRIP_GPIO_PIN,                        // The GPIO that connected to the LED strip's data line
        .max_leds = LED_STRIP_LED_COUNT,                             // The number of LEDs in the strip,
        .led_model = LED_MODEL_WS2812,                               // LED strip model
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color order of the strip: GRB
        .flags = {
            .invert_out = false, // don't invert the output signal
        }};

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,                    // different clock source can lead to different power consumption
        .resolution_hz = LED_STRIP_RMT_RES_HZ,             // RMT counter clock frequency
        .mem_block_symbols = LED_STRIP_MEMORY_BLOCK_WORDS, // the memory block size used by the RMT channel
        .flags = {
            .with_dma = LED_STRIP_USE_DMA, // Using DMA can improve performance when driving more LEDs
        }};

    // LED Strip object handle
    led_strip_handle_t led_strip;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");
    return led_strip;
}

void app_main(void)
{

    led_strip_handle_t led_strip = configure_led();
    PS2_Init();
    ESP_LOGI(TAG, "Start blinking LED strip");
    while (1)
    {
         key = PS2_DataKey();
			//获取模拟值
			if(key == PSB_L1 || key == PSB_R1)
			{
				X1 = PS2_AnologData(PSS_LX);
				Y1 = PS2_AnologData(PSS_LY);
				X2 = PS2_AnologData(PSS_RX);
				Y2 = PS2_AnologData(PSS_RY);
            }
              ESP_LOGI(TAG, "PS2 Key:  %d, X1: %d, Y1: %d, X2: %d, Y2: %d", key, X1, Y1, X2, Y2);
                                        vTaskDelay(pdMS_TO_TICKS(1));
                 /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
            // for (int i = 0; i < LED_STRIP_LED_COUNT; i++) {
            //     ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, i, i, 0, 0));
            // }
            // for (int i = 0; i < 255; i++)
            // {
            //     ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, i, 0, 0));
            //     ESP_ERROR_CHECK(led_strip_refresh(led_strip));

            //     vTaskDelay(pdMS_TO_TICKS(5));
            // }
            // ESP_ERROR_CHECK(led_strip_refresh(led_strip));

            // vTaskDelay(pdMS_TO_TICKS(1000));

            // for (int i = 0; i < 255; i++)
            // {
            //     ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 0, i, 0));
            //     ESP_ERROR_CHECK(led_strip_refresh(led_strip));
            //     vTaskDelay(pdMS_TO_TICKS(5));
            // }
            // ESP_ERROR_CHECK(led_strip_refresh(led_strip));

         //  vTaskDelay(pdMS_TO_TICKS(1000));

            // for (int i = 0; i < 255; i++)
            // {
            //     ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 0, 0, i));
            //     ESP_ERROR_CHECK(led_strip_refresh(led_strip));

            //     vTaskDelay(pdMS_TO_TICKS(5));
            // } 
    //         CS_H;
    //                         mydelay_us(22);

    //         CS_L;
    //    //             vTaskDelay(pdMS_TO_TICKS(5));


    //             mydelay_us(4);
    //                CS_H;
    //                                    vTaskDelay(pdMS_TO_TICKS(1));
    //                                              CS_L;
    //    //             vTaskDelay(pdMS_TO_TICKS(5));


    //             mydelay_us(30);



    }
}
