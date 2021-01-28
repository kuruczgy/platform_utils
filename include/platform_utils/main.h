#ifndef PLATFORM_UTILS_MAIN_H
#define PLATFORM_UTILS_MAIN_H

#if defined(__ANDROID__)
#include <android_native_app_glue.h>
#endif

struct platform {
#if defined(__ANDROID__)
#define PU_MAIN_HAS_ARGS 0
	struct android_app *app;
#else
#define PU_MAIN_HAS_ARGS 1
	int argc;
	char **argv;
#endif
};

void platform_main(struct platform *plat);

#endif

