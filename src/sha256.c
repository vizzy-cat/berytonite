#include "sha256.h"
#include "internal/attribute.h"
#include "internal/compiler.h"
#include "internal/api.h"

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

// transformation functions
static inline INLINE_ATT uint32_t ROTR(uint32_t x, uint32_t n) { return ((x >> n) | (x << (32 - n))); }
static inline INLINE_ATT uint32_t SHR(uint32_t x, uint32_t n) { return (x >> n); }
static inline INLINE_ATT uint32_t CH(uint32_t x, uint32_t y, uint32_t z) { return ((x & y) ^ (~x & z)); }
static inline INLINE_ATT uint32_t MAJ(uint32_t x, uint32_t y, uint32_t z) { return ((x & y) ^ (x & z) ^ (y & z)); }
static inline INLINE_ATT uint32_t SIG0(uint32_t x) { return (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22)); }
static inline INLINE_ATT uint32_t SIG1(uint32_t x) { return (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25)); }
static inline INLINE_ATT uint32_t S0(uint32_t x) { return (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3)); }
static inline INLINE_ATT uint32_t S1(uint32_t x) { return (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10)); }

// sha256_ctx
typedef struct {
	uint32_t h[8];
	uint8_t buffer[64];
	size_t buffer_len;
	size_t total_len;
} sha256_ctx;

// sha256_transform
static void sha256_transform(sha256_ctx* restrict ctx, const uint8_t* restrict data) {
	uint32_t w[64];
	uint32_t a, b, c, d, e, f, g, h;

	for (int i = 0; i < 16; ++i)
		w[i] = (data[i * 4] << 24) | (data[i * 4 + 1] << 16) | (data[i * 4 + 2] << 8) | data[i * 4 + 3];

	for (int i = 16; i < 64; i++)
		w[i] = S1(w[i - 2]) + w[i - 7] + S0(w[i - 15]) + w[i - 16];

	a = ctx->h[0];
	b = ctx->h[1];
	c = ctx->h[2];
	d = ctx->h[3];
	e = ctx->h[4];
	f = ctx->h[5];
	g = ctx->h[6];
	h = ctx->h[7];

	UNROLL(4)
	for (int i = 0; i < 64; i++) {
		uint32_t t1 = h + SIG1(e) + CH(e, f, g) + k[i] + w[i];
		uint32_t t2 = SIG0(a) + MAJ(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->h[0] += a;
	ctx->h[1] += b;
	ctx->h[2] += c;
	ctx->h[3] += d;
	ctx->h[4] += e;
	ctx->h[5] += f;
	ctx->h[6] += g;
	ctx->h[7] += h;
}

// sha256_init
static void sha256_init(void* ctx_ptr) {
	sha256_ctx* ctx = (sha256_ctx*)ctx_ptr;

	ctx->h[0] = 0x6a09e667;
	ctx->h[1] = 0xbb67ae85;
	ctx->h[2] = 0x3c6ef372;
	ctx->h[3] = 0xa54ff53a;
	ctx->h[4] = 0x510e527f;
	ctx->h[5] = 0x9b05688c;
	ctx->h[6] = 0x1f83d9ab;
	ctx->h[7] = 0x5be0cd19;
	ctx->buffer_len = 0;
	ctx->total_len = 0;
}

static void sha256_final(void* ctx_ptr, uint8_t* out);

// sha256_update
static void sha256_update(void* ctx_ptr, const uint8_t* in, size_t len, uint8_t* out) {
	sha256_ctx* ctx = (sha256_ctx*)ctx_ptr;

	ctx->total_len += len;
	size_t i = 0;
	while (len > 0) {
		size_t to_copy = 64 - ctx->buffer_len;
		if (to_copy > len) to_copy = len;

		for (size_t j = 0; j < to_copy; j++) 
			ctx->buffer[ctx->buffer_len + j] = in[i + j];

		ctx->buffer_len += to_copy;
		i += to_copy;
		len -= to_copy;
		if (ctx->buffer_len == 64) {
			sha256_transform(ctx, ctx->buffer);
			ctx->buffer_len = 0;
		}
	}

	if (out) return;
}

// sha256_final
static void sha256_final(void* ctx_ptr, uint8_t* out) {
	sha256_ctx* ctx = (sha256_ctx*)ctx_ptr;

	uint64_t bit_len = ctx->total_len * 8;
	ctx->buffer[ctx->buffer_len++] = 0x80;

	if (ctx->buffer_len > 56) {
		while (ctx->buffer_len < 64)
			ctx->buffer[ctx->buffer_len++] = 0;
		sha256_transform(ctx, ctx->buffer);
		ctx->buffer_len = 0;
	}

	while (ctx->buffer_len < 56)
		ctx->buffer[ctx->buffer_len++] = 0;

	for (int i = 7; i >= 0; i--)
		ctx->buffer[ctx->buffer_len++] = (bit_len >> (i * 8)) & 0xFF;
	sha256_transform(ctx, ctx->buffer);

	for (int i = 0; i < 8; i++) {
		out[i*4] = (ctx->h[i] >> 24) & 0xFF;
		out[i*4+1] = (ctx->h[i] >> 16) & 0xFF;
		out[i*4+2] = (ctx->h[i] >> 8) & 0xFF;
		out[i*4+3] = ctx->h[i] & 0xFF;
	}
}

// bt_sha256
const bt_algo bt_sha256 = {
	.init = sha256_init,
	.update = sha256_update,
	.final = sha256_final,
	.ctx_size = sizeof(sha256_ctx),
};

// sha256_digest
void bt_sha256_digest(uint8_t* digest, const uint8_t* data, size_t len) {
	sha256_ctx ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, data, len, NULL);
	sha256_final(&ctx, digest);
}
