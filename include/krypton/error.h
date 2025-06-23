// krypton error handler

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	KRYPTON_OK = 0,
	KRYPTON_ERR_INVALID_SOURCE,
	KRYPTON_ERR_INVALID_DEST,
	KRYPTON_ERR_INVALID_LENGTH,
	KRYPTON_ERR_RAND_FAILED
} krypton_error_t;

void krypton_set_error(krypton_error_t error);
krypton_error_t krypton_get_error();
void krypton_clear_error();

#ifdef __cplusplus
}
#endif
