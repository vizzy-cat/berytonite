// berytonite universal api

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "internal/forward.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	void* algo_ctx;
	bt_algo* algo;
} bt_ctx;

void bt_init(bt_ctx* ctx, bt_algo* algo);
void bt_update(bt_ctx* ctx, const uint8_t* in, size_t len, uint8_t* out);
void bt_final(bt_ctx* ctx, uint8_t* out);
void bt_free(bt_ctx* ctx);
inline void bt_reuse(bt_ctx* ctx);

#ifdef __cplusplus
}
#endif
