// berytonite sha256 digest

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct bt_hash bt_hash;

#ifndef BT_SHA256_DIGEST_SIZE
#define BT_SHA256_DIGEST_SIZE 32
#endif

extern const bt_hash bt_sha256;

void bt_sha256_digest(uint8_t* digest, const uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif
