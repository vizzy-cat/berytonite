// berytonite internal

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef struct bt_hash {
	void (*init)(void*);
	void (*update)(void* restrict, const uint8_t* restrict, size_t);
	void (*final)(void*, uint8_t*);
	size_t ctx_size;
} bt_hash;

#ifdef __cplusplus
}
#endif
