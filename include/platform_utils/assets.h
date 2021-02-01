#ifndef PLATFORM_UTILS_ASSETS_H
#define PLATFORM_UTILS_ASSETS_H

#define pu_assets_declare(sym) \
	extern unsigned char emb_data_##sym[]; \
	extern unsigned long emb_size_##sym;

struct pu_asset {
	unsigned char *data;
	unsigned long size;
};

#define pu_assets_get(sym) (struct pu_asset){ \
	.data = emb_data_##sym, .size = emb_size_##sym };

const char *pu_get_config_dir();

#endif
