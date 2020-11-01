#ifndef _MINIPC_H_
#define _MINIPC_H_
#include <sys/poll.h>
#include <stdint.h>

struct loop;
typedef void (*loop_cb)(void *env, struct pollfd pfd);

struct loop *loop_create();
void loop_destroy(struct loop *l);
void loop_add(struct loop *l, int fd, short evs, void *env, loop_cb cb);
void loop_remove(struct loop *l, int fd);
void loop_run(struct loop *l);

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
