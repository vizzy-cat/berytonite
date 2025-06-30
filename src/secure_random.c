#include "util.h"

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
size_t bt_secure_rand(void* buffer, size_t len) {
#if defined(__linux__)
	// Linux
#if defined(__ANDROID__)
	// Android fallback
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		return 0;
	}

	ssize_t total = 0;
	while (total < (ssize_t)len) {
		ssize_t r = read(fd, (char*)(buffer + total), len - total);
		if (r <= 0) {
			close(fd);
			return 0;
		}
		total += r;
	}

	close(fd);
#else
	int out = getrandom(buffer, len, 0);

	if (out != (ssize_t)len) {
		return 0;
	}
#endif // Android
#elif defined(_WIN32)
	// Windows
	if (BCryptGenRandom(NULL, buffer, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
		return 0;
	}
#elif defined(__APPLE__)
	// Apple Ecosystem
	if (SecRandomCopyBytes(kSecRandomDefault, len, buffer) != errSecSuccess) {
		return 0;
	}
#endif
	return len;
}
