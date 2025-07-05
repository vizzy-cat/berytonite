// berytonite internal api

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bt_algo {
	void (*init)(void*);
	void (*update)(void*, uint8_t*, size_t, uint8_t*);
	void (*final)(void*, uint8_t*);
	size_t ctx_size;
} bt_algo_ctx;

#ifdef __cplusplus
}
#endid
