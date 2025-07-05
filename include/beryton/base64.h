// berytonite base64 coder

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "internal/forward.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const bt_algo bt_base64_enc;
extern const bt_algo bt_base64_dec;

void bt_base64_encode(char* out, const uint8_t* data, size_t length);
void bt_base64_decode(uint8_t* out, const char* encoded);

#ifdef __cplusplus
}
#endif
