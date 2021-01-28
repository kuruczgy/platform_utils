#if defined(__EMSCRIPTEN__)
#include <emscripten/html5.h>
#else
#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <sys/timerfd.h>
#endif

#include <platform_utils/event_loop.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ds/vec.h>

struct loop_entry {
	void *env;
	event_loop_cb cb;
};

struct event_loop {
	struct platform *plat;
	bool stop;
#if defined(__ANDROID__)
	struct {
		void *env;
		void (*f)(void *env);
	} idle;
#endif
	struct vec pollfds; /* vec<struct pollfd> */
	struct vec entries; /* vec<struct loop_entry> */
};

struct event_loop *event_loop_create(struct platform *plat) {
	struct event_loop *el = malloc(sizeof(struct event_loop));
	if (!el) return NULL;

	*el = (struct event_loop){ .plat = plat };

	el->stop = false;

#if defined(__EMSCRIPTEN__)
	// nothing
#else
	el->pollfds = vec_new_empty(sizeof(struct pollfd));
	el->entries = vec_new_empty(sizeof(struct loop_entry));
#endif
	return el;
}
void event_loop_destroy(struct event_loop *el) {
#if defined(__EMSCRIPTEN__)
	// nothing
#else
	vec_free(&el->pollfds);
	vec_free(&el->entries);
#endif
	free(el);
}
void event_loop_stop(struct event_loop *el) {
	el->stop = true;
}
void event_loop_run(struct event_loop *el) {
#if defined(__ANDROID__)
	int ident;
	int events;
	int fd;
	void *data;
	while (true) {
		while ((ident = ALooper_pollAll(el->idle.f ? 0 : -1,
				&fd, &events, &data)) >= 0) {
			if (ident == LOOPER_ID_MAIN
					|| ident == LOOPER_ID_INPUT) {
				struct android_poll_source *source = data;
				source->process(el->plat->app, source);
			} else if (ident == LOOPER_ID_USER) {
				for (int i = 0; i < el->pollfds.len; ++i) {
					struct pollfd *pfd =
						vec_get(&el->pollfds, i);
					if (pfd->fd == fd) {
						pfd->revents = POLLIN;
						struct loop_entry *e =
						vec_get(&el->entries, i);
						e->cb(e->env, *pfd);
						break;
					}
				}
			}
			if (el->stop || el->plat->app->destroyRequested) {
				goto stop;
			}
		}
		if (el->idle.f) el->idle.f(el->idle.env);
	}
stop:
	;
#elif defined(__EMSCRIPTEN__)
	/* this is kind of a hack, this function actually never returns */
	emscripten_unwind_to_js_event_loop();
#else
	while (!el->stop && poll(el->pollfds.d, el->pollfds.len, -1) != -1) {
		for (int i = 0; i < el->pollfds.len; ++i) {
			struct pollfd *pfd = vec_get(&el->pollfds, i);
			if (pfd->revents != 0) {
				struct loop_entry *e = vec_get(&el->entries, i);
				e->cb(e->env, *pfd);
			}
		}
	}
#endif
}

#if defined(__ANDROID__)
void event_loop_set_idle_func(struct event_loop *el,
		void *env, void (*f)(void *env)) {
	el->idle.env = env;
	el->idle.f = f;
}
#endif

void event_loop_add_fd(struct event_loop *el, int fd,
		short evs, void *env, event_loop_cb cb) {
	struct pollfd pfd = { .fd = fd, .events = evs };
	vec_append(&el->pollfds, &pfd);

	struct loop_entry entry = { .env = env, .cb = cb };
	vec_append(&el->entries, &entry);

#if defined(__ANDROID__)
	ALooper_addFd(el->plat->app->looper, fd, LOOPER_ID_USER,
		ALOOPER_EVENT_INPUT, NULL, NULL);
#endif
}
void event_loop_remove_fd(struct event_loop *el, int fd) {
	for (int i = 0; i < el->pollfds.len; ++i) {
		struct pollfd *pfd = vec_get(&el->pollfds, i);
		if (pfd->fd == fd) {
			vec_remove(&el->pollfds, i);
			vec_remove(&el->entries, i);
			break;
		}
	}

#if defined(__ANDROID__)
	ALooper_removeFd(el->plat->app->looper, fd);
#endif
}

struct platform *event_loop_get_platform(struct event_loop *el) {
	return el->plat;
}

#if defined(__EMSCRIPTEN__)
static void alarm_cb(void *env) {
	struct event_loop_timer *t = env;
	t->id = -1;
	t->cb(t->env);
}
#else
static void alarm_cb(void *env, struct pollfd pfd) {
	struct event_loop_timer *t = env;
	if (pfd.revents & POLLIN) {
		uint64_t exp;
		read(t->fd, &exp, sizeof(exp));
		t->cb(t->env);
	}
}
#endif
void event_loop_timer_init(struct event_loop_timer *t, struct event_loop *el,
		void *env, event_loop_timer_cb cb) {
	*t = (struct event_loop_timer){
		.el = el,
		.env = env,
		.cb = cb
	};
#if defined(__EMSCRIPTEN__)
	t->id = -1;
#else
	t->fd = timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK | TFD_CLOEXEC);
	event_loop_add_fd(el, t->fd, POLLIN, t, alarm_cb);
#endif
}
void event_loop_timer_finish(struct event_loop_timer *t) {
#if defined(__EMSCRIPTEN__)
	if (t->id != -1) {
		emscripten_clear_timeout(t->id);
	}
#else
	event_loop_remove_fd(t->el, t->fd);
	close(t->fd);
#endif
}
void event_loop_timer_set_abs(struct event_loop_timer *t, struct timespec ts) {
#if defined(__EMSCRIPTEN__)
	if (t->id != -1) {
		emscripten_clear_timeout(t->id);
	}
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	double msecs = 0.0;
	msecs += (ts.tv_sec - now.tv_sec) * 1e3;
	msecs += (ts.tv_nsec - now.tv_nsec) / 1e6;
	emscripten_set_timeout(alarm_cb, msecs, t);
#else
	struct itimerspec spec = { .it_value = ts };
	timerfd_settime(t->fd, TFD_TIMER_ABSTIME, &spec, NULL);
#endif
}
