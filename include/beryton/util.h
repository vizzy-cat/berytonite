// berytonite utility header

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void beryton_bytetohex(char* out, const uint8_t* in, size_t len);

#ifdef __cplusplus
}
#endif
