#ifndef PLATFORM_UTILS_LOOP_H
#define PLATFORM_UTILS_LOOP_H
#include <poll.h>

struct loop;
typedef void (*loop_cb)(void *env, struct pollfd pfd);
typedef void (*loop_alarm_cb)(void *env);

struct loop *loop_create();
void loop_destroy(struct loop *l);
void loop_add(struct loop *l, int fd, short evs, void *env, loop_cb cb);
void loop_remove(struct loop *l, int fd);
void loop_stop(struct loop *l);
void loop_run(struct loop *l);

#endif
