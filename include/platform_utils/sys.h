#ifndef PLATFORM_UTILS_SYS_H
#define PLATFORM_UTILS_SYS_H
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define PU_SYS_HAS_GETRANDOM 1
bool pu_getrandom(void *buf, size_t buflen);

#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
#define PU_SYS_HAS_CLONE3 0
#else
#define PU_SYS_HAS_CLONE3 1
_Static_assert(sizeof(uint64_t) == 8, "");
struct clone_args {
	uint64_t flags;
	uint64_t pidfd;
	uint64_t child_tid;
	uint64_t parent_tid;
	uint64_t exit_signal;
	uint64_t stack;
	uint64_t stack_size;
	uint64_t tls;
	uint64_t set_tid;
	uint64_t set_tid_size;
	uint64_t cgroup;
};
long pu_clone3(struct clone_args args);
#endif

#if defined(__ANDROID__) || defined(__EMSCRIPTEN__)
#define PU_SYS_HAS_WORDEXP 0
#else
#define PU_SYS_HAS_WORDEXP 1
#include <wordexp.h>
#endif

#endif
