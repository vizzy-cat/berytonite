#include "util.h"
#include "internal/attribute.h"
#include <stdio.h>
#include <string.h>

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

void bt_bytetohex(char* out, const uint8_t* in, size_t len) {
	for (size_t i = 0; i < len; i++) {
		sprintf(&out[i * 2], "%02x", in[i]);
	}
	out[len * 2] = '\0';
}

void bt_memzero(void* ptr, size_t len) {
	if (ptr == NULL) {
		return 0;
	} else if (len == 0) {
		return 0;
	}
#if defined(__GNUC__)||defined(__clang__)
	volatile unsigned char* p = (volatile unsigned char*)ptr;
	while (len--) *p++ = 0;
#elif defined(_WIN32)
	SecureZeroMemory(ptr, len);
#else
	memset(ptr, 0, len);

	asm volatile ("" : : "r"(ptr) : "memory");
#endif
}

// secure_random
size_t bt_secure_rand(void* buffer, size_t len) { 
	if (buffer == NULL) {
		return 0;
	} else if (len == 0) {
		return 0;
	}
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
		ssize_t r = read(fd, (char*)buffer + total, len - total);
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