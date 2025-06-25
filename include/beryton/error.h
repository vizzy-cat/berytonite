// berytonite error handler

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	BERYTON_OK = 0,
	BERYTON_ERR_INVALID_SOURCE,
	BERYTON_ERR_INVALID_DEST,
	BERYTON_ERR_INVALID_LENGTH,
	BERYTON_ERR_RAND_FAILED,
	BERYTON_ERR_EMPTY_SOURCE
} beryton_error_t;

void beryton_set_error(beryton_error_t error);
beryton_error_t beryton_get_error();
void beryton_clear_error();

#ifdef __cplusplus
}
#endif
