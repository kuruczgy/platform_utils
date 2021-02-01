#ifndef PLATFORM_UTILS_LOG_H
#define PLATFORM_UTILS_LOG_H

void pu_log_out(const char *fmt, ...);
void pu_log_info(const char *fmt, ...);

#define pu_log_trace(fmt, ...) \
	pu_log_info("[trace] %s:%d: " fmt, __func__, __LINE__, __VA_ARGS__)

#endif
