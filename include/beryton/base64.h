// berytonite base64 coder

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t beryton_base64_encode(char* out, const uint8_t* data, size_t length);
size_t beryton_base64_decode(uint8_t* out, const char* encoded);

#ifdef __cplusplus
}
#endif
