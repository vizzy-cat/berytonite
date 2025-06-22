#include "error.h"

#ifdef _MSC_VER
static __declspec(thread) krypton_error_t global = 0;
#else
static _Thread_local krypton_error_t global = 0;
#endif

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
