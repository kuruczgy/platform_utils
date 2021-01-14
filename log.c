#include <platform_utils/log.h>
#include <stdarg.h>

enum level { OUT, INFO };

#if defined(__ANDROID__)
#include <android/log.h>
static void log_impl(enum level level, const char *fmt, va_list args) {
	__android_log_vprint(ANDROID_LOG_INFO, "pu_log", fmt, args);
}
#else
#include <stdio.h>
static void log_impl(enum level level, const char *fmt, va_list args) {
	vfprintf(level == OUT ? stdout : stderr, fmt, args);
}
#endif

void pu_log_out(const char *fmt, ...) {
       va_list args;
       va_start(args, fmt);
       log_impl(OUT, fmt, args);
       va_end(args);
}
void pu_log_info(const char *fmt, ...) {
       va_list args;
       va_start(args, fmt);
       log_impl(INFO, fmt, args);
       va_end(args);
}
