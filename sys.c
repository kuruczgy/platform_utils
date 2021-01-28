#if defined(__EMSCRIPTEN__)
#else
#define _DEFAULT_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#endif

#include <platform_utils/sys.h>

#if PU_SYS_HAS_GETRANDOM
bool pu_getrandom(void *buf, size_t count) {
#if defined(__EMSCRIPTEN__)
	return false;
#else
	long ret = syscall(__NR_getrandom, buf, count, 0);
	return ret == count;
#endif
}
#endif

#if PU_SYS_HAS_CLONE3
long pu_clone3(struct clone_args args) {
	return syscall(__NR_clone3, &args, sizeof(args));
}
#endif
