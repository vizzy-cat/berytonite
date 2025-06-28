#include "secure_random.h"

#if defined(__linux__)
	#include <sys/random.h>
	#include <sys/types.h>
	#include <fcntl.h>
	#include <unistd.h>
#elif defined(_WIN32)
	#include <windows.h>
	#include <bcrypt.h>
#elif defined(__APPLE__)
	#include <Security/Security.h>
#else
	#error "Unsupported Platform"
#endif

// secure_random
bool beryton_secure_random(void* buffer, size_t buffer_length) {
#if defined(__linux__)
	// Linux
#if defined(__ANDROID__)
	// Android fallback
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		return false;
	}

	ssize_t total = 0;
	while (total < (ssize_t)buffer_length) {
		ssize_t r = read(fd, (char*)buffer + total, buffer_length - total);
		if (r <= 0) {
			close(fd);
			return false;
		}
		total += r;
	}

	close(fd);
#else
	int out = getrandom(buffer, buffer_length, 0);

	if (out != (ssize_t)buffer_length) {
		return false;
	}
#endif // Android
#elif defined(_WIN32)
	// Windows
	if (BCryptGenRandom(NULL, buffer, (ULONG)buffer_length, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
		return false;
	}
#elif defined(__APPLE__)
	// Apple Ecosystem
	if (SecRandomCopyBytes(kSecRandomDefault, buffer_length, buffer) != errSecSuccess) {
		return false;
	}
#endif
	return true;
}
