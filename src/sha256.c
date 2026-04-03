#include "sha256.h"
#include "util.h"
#include <string.h>

// SHA-256 round constants
static const uint32_t k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

typedef struct {
	uint8_t buffer[64];
	uint32_t h[8];
	size_t buffer_len;
	uint64_t total_len;
} sha256_ctx;

// Rotate Right
[[gnu::const, gnu::always_inline]] static inline uint32_t ROTR32(uint32_t x, uint32_t n) {
	return (uint32_t)(x >> n) | (x << (-n & 31));
}

// Choose
[[gnu::const, gnu::always_inline]] static inline uint32_t CH(uint32_t x, uint32_t y, uint32_t z) {
	return (uint32_t)((x & y) ^ (~x & z));
}

// Majority Voting Bitwise
[[gnu::const, gnu::always_inline]] static inline uint32_t MAJ(uint32_t x, uint32_t y, uint32_t z) {
	return (uint32_t)((x & y) ^ (x & z) ^ (y & z));
}

// Big Sigma 0
[[gnu::const, gnu::always_inline]] static inline uint32_t SIG0(uint32_t x) {
	return (uint32_t)(ROTR32(x, 2) ^ ROTR32(x, 13) ^ ROTR32(x, 22));
}

// Big Sigma 1
[[gnu::const, gnu::always_inline]] static inline uint32_t SIG1(uint32_t x) {
	return (uint32_t)(ROTR32(x, 6) ^ ROTR32(x, 11) ^ ROTR32(x, 25));
}

// Small Sigma 0
[[gnu::const, gnu::always_inline]] static inline uint32_t S0(uint32_t x) {
	return (uint32_t)(ROTR32(x, 7) ^ ROTR32(x, 18) ^ (x >> 3));
}

// Small Sigma 1
[[gnu::const, gnu::always_inline]] static inline uint32_t S1(uint32_t x) {
	return (uint32_t)(ROTR32(x, 17) ^ ROTR32(x, 19) ^ (x >> 10));
}

static void sha256_init(sha256_ctx* ctx) {
	ctx->h[0] = 0x6a09e667U;
	ctx->h[1] = 0xbb67ae85U;
	ctx->h[2] = 0x3c6ef372U;
	ctx->h[3] = 0xa54ff53aU;
	ctx->h[4] = 0x510e527fU;
	ctx->h[5] = 0x9b05688cU;
	ctx->h[6] = 0x1f83d9abU;
	ctx->h[7] = 0x5be0cd19U;
	ctx->buffer_len = 0;
	ctx->total_len = 0;
	memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

static void sha224_init(sha256_ctx* ctx) {
	ctx->h[0] = 0xc1059ed8U;
	ctx->h[1] = 0x367cd507U;
	ctx->h[2] = 0x3070dd17U;
	ctx->h[3] = 0xf70e5939U;
	ctx->h[4] = 0xffc00b31U;
	ctx->h[5] = 0x68581511U;
	ctx->h[6] = 0x64f98fa7U;
	ctx->h[7] = 0xbefa4fa4U;
	ctx->buffer_len = 0;
	ctx->total_len = 0;
	memset(ctx->buffer, 0, sizeof(ctx->buffer));
}

static void sha256_compression(sha256_ctx* restrict ctx, const uint8_t* restrict message) {
	uint32_t w[64];
	uint32_t h[8];

	// Copy chunk into first 16 words w[0..15] of the message schedule array
	for (uint32_t i = 0; i < 16; i++) {
		w[i] = ((uint32_t)message[i*4] << 24) | ((uint32_t)message[i*4+1] << 16) | ((uint32_t)message[i*4+2] << 8) | message[i*4+3];
	}

	// Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array
	for (uint32_t i = 16; i < 64; i++) {
		w[i] = S1(w[i-2]) + w[i-7] + S0(w[i-15]) + w[i-16];
	}

	// Initialize working variables to current hash value
	memcpy(h, ctx->h, sizeof(ctx->h));

	// Compression function main loop
	for (uint32_t i = 0; i < 64; i++) {
		uint32_t t1 = h[7] + SIG1(h[4]) + CH(h[4], h[5], h[6]) + k[i] + w[i];
		uint32_t t2 = SIG0(h[0]) + MAJ(h[0], h[1], h[2]);
		h[7] = h[6];
		h[6] = h[5];
		h[5] = h[4];
		h[4] = h[3] + t1;
		h[3] = h[2];
		h[2] = h[1];
		h[1] = h[0];
		h[0] = t1 + t2;
	}

	// Add the compressed chunk to the current hash value
	for (uint32_t i = 0; i < 8; i++) {
		ctx->h[i] += h[i];
	}

	// Cleanup
	bt_memzero(&w[0], sizeof(w));
	bt_memzero(&h[0], sizeof(h));
}

static void sha256_update(sha256_ctx* restrict ctx, const uint8_t* restrict message, uint64_t len) {
	// Add total length with new message length
	ctx->total_len += len;

	while (len > 0) {
		// Determine how many bytes to be copy first
		size_t bytes_to_copy = 64 - ctx->buffer_len;
		if (bytes_to_copy > (size_t)len) bytes_to_copy = (size_t)len;

		// Copy determined number of bytes
		memcpy(&ctx->buffer[ctx->buffer_len], message, bytes_to_copy);

		// Update buffer length, message pointer, and remaining new message length
		ctx->buffer_len += bytes_to_copy;
		message += bytes_to_copy;
		len -= (uint64_t)bytes_to_copy;

		// If the buffer is full after copy some bytes of new message, compress
		if (ctx->buffer_len == 64) {
			sha256_compression(ctx, ctx->buffer);
			ctx->buffer_len = 0;
		}
	}
}

static void sha256_padding(sha256_ctx* restrict ctx) {
	// Total length in bits
	uint64_t bit_len = ctx->total_len * 8;

	// Append 0x80
	ctx->buffer[ctx->buffer_len++] = 0x80;

	// If the buffer is full after appending 0x80, compress
	if (ctx->buffer_len == 64) {
		sha256_compression(ctx, ctx->buffer);
		ctx->buffer_len = 0;
	}

	// If not enough room for length, pad and compress
	if (ctx->buffer_len > 56) {
		while (ctx->buffer_len < 64) ctx->buffer[ctx->buffer_len++] = 0;
		sha256_compression(ctx, ctx->buffer);
		ctx->buffer_len = 0;
	}

	// Pad with zeros until 56
	while (ctx->buffer_len < 56) ctx->buffer[ctx->buffer_len++] = 0;

	// Append bit length big-endian
	for (int i = 7; i >= 0; i--) {
		ctx->buffer[ctx->buffer_len++] = (uint8_t)((bit_len >> (i * 8)) & 0xFF);
	}

	// Compress the last block
	sha256_compression(ctx, ctx->buffer);
}

static void sha256_final(sha256_ctx* restrict ctx, uint8_t* restrict digest) {
	sha256_padding(ctx);

	// Copy the hash to the output
	for (uint32_t i = 0; i < 8; i++) {
		digest[i*4] = (uint8_t)((ctx->h[i] >> 24) & 0xFF);
		digest[i*4+1] = (uint8_t)((ctx->h[i] >> 16) & 0xFF);
		digest[i*4+2] = (uint8_t)((ctx->h[i] >> 8) & 0xFF);
		digest[i*4+3] = (uint8_t)(ctx->h[i] & 0xFF);
	}

	// Cleanup
	bt_memzero(ctx, sizeof(sha256_ctx));
}

static void sha224_final(sha256_ctx* restrict ctx, uint8_t* restrict digest) {
	sha256_padding(ctx);

	// Copy the hash to the output
	for (uint32_t i = 0; i < 7; i++) {
		digest[i*4] = (uint8_t)((ctx->h[i] >> 24) & 0xFF);
		digest[i*4+1] = (uint8_t)((ctx->h[i] >> 16) & 0xFF);
		digest[i*4+2] = (uint8_t)((ctx->h[i] >> 8) & 0xFF);
		digest[i*4+3] = (uint8_t)(ctx->h[i] & 0xFF);
	}

	// Cleanup
	bt_memzero(ctx, sizeof(sha256_ctx));
}

void bt_sha256_digest(uint8_t* digest, const uint8_t* message, size_t len) {
	sha256_ctx ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, message, len);
	sha256_final(&ctx, digest);
}

void bt_sha224_digest(uint8_t* digest, const uint8_t* message, size_t len) {
	sha256_ctx ctx;
	sha224_init(&ctx);
	sha256_update(&ctx, message, len);
	sha224_final(&ctx, digest);
}