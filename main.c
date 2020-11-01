#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <minipc.h>

void cmd_1(void *env) {
	fprintf(stderr, "%s called!\n", __func__);
}

int main() {
	char magic[4] = "hh\0\0";
	uint32_t ver = 1;

	struct loop *l = loop_create();
	struct minipc *mp = minipc_create(l,
			"minipc-demo", ntohl(*(uint32_t*)magic), ver);

	minipc_add_cmd(mp, (struct minipc_cmd){
		.cmd = 1, .env = NULL, .cb = cmd_1 });

	loop_run(l);
	loop_destroy(l);
	minipc_destroy(mp);
}