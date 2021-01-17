#include <platform_utils/event_loop.h>

#include <stdbool.h>
#include <stdlib.h>
#if defined(__ANDROID__)
// nothing
#else
#include <ds/vec.h>
#endif

#if defined(__EMSCRIPTEN__)
#include <emscripten/html5.h>
#endif

struct event_loop {
	struct platform *plat;
	bool stop;
#if defined(__ANDROID__)
	struct {
		void *env;
		void (*f)(void *env);
	} idle;
#else
	struct loop_entry {
		void *env;
		event_loop_cb cb;
	};
	struct vec pollfds; /* vec<struct pollfd> */
	struct vec entries; /* vec<struct loop_entry> */
#endif
};

struct event_loop *event_loop_create(struct platform *plat) {
	struct event_loop *el = malloc(sizeof(struct event_loop));
	if (!el) return NULL;

	el->plat = plat;

#if defined(__ANDROID__)
	// nothing
#else
	el->pollfds = vec_new_empty(sizeof(struct pollfd));
	el->entries = vec_new_empty(sizeof(struct loop_entry));
	el->stop = false;
#endif
	return el;
}
void event_loop_destroy(struct event_loop *el) {
#if defined(__ANDROID__)
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
	void *data;
	int timeout = el->idle.f ? 0 : -1;
	while (true) {
		while ((ident = ALooper_pollAll(
				timeout, NULL, &events, &data)) >= 0) {
			if (ident == LOOPER_ID_MAIN
					|| ident == LOOPER_ID_INPUT) {
				struct android_poll_source *source = data;
				source->process(el->plat->app, source);
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
#else
void event_loop_add_fd(struct event_loop *el, int fd,
		short evs, void *env, event_loop_cb cb) {
	struct pollfd pfd = { .fd = fd, .events = evs };
	vec_append(&el->pollfds, &pfd);

	struct loop_entry entry = { .env = env, .cb = cb };
	vec_append(&el->entries, &entry);
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
}
#endif

struct platform *event_loop_get_platform(struct event_loop *el) {
	return el->plat;
}
