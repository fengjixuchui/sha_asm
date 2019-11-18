#ifndef SHA1_H
#define SHA1_H

struct sha1_ctx {
	size_t data_counter;
	size_t buffer_counter;
	uint32_t state[5];
	uint8_t buffer[64];
};

void sha1_init(struct sha1_ctx *ctx);
void sha1_update(struct sha1_ctx *ctx, uint8_t *data, size_t data_size);
void sha1_final(struct sha1_ctx *ctx, uint8_t *md);

#endif
