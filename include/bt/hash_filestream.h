// Berytonite Filestream

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "hash_stream.h"

int bt_hash_file_digest(const char* path, bt_hash* hash_algo, uint8_t* restrict out);
int bt_hash_file_update(bt_hash_ctx* restrict ctx, const char* path);

#ifdef __cplusplus
}
#endif