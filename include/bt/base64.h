// berytonite base64 coder

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t bt_base64_encode(char* restrict out, const uint8_t* restrict in, size_t len);
size_t bt_base64_decode(uint8_t* restrict out, const char* restrict in, size_t len);

#ifdef __cplusplus
}
#endif
