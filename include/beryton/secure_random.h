// berytonite secure random

#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

bool beryton_secure_random(void* buffer, size_t buffer_length);

#ifdef __cplusplus
}
#endif
