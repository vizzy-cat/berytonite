#include "sha256.h"
#include "util.h"
#include "internal/attribute.h"
#include "internal/stream.h"
#include <stdalign.h>
#include <string.h>

#if defined(ENABLE_SSE)
#include <emmintrin.h> // SSE2
#endif

// SHA2-256's constant round from FIPS PUB 180-4
static const uint32_t k[64] = {
	0x428a2f98, 0x71374491,
	0xb5c0fbcf, 0xe9b5dba5,
	0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01,
	0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe,
	0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc,
	0x2de92c6f, 0x4a7484aa,
	0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d,
	0xb00327c8, 0xbf597fc7,
	0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138,
	0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb,
	0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3,
	0xd192e819, 0xd6990624,
	0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08,
	0x2748774c, 0x34b0bcb5,
	0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f,
	0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb,
	0xbef9a3f7, 0xc67178f2
};

// Bitwise Transformation Functions

// Scalar Rotate Right
CONST_ATT
static inline uint32_t ROTR32(uint32_t x, uint32_t n) {
	return (uint32_t)((x >> n) | (x << (32 - n)));
}

#if defined(ENABLE_SSE)
// SIMD Rotate Right
CONST_ATT
static inline __m128i ROTR128(__m128i x, uint32_t n) {
	__m128i a = _mm_srli_epi32(x, (int)n);
	__m128i b = _mm_slli_epi32(x, (int)(32U-n));
	return _mm_or_si128(a, b);
}
#endif

// Shift Right
/*CONST_ATT
static inline uint32_t SHR(uint32_t x, uint32_t n) {
	return (uint32_t)(x >> n);
}*/

// Choose
CONST_ATT
static inline uint32_t CH(uint32_t x, uint32_t y, uint32_t z) {
	return (uint32_t)((x & y) ^ (~x & z));
}

// Majority Voting Bitwise
CONST_ATT
static inline uint32_t MAJ(uint32_t x, uint32_t y, uint32_t z) {
	return (uint32_t)((x & y) ^ (x & z) ^ (y & z));
}

// Big Sigma 0
CONST_ATT
static inline uint32_t SIG0(uint32_t x) {
	return (uint32_t)(ROTR32(x, 2) ^ ROTR32(x, 13) ^ ROTR32(x, 22));
}

// Big Sigma 1
CONST_ATT
static inline uint32_t SIG1(uint32_t x) {
	return (uint32_t)(ROTR32(x, 6) ^ ROTR32(x, 11) ^ ROTR32(x, 25));
}

// Small Sigma 0
CONST_ATT
static inline uint32_t S0_32(uint32_t x) {
	return (uint32_t)(ROTR32(x, 7) ^ ROTR32(x, 18) ^ (x >> 3));
}

// Small Sigma 1
CONST_ATT
static inline uint32_t S1_32(uint32_t x) {
	return (uint32_t)(ROTR32(x, 17) ^ ROTR32(x, 19) ^ (x >> 10));
}

#if defined(ENABLE_SSE)
// SSE4
// Small Sigma 0
CONST_ATT
static inline __m128i S0_128(__m128i x) {
	// return (uint32_t)(ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3));
	__m128i a = ROTR128(x, 7);
	__m128i b = ROTR128(x, 18);
	__m128i c = _mm_srli_epi32(x, 3);
	return _mm_xor_si128(_mm_xor_si128(a, b), c);
}

// Small Sigma 1
CONST_ATT
static inline __m128i S1_128(__m128i x) {
	// return (uint32_t)(ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10));
	__m128i a = ROTR128(x, 17);
	__m128i b = ROTR128(x, 19);
	__m128i c = _mm_srli_epi32(x, 10);
	return _mm_xor_si128(_mm_xor_si128(a, b), c);
}

// Context struct for SHA-256
// SSE Aligned
typedef struct {
	alignas(16) uint8_t buffer[64];	// Buffer for 512-bit block
	alignas(16) uint32_t state[8];	// State
	size_t buffer_len;	// Size of unprocessed data
	size_t total_len;	// Size of total data
} sha256_ctx;
#else
// Scalar
// Context struct for SHA-256
typedef struct {
	uint8_t buffer[64];	// Buffer for 512-bit block
	uint32_t state[8];	// State
	size_t buffer_len;	// Size of unprocessed data
	size_t total_len;	// Size of total data
} sha256_ctx;
#endif

// SHA2-256's transformation function
#if defined(ENABLE_SSE)
// SSE4
static void sha256_transform(sha256_ctx* restrict ctx, const uint8_t* restrict data) {
	alignas(16) uint32_t w[64];		// Expanded message schedule
	alignas(16) uint32_t state[8];

	// Parse the first 16 word of input (big endian)
	for (int i = 0; i < 16; i++) {
		w[i] = ((uint32_t)data[i*4] << 24) |
				((uint32_t)data[i*4+1] << 16) |
				((uint32_t)data[i*4+2] << 8) |
				data[i*4+3];
	}

	// Expand to 64 word
	for (int i = 16; i < 64; i++) {
		w[i] = S1_32(w[i - 2]) + w[i - 7] + S0_32(w[i - 15]) + w[i - 16];
	}

	// Load state to local register
	{
		__m128i tmp = _mm_load_si128((__m128i*)&ctx->state[0]);
		_mm_storeu_si128((__m128i*)&state[0], tmp);
		tmp = _mm_load_si128((__m128i*)&ctx->state[4]);
		_mm_storeu_si128((__m128i*)&state[4], tmp);
	}

	// 64 Round of SHA-256
	UNROLL(4) // Unroll pragma for compiler
	for (int i = 0; i < 64; i++) {
		uint32_t t1 = state[7] + SIG1(state[4]) + CH(state[4], state[5], state[6]) + k[i] + w[i];
		uint32_t t2 = SIG0(state[0]) + MAJ(state[0], state[1], state[2]);
		state[7] = state[6];
		state[6] = state[5];
		state[5] = state[4];
		state[4] = state[3] + t1;
		state[3] = state[2];
		state[2] = state[1];
		state[1] = state[0];
		state[0] = t1 + t2;
	}

	// Add to state
	{
		__m128i a = _mm_loadu_si128((__m128i*)&ctx->state[0]);
		__m128i b = _mm_loadu_si128((__m128i*)&state[0]);
		_mm_storeu_si128((__m128i*)&ctx->state[0], _mm_add_epi32(a, b));
		a = _mm_loadu_si128((__m128i*)&ctx->state[4]);
		b = _mm_loadu_si128((__m128i*)&state[4]);
		_mm_storeu_si128((__m128i*)&ctx->state[4], _mm_add_epi32(a, b));
	}

	// Cleanup
	bt_memzero(&w[0], sizeof(w));
	bt_memzero(&state[0], sizeof(state));
}
#else
// Scalar
static void sha256_transform(sha256_ctx* restrict ctx, const uint8_t* restrict data) {
	uint32_t w[64];		// Expanded message schedule
	uint32_t state[8];

	// Parse the first 16 word of input (big endian)
	UNROLL(4)
	for (int i = 0; i < 16; i++) {
		w[i] = ((uint32_t)data[i*4] << 24) |
				((uint32_t)data[i*4+1] << 16) |
				((uint32_t)data[i*4+2] << 8) |
				data[i*4+3];
	}

	// Expand to 64 word
	for (int i = 16; i < 64; i++) {
		w[i] = S1_32(w[i - 2]) + w[i - 7] + S0_32(w[i - 15]) + w[i - 16];
	}

	// Load state to local register
	memcpy(state, ctx->state, sizeof(ctx->state));

	// 64 Round of SHA-256
	UNROLL(4) // Unroll pragma for compiler
	for (int i = 0; i < 64; i++) {
		uint32_t t1 = state[7] + SIG1(state[4]) + CH(state[4], state[5], state[6]) + k[i] + w[i];
		uint32_t t2 = SIG0(state[0]) + MAJ(state[0], state[1], state[2]);
		state[7] = state[6];
		state[6] = state[5];
		state[5] = state[4];
		state[4] = state[3] + t1;
		state[3] = state[2];
		state[2] = state[1];
		state[1] = state[0];
		state[0] = t1 + t2;
	}

	// Add to state
	for (int i = 0; i < 8; i++) {
		ctx->state[i] += state[i];
	}

	// Cleanup
	bt_memzero(&w[0], sizeof(w));
	bt_memzero(&state[0], sizeof(state));
}
#endif

// Context initialization
static void sha256_init(sha256_ctx* ctx) {
	// Set IV to state
#if defined(ENABLE_SSE)
	{
		__m128i tmp = _mm_setr_epi32((int)0x6a09e667U, (int)0xbb67ae85U, (int)0x3c6ef372U, (int)0xa54ff53aU);
		_mm_store_si128((__m128i*)&ctx->state[0], tmp);
		tmp = _mm_setr_epi32((int)0x510e527fU, (int)0x9b05688cU, (int)0x1f83d9abU, (int)0x5be0cd19U);
		_mm_store_si128((__m128i*)&ctx->state[4], tmp);
	}
#else
	ctx->state[0] = 0x6a09e667U;
	ctx->state[1] = 0xbb67ae85U;
	ctx->state[2] = 0x3c6ef372U;
	ctx->state[3] = 0xa54ff53aU;
	ctx->state[4] = 0x510e527fU;
	ctx->state[5] = 0x9b05688cU;
	ctx->state[6] = 0x1f83d9abU;
	ctx->state[7] = 0x5be0cd19U;
#endif

	ctx->buffer_len = 0;
	ctx->total_len = 0;
}

// Process the data in stages
static void sha256_update(sha256_ctx* restrict ctx, const uint8_t* restrict in, size_t len) {
	ctx->total_len += len;
	size_t i = 0;
	while (len > 0) {
		size_t to_copy = 64 - ctx->buffer_len;
		if (to_copy > len) to_copy = len;

		// Copy data to buffer
		for (size_t j = 0; j < to_copy; j++) 
			ctx->buffer[ctx->buffer_len + j] = in[i + j];

		ctx->buffer_len += to_copy;
		i += to_copy;
		len -= to_copy;

		// If the buffer is full
		if (ctx->buffer_len == 64) {
			// Process immediately
			sha256_transform(ctx, ctx->buffer);
			ctx->buffer_len = 0;
		}
	}
}


// Hash finalization
static void sha256_final(sha256_ctx* restrict ctx, uint8_t* restrict out) {
	// Total length in bit
	uint64_t bit_len = ctx->total_len * 8;

	// Add '1' bit
	ctx->buffer[ctx->buffer_len++] = 0x80;

	// Add padding until 56 bytes
	if (ctx->buffer_len > 56) {
		while (ctx->buffer_len < 64)
			ctx->buffer[ctx->buffer_len++] = 0;
		sha256_transform(ctx, ctx->buffer);
		ctx->buffer_len = 0;
	}

	// Add zero until 56 bytes offset
	while (ctx->buffer_len < 56)
		ctx->buffer[ctx->buffer_len++] = 0;

	// Add length (big-endian)
	for (int i = 7; i >= 0; i--)
		ctx->buffer[ctx->buffer_len++] = (bit_len >> (i * 8)) & 0xFF;

	// Process the last block
	sha256_transform(ctx, ctx->buffer);

	// Copy the hash to the output
	for (int i = 0; i < 8; i++) {
		out[i*4] = (uint8_t)((ctx->state[i] >> 24) & 0xFF);
		out[i*4+1] = (uint8_t)((ctx->state[i] >> 16) & 0xFF);
		out[i*4+2] = (uint8_t)((ctx->state[i] >> 8) & 0xFF);
		out[i*4+3] = (uint8_t)(ctx->state[i] & 0xFF);
	}

	// Cleanup
	bt_memzero(&ctx->buffer, sizeof(ctx->buffer));
	bt_memzero(&ctx->state, sizeof(ctx->state));
}

// SHA-256 Algorithm Descriptor
const bt_hash bt_sha256 = {
	.init = (void(*)(void*))sha256_init,
	.update = (void(*)(void* restrict, const uint8_t* restrict, size_t))sha256_update,
	.final = (void(*)(void*, uint8_t*))sha256_final,
	.ctx_size = sizeof(sha256_ctx)
};

// Single digest function
void bt_sha256_digest(uint8_t* digest, const uint8_t* data, size_t len) {
	sha256_ctx ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, len);
	sha256_final(&ctx, digest);
}
