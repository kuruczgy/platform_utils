#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <platform_utils/assets.h>
#include <platform_utils/sys.h>

const char *pu_get_config_dir() {
#if defined(__EMSCRIPTEN__)
	return NULL;
#elif defined(__ANDROID__)
	const char *str = "/sdcard/smuc";
	int len = strlen(str);
	char *res = malloc(len + 1);
	memcpy(res, str, len + 1);
	return res;
#else
#if PU_SYS_HAS_WORDEXP
	const char *in = "$HOME/.config/smuc";
	if (getenv("XDG_CONFIG_HOME")) {
		in = "$XDG_CONFIG_HOME/smuc";
	}
	char *res = NULL;
	wordexp_t p;
	if (wordexp(in, &p, WRDE_NOCMD) == 0) {
		const char *str = p.we_wordv[0];
		int len = strlen(str);
		res = malloc(len + 1);
		memcpy(res, str, len + 1);
		wordfree(&p);
	}
	return res;
#else
	return NULL;
#endif
#endif
}
