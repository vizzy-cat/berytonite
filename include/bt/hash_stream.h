// berytonite universal hash stream

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bt_hash bt_hash;

typedef struct {
	void* internal_ctx;
	bt_hash* hash_algo;
} bt_hash_ctx;

void bt_hash_init(bt_hash_ctx* ctx, bt_hash* hash_algo);
void bt_hash_update(bt_hash_ctx* restrict ctx, const uint8_t* restrict in, size_t len);
void bt_hash_final(bt_hash_ctx* ctx, uint8_t* out);
void bt_hash_free(bt_hash_ctx* ctx);

static inline void bt_hash_reuse(bt_hash_ctx* ctx, bt_hash* hash_algo) {
	bt_hash_free(ctx);
	bt_hash_init(ctx, hash_algo);
}

#ifdef __cplusplus
}
#endif