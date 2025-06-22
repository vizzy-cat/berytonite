#include "secure_random.h"
#include "error.h"

#if defined(__linux__)
	#include <sys/random.h>
	#include <sys/types.h>
#elif defined(_WIN32)
	#include <windows.h>
	#include <bcrypt.h>
	#pragma comment(lib, "bcrypt.lib")
#elif defined(__APPLE__)
	#include <Security/Security.h>
#else
	#error "Unsupported Platform"
#endif

// secure_random
bool krypton_secure_random(void* buffer, size_t buffer_length) {
	if (!buffer) {
		krypton_set_error(KRYPTON_ERR_NULLPTR);
		return false;
	} else if (buffer_length == 0) {
		krypton_set_error(KRYPTON_ERR_INVALID_LENGTH);
		return false;
	}

#if defined(__linux__)
	// Linux
	if (getrandom(buffer, buffer_length, 0) != (ssize_t)buffer_length) {
		krypton_set_error(KRYPTON_ERR_RAND_FAILED);
		return false;
	}
#elif defined(_WIN32)
	// Windows
	if (BCryptGenRandom(NULL, buffer, (ULONG)buffer_length, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
		krypton_set_error(KRYPTON_ERR_RAND_FAILED);
		return false;
	}
#elif defined(__APPLE__)
	// Apple Ecosystem
	if (SecRandomCopyBytes(kSecRandomDefault, buffer_length, buffer) != errSecSuccess) {
		krypton_set_error(KRYPTON_ERR_RAND_FAILED);
		return false;
	}
#endif
	krypton_clear_error();
	return true;
}
