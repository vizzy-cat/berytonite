#include "error.h"

static krypton_error_t global = 0;

// set_error
void krypton_set_error(krypton_error_t error) {
	global = error;
}

// get_error
krypton_error_t krypton_get_error() {
	return global;
}

// clear_error
void krypton_clear_error() {
	global = KRYPTON_OK;
}
