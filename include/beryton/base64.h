// berytonite base64 coder

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t beryton_base64_encode(char* out, const void* data, size_t data_size);
size_t beryton_base64_decode(void* out, const char* encoded, size_t data_size);

#ifdef __cplusplus
}
#endif
