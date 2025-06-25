#include "error.h"

#ifdef _MSC_VER
static __declspec(thread) beryton_error_t global = 0;
#else
static _Thread_local beryton_error_t global = 0;
#endif

// set_error
void beryton_set_error(beryton_error_t error) {
	global = error;
}

// get_error
beryton_error_t beryton_get_error() {
	return global;
}

// clear_error
void beryton_clear_error() {
	global = KRYPTON_OK;
}
