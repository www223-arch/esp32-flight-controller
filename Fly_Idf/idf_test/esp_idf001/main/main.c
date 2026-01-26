#include <stdio.h>
#include "driver/gptimer.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
void  ws2812_demo_task(void *arg);

// ====================== 硬件配置 ======================
void app_main(void)
{

    // // 2. 创建演示任务（栈大小4096足够）
    xTaskCreate(ws2812_demo_task, "ws2812_demo", 4096, NULL, 5, NULL);

}

void  ws2812_demo_task(void *arg)
{
ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_13_BIT,
        .freq_hz          = 5000,  // Frequency in Hertz. Set frequency at 5 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = 16,
        .duty           = 4096, // Set duty to 50%
        .hpoint         = 0
    };
        ledc_channel_config(&ledc_channel);
        ledc_channel.channel = LEDC_CHANNEL_1;
        ledc_channel.gpio_num       = 17;
        ledc_channel_config(&ledc_channel);

        ledc_channel.channel = LEDC_CHANNEL_2;
        ledc_channel.gpio_num   = 18;
        ledc_channel_config(&ledc_channel);
   
        ledc_channel.channel = LEDC_CHANNEL_3;
        ledc_channel.gpio_num   = 16;
        ledc_channel_config(&ledc_channel);

        ledc_channel.channel = LEDC_CHANNEL_4;
        ledc_channel.gpio_num   = 4;
        ledc_channel_config(&ledc_channel);

        ledc_channel.channel = LEDC_CHANNEL_5;
        ledc_channel.gpio_num   = 5;
        ledc_channel_config(&ledc_channel);

        ledc_channel.channel = LEDC_CHANNEL_6;
        ledc_channel.gpio_num   = 6;
        ledc_channel.duty= 4096*2/5; // Set duty to 50%

        ledc_channel_config(&ledc_channel);

        ledc_channel.channel = LEDC_CHANNEL_7;
        ledc_channel.gpio_num   = 7;
        ledc_channel.duty= 4096*2/3; // Set duty to 50%

        ledc_channel_config(&ledc_channel);

        ledc_channel.channel = LEDC_CHANNEL_0;
        ledc_channel.gpio_num   = 15;
        ledc_channel.duty           = 4096*2/4; // Set duty to 50%
        ledc_channel_config(&ledc_channel);

        while(1) {
         vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}