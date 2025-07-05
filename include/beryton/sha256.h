// berytonite sha256 digest

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "internal/forward.h"

#define BERYTON_SHA256_DIGEST_SIZE 32

#ifdef __cplusplus
extern "C" {
#endif

extern const bt_algo bt_sha256;

void bt_sha256_digest(uint8_t* digest, const uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif
