#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <minipc.h>
#include <ds/vec.h>

struct minipc {
	int sfd;
	struct loop *l;
	struct vec cmds;
	uint32_t magic, ver;
};

static int make_addr(const char *name, struct sockaddr_un *addr) {
	const char *dir = getenv("XDG_RUNTIME_DIR");
	if (!dir) {
		return -1;
	}

	int ml = sizeof(addr->sun_path);
	if (snprintf(addr->sun_path, ml, "%s/%s.sock", dir, name) >= ml) {
		return -1;
	}
	addr->sun_family = AF_UNIX;
	return 0;
}

static void client_cb(void *env, struct pollfd pfd) {
	struct minipc *mp = env;
	if (pfd.revents & POLLIN) {
		int av_len;
		if (ioctl(pfd.fd, FIONREAD, &av_len) == -1) {
			close(pfd.fd);
			return;
		}
		if (av_len >= 3 * sizeof(uint32_t)) {
			uint32_t buf[3];
			int len = sizeof(uint32_t) * 3;
			if (recv(pfd.fd, buf, len, 0) != len) {
				close(pfd.fd);
				return;
			}
			uint32_t magic = ntohl(buf[0]);
			uint32_t ver = ntohl(buf[1]);
			uint32_t c = ntohl(buf[2]);
			if (magic != mp->magic) {
				fprintf(stderr, "client[%d]: bad magic\n", pfd.fd);
				close(pfd.fd);
				return;
			}
			if (ver != mp->ver) {
				fprintf(stderr, "client[%d]: bad version\n", pfd.fd);
				close(pfd.fd);
				return;
			}
			for (int i = 0; i < mp->cmds.len; ++i) {
				struct minipc_cmd *cmd = vec_get(&mp->cmds, i);
				if (cmd->cmd == c) {
					cmd->cb(cmd->env);
					break;
				}
			}
		}
	}
	if (pfd.revents & (POLLHUP | POLLNVAL)) {
		loop_remove(mp->l, pfd.fd);
		fprintf(stderr, "disconnect[%d]\n", pfd.fd);
		return;
	}
}
static void accept_cb(void *env, struct pollfd pfd) {
	struct minipc *mp = env;
	if (pfd.revents & (POLLHUP | POLLNVAL)) {
		loop_remove(mp->l, pfd.fd);
		return;
	}
	if (pfd.revents & POLLIN) {
		int fd = accept(pfd.fd, NULL, NULL);
		if (fd == -1) {
			return;
		}
		loop_add(mp->l, fd, POLLIN, mp, client_cb);
		fprintf(stderr, "accept[%d]\n", fd);
	}
}

struct minipc *minipc_create(struct loop *l, const char *name, uint32_t magic,
		uint32_t ver) {
	struct minipc *mp = malloc(sizeof(struct minipc));
	mp->l = l;
	mp->magic = magic;
	mp->ver = ver;

	struct sockaddr_un addr = { 0 };
	if (make_addr(name, &addr) == -1) {
		free(mp);
		return NULL;
	}

	mp-> sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (mp->sfd == -1) {
		free(mp);
		return NULL;
	}

	unlink(addr.sun_path);
	if (bind(mp->sfd,
			(const struct sockaddr *)&addr,
			sizeof(struct sockaddr_un)) == -1) {
		free(mp);
		return NULL;
	}

	if (listen(mp->sfd, 8) == -1) {
		free(mp);
		return NULL;
	}

	loop_add(mp->l, mp->sfd, POLLIN, mp, accept_cb);

	mp->cmds = vec_new_empty(sizeof(struct minipc_cmd));
	return mp;
}
void minipc_destroy(struct minipc *mp) {
	loop_remove(mp->l, mp->sfd);
	close(mp->sfd);
	vec_free(&mp->cmds);
	free(mp);
}

void minipc_add_cmd(struct minipc *mp, struct minipc_cmd cmd) {
	vec_append(&mp->cmds, &cmd);
}
