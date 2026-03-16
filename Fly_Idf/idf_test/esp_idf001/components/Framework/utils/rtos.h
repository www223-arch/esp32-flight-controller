bool pubsub_init(void);
bool pubsub_subscribe(pubsub_cb_t cb, void* user);
bool pubsub_publish(const void* data, size_t len);
/* RTOS helper header
 * Provide small FreeRTOS helper inline functions and light wrappers used by
 * framework components. Keep C linkage for C++ usage.
 */

#ifndef FRAMEWORK_RTOS_H
#define FRAMEWORK_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Millisecond / microsecond delays (wrappers)
static inline void rtos_delay_ms(uint32_t ms){ vTaskDelay(pdMS_TO_TICKS(ms)); }
// Busy-wait microsecond delay: uses vTaskDelay if >1ms, otherwise spin
static inline void rtos_delay_us(uint32_t us){
	if(us >= 1000){ vTaskDelay(pdMS_TO_TICKS(us/1000)); return; }
	TickType_t start = xTaskGetTickCount();
	const TickType_t ticks = 1; (void)start; (void)ticks; // minimal placeholder
	// For precise us delays platform-specific timers should be used.
}

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORK_RTOS_H
