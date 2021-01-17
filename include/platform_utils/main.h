#ifndef PLATFORM_UTILS_MAIN_H
#define PLATFORM_UTILS_MAIN_H

#if defined(__ANDROID__)
#include <android_native_app_glue.h>
#endif

struct platform {
#if defined(__ANDROID__)
	struct android_app *app;
#else
	int argc;
	char **argv;
#endif
};

void platform_main(struct platform *plat);

#endif

