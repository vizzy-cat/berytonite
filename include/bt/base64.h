// berytonite base64 coder

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint8_t buffer[4];
	size_t buffer_len;
} base64_ctx;

static inline void base64_init(base64_ctx* ctx) {
	ctx->buffer_len = 0;
}

void base64_enc_update(base64_ctx* restrict ctx, const uint8_t* restrict in, uint8_t* restrict out, size_t len);
void base64_dec_update(base64_ctx* restrict ctx, const uint8_t* restrict in, uint8_t* restrict out, size_t len);
void base64_enc_final(base64_ctx* ctx, uint8_t* out);
void base64_dec_final(base64_ctx* ctx, uint8_t* out);

void bt_base64_encode(char* restrict encoded, const uint8_t* restrict data, size_t len);
void bt_base64_decode(uint8_t* restrict decoded, const char* restrict encoded);

#ifdef __cplusplus
}
#endif
