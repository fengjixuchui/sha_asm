#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "sha1.h"
#include "bits.h"

void sha1_transform_generic(uint32_t *sha1_state, uint32_t *data_block)
{
	uint32_t a, b, c, d, e, f, k, t;
	size_t i;

	a = sha1_state[0];
	b = sha1_state[1];
	c = sha1_state[2];
	d = sha1_state[3];
	e = sha1_state[4];

	uint32_t state[80];

	for (i = 0; i < 80; ++i)
	{
		if (i < 16) {
#ifdef LITTLE_ENDIAN
			state[i] = bswap32(data_block[i]);
#else
			state[i] = data_block[i];
#endif
		} else {
			state[i] = rotl32((state[i - 3] ^ state[i - 8] ^ state[i - 14] ^ state[i - 16]), 1);
		}

		if (i < 20) {
			f = b & c ^ ~b & d;
			k = 0x5a827999;
		} else if (i < 40) {
			f = b ^ c ^ d;
			k = 0x6ed9eba1;
		} else if (i < 60) {
			f = b & c ^ b & d ^ c & d;
			k = 0x8f1bbcdc;
		} else {
			f = b ^ c ^ d;
			k = 0xca62c1d6;
		}

		t = rotl32(a, 5) + f + e + k + state[i];
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = t;
	}

	sha1_state[0] += a;
	sha1_state[1] += b;
	sha1_state[2] += c;
	sha1_state[3] += d;
	sha1_state[4] += e;

	/* avoid leaving sensitive data in memory */
	a = b = c = d = e = f = k = t = 0;
	explicit_bzero(state, 80 * sizeof(uint32_t));
}

void sha1_transform_fast(uint32_t *sha1_state, uint32_t *data_block)
{
	uint32_t a, b, c, d, e, f, t;
	size_t i;

	a = sha1_state[0];
	b = sha1_state[1];
	c = sha1_state[2];
	d = sha1_state[3];
	e = sha1_state[4];

	uint32_t state[80];

	for (i = 0; i < 16; ++i) {
		#ifdef LITTLE_ENDIAN
			state[i] = bswap32(data_block[i]);
		#else
			state[i] = data_block[i];
		#endif

		//f = b & c ^ ~b & d;
		f = b&(c^d)^d;

		t = rotl32(a, 5) + f + e + 0x5a827999 + state[i];
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = t;
	}

	for (; i < 20; ++i) {
		state[i] = rotl32((state[i - 3] ^ state[i - 8] ^ state[i - 14] ^ state[i - 16]), 1);

		f = b&(c^d)^d;

		t = rotl32(a, 5) + f + e + 0x5a827999 + state[i];
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = t;
	}

	for (; i < 40; ++i) {
		state[i] = rotl32((state[i - 3] ^ state[i - 8] ^ state[i - 14] ^ state[i - 16]), 1);

		f = b ^ c ^ d;

		t = rotl32(a, 5) + f + e + 0x6ed9eba1 + state[i];
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = t;
	}

	for (; i < 60; ++i) {
		state[i] = rotl32((state[i - 3] ^ state[i - 8] ^ state[i - 14] ^ state[i - 16]), 1);

		//f = b & c ^ b & d ^ c & d;
		f = (d^b)&c ^ b&d;

		t = rotl32(a, 5) + f + e + 0x8f1bbcdc + state[i];
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = t;
	}

	for (; i < 80; ++i) {
		state[i] = rotl32((state[i - 3] ^ state[i - 8] ^ state[i - 14] ^ state[i - 16]), 1);

		f = b ^ c ^ d;

		t = rotl32(a, 5) + f + e + 0xca62c1d6 + state[i];
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = t;
	}

	sha1_state[0] += a;
	sha1_state[1] += b;
	sha1_state[2] += c;
	sha1_state[3] += d;
	sha1_state[4] += e;

	/* avoid leaving sensitive data in memory */
	//a = b = c = d = e = f = t = 0;
	//explicit_bzero(state, 80 * sizeof(uint32_t));
}

/* assembly implementation of SHA1 transform for amd64 */
void sha1_transform_amd64(uint32_t *sha1_state, uint32_t *data_block);

#ifdef __x86_64__
#define sha1_transform sha1_transform_amd64
#else
#define sha1_transform sha1_transform_generic
#endif

void sha1_init(struct sha1_ctx *ctx)
{
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
	ctx->state[4] = 0xc3d2e1f0;
	ctx->data_counter = 0;
	//ctx->buffer_counter = 0;
}

/*void sha1_update(struct sha1_ctx *ctx, uint8_t *data, size_t data_size)
{
	while (ctx->buffer_counter + data_size >= 64) {
		memcpy(ctx->buffer + ctx->buffer_counter, data, 64 - ctx->buffer_counter);
		data_size -= 64 - ctx->buffer_counter;
		data += 64 - ctx->buffer_counter;
		ctx->buffer_counter = 0;

		sha1_transform(ctx->state, (uint32_t *) ctx->buffer);
		ctx->data_counter += 64;
	}

	if (data_size) {
		memcpy(ctx->buffer + ctx->buffer_counter, data, data_size);
		ctx->buffer_counter += data_size;
	}
}*/

void sha1_update64(struct sha1_ctx *ctx, uint8_t *data)
{
	sha1_transform(ctx->state, (uint32_t *) data);
	ctx->data_counter += 64;
}

void sha1_final(struct sha1_ctx *ctx, uint8_t *md,
	uint8_t *data, size_t data_len)
{
	uint8_t buffer[64];
	size_t i;
	uint64_t bit_length;

	assert(data_len < 64);

	explicit_bzero(buffer, 64);
	memcpy(buffer, data, data_len);
	buffer[data_len] = 0x80;

	if (data_len >= 56) {
		sha1_transform(ctx->state, (uint32_t *) buffer);
		explicit_bzero(buffer, 64);
	}

	ctx->data_counter += data_len;

#ifdef LITTLE_ENDIAN
	bit_length = bswap64((uint64_t) ctx->data_counter * 8);
#else
	bit_length = ctx->data_counter * 8;
#endif

	memcpy(buffer + 56, &bit_length, sizeof(uint64_t));
	sha1_transform(ctx->state, (uint32_t *) buffer);

#ifdef LITTLE_ENDIAN
	for (i = 0; i < 5; ++i) {
		ctx->state[i] = bswap32(ctx->state[i]);
	}
#endif
	memcpy(md, ctx->state, 20);

	/* avoid leaving sensitive data in memory */
	bit_length = 0;
	explicit_bzero(buffer, 64);
	explicit_bzero(ctx, sizeof(struct sha1_ctx));
}

// void sha1_final(struct sha1_ctx *ctx, uint8_t *md)
// {
// 	size_t i;
// 	uint64_t bit_length;

// 	/* if buffer size isn't less then 64 something went south */
// 	assert(ctx->buffer_counter < 64);

// 	explicit_bzero(ctx->buffer + ctx->buffer_counter, 64 - ctx->buffer_counter);
// 	ctx->buffer[ctx->buffer_counter] = 0x80;

// 	if (ctx->buffer_counter >= 56) {
// 		sha1_transform(ctx->state, (uint32_t *) ctx->buffer);
// 		explicit_bzero(ctx->buffer, 64);
// 	}

// 	ctx->data_counter += ctx->buffer_counter;
// 	ctx->buffer_counter = 0;

// #ifdef LITTLE_ENDIAN
// 	bit_length = bswap64((uint64_t) ctx->data_counter * 8);
// #else
// 	bit_length = ctx->data_counter * 8;
// #endif

// 	memcpy(ctx->buffer + 56, &bit_length, sizeof(uint64_t));
// 	sha1_transform(ctx->state, (uint32_t *) ctx->buffer);

// #ifdef LITTLE_ENDIAN
// 	for (i = 0; i < 5; ++i) {
// 		ctx->state[i] = bswap32(ctx->state[i]);
// 	}
// #endif
// 	memcpy(md, ctx->state, 20);

// 	/* avoid leaving sensitive data in memory */
// 	bit_length = 0;
// 	explicit_bzero(ctx, sizeof(struct sha1_ctx));
// }
