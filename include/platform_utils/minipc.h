#ifndef PLATFORM_UTILS_MINIPC_H
#define PLATFORM_UTILS_MINIPC_H
#include <platform_utils/loop.h>
#include <stdint.h>

struct minipc;
typedef void (*minipc_cb)(void *env);
struct minipc_cmd {
	uint32_t cmd;
	void *env;
	minipc_cb cb;
};
struct minipc *minipc_create(struct loop *l, const char *name,
	uint32_t magic, uint32_t ver);
void minipc_destroy(struct minipc *mp);
void minipc_add_cmd(struct minipc *mp, struct minipc_cmd cmd);

#endif
