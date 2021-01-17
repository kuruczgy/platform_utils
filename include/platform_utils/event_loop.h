#ifndef PLATFORM_UTILS_EVENT_LOOP_H
#define PLATFORM_UTILS_EVENT_LOOP_H
#include <platform_utils/main.h>

struct event_loop;

#if !defined(__ANDROID__)
#include <poll.h>
typedef void (*event_loop_cb)(void *env, struct pollfd pfd);
void event_loop_add_fd(struct event_loop *el, int fd,
	short evs, void *env, event_loop_cb cb);
void event_loop_remove_fd(struct event_loop *el, int fd);
#endif

#if defined(__ANDROID__)
void event_loop_set_idle_func(struct event_loop *el,
	void *env, void (*f)(void *env));
#endif

struct event_loop *event_loop_create(struct platform *plat);
void event_loop_destroy(struct event_loop *el);
void event_loop_stop(struct event_loop *el);
void event_loop_run(struct event_loop *el);
struct platform *event_loop_get_platform(struct event_loop *el);

#endif
