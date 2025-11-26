// berytonite utility header

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void bt_bytetohex(char* out, const uint8_t* in, size_t len);
void bt_memzero(void* ptr, size_t len);
size_t bt_secure_rand(void* buffer, size_t len);

#ifdef __cplusplus
}
#endif
