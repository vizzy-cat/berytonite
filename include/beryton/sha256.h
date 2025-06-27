// berytonite sha256 digest

#pragma once

#include <stdint.h>
#include <stddef.h>

#define BERYTON_SHA256_DIGEST_SIZE 32

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t h[8];
	uint8_t buffer[64];
	size_t total_len;
	size_t buffer_len;
} beryton_sha256_ctx;

void beryton_sha256_init(beryton_sha256_ctx* ctx);
void beryton_sha256_update(beryton_sha256_ctx* ctx, const uint8_t* data, size_t len);
void beryton_sha256_final(beryton_sha256_ctx* ctx, uint8_t* digest);

void beryton_sha256_digest(uint8_t* digest, const uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif
