#include <platform_utils/log.h>

int platform_main(int argc, char **argv) {
	pu_log_out("Hello world! 1 + 1 = %d\n", 1 + 1);
	return 0;
}
