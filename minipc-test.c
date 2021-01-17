#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <platform_utils/main.h>
#include <platform_utils/minipc.h>

void cmd_1(void *env) {
	fprintf(stderr, "%s called!\n", __func__);
}

void platform_main(struct platform *plat) {
	uint32_t ver = 1;

	struct event_loop *el = event_loop_create(plat);
	struct minipc *mp = minipc_create(el,
			"minipc-demo", 0xFEFE0000, ver);

	minipc_add_cmd(mp, (struct minipc_cmd){
		.cmd = 1, .env = NULL, .cb = cmd_1 });

	event_loop_run(el);
	event_loop_destroy(el);
	minipc_destroy(mp);
}
