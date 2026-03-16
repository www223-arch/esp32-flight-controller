bool pubsub_publish_from_isr(uint32_t topic, const void* data, uint16_t len);
bool pubsub_init(void);
bool pubsub_subscribe(pubsub_cb_t cb, void* user);
bool pubsub_publish(const void* data, size_t len);
/* Framework logging implementation
 * Provides `framework_log_set_backend` and `framework_log_printf`.
 * This file was accidentally overwritten; restore a minimal, thread-safe
 * implementation that delegates to a backend or `printf` by default.
 */

#include "log.h"
#include <stdarg.h>
#include <string.h>

static framework_log_backend_t s_backend = NULL;

void framework_log_set_backend(framework_log_backend_t backend){
	s_backend = backend;
}

void framework_log_printf(const char* level, const char* tag, const char* fmt, ...){
	char buf[256];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if(s_backend){
		s_backend(level, tag, buf);
	} else {
		// fallback to printf
		printf("%s/%s: %s\n", level, tag, buf);
	}
}
