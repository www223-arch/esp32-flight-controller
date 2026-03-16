#ifndef FRAMEWORK_LOG_BACKENDS_H
#define FRAMEWORK_LOG_BACKENDS_H

// Provide simple backends registration helpers
void framework_log_use_vofa_backend(void);
void framework_log_use_file_backend(const char* path);
void framework_log_use_udp_backend(const char* ip, uint16_t port);
void framework_log_use_tcp_backend(const char* ip, uint16_t port);

#endif
