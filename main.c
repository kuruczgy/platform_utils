
extern int platform_main(int argc, char **argv);

#if defined(__ANDROID__)
#include <android_native_app_glue.h>
void android_main(struct android_app *state) {
	platform_main(0, (char*[]){ NULL });
	while (1) {
		if (state->destroyRequested != 0) break;
	}

}
#else
int main(int argc, char **argv) {
	platform_main(argc, argv);
}
#endif
