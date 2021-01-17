#include <platform_utils/main.h>

#if defined(__ANDROID__)
void android_main(struct android_app *app) {
	struct platform plat = { .app = app };
	platform_main(&plat);
}
#else
int main(int argc, char **argv) {
	struct platform plat = { .argc = argc, .argv = argv };
	platform_main(&plat);
}
#endif
