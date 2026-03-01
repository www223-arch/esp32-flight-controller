/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "ps2.h"
#include "mytime.h"
#include "Rmt.h"
#include "vofa.h"
#include "myuart.h"

static const char *TAG = "example";
u8 key = 0, X1=0,Y1=0,X2=0,Y2=0; 

void app_main(void)
{

   // led_strip_handle_t led_strip = configure_led();
    PS2_Init();
     vofa_init(&vofa_esp32_hal, VOFA_FORMAT_JUSTFLOAT, 115200);
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

    }
}
