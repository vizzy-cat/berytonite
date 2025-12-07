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

// bytetohex
void bt_bytetohex(char* out, const uint8_t* in, size_t len) {
	// check each parameter
	if (ptr == NULL) {
		return 0;
	} else if (len == 0) {
		return 0;
	}

	// start to convert every bytes to a hexadecimal string
	for (size_t i = 0; i < len; i++) {
		sprintf(&out[i * 2], "%02x", in[i]);
	}

	// put the string TERMINATOR at the end of string
	out[len * 2] = '\0';
}

// memzero
void bt_memzero(void* ptr, size_t len) {
	// check each parameter
	if (ptr == NULL) {
		return 0;
	} else if (len == 0) {
		return 0;
	}

	// we use different approach for different platform
#if defined(_WIN32)
	// if the platform is Windows, use their own memzero
	SecureZeroMemory(ptr, len);
#else
	// if something else, do manually instead
	volatile unsigned char* p = (volatile unsigned char*)ptr;
	while (len--) *p++ = 0;
#endif
}

// secure_random
size_t bt_secure_rand(void* buffer, size_t len) {
	// check each parameter
	if (ptr == NULL) {
		return 0;
	} else if (len == 0) {
		return 0;
	}

	// again, we use different approach for different platform
#if defined(__linux__)
	// if the platform is Linux, use their own syscall
#if defined(__ANDROID__)
	// if the platform is Android, use the old fashion way
	// why we need these things? ask some phone manufacturers WHO DONT UPDATE THEIR C LIBRARY

	// load the whitenoise
	int fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0) {
		// why this thing even need to failed?
		return 0;
	}

	// start to fill up the buffer with random noise
	ssize_t total = 0;

	// if the total size of taken bytes from the whitenoise
	// is less than the requested size, repeat to take some bytes
	// until the condition is false or the system is somehow crashed
	while (total < (ssize_t)len) {
		// take some bytes from the whitenoise and move it into the buffer
		ssize_t r = read(fd, (char*)buffer + total, len - total);
		
		// if it failed, why do you even asking?
		if (r <= 0) {
			close(fd);
			return 0;
		}

		// adds the total size of taken bytes from whitenoise
		total += r;
	}

	// close the whitenoise
	close(fd);
#else
	// if the platform isnt Android, use the syscall
	int out = getrandom(buffer, len, 0);

	if (out != (ssize_t)len) {
		return 0;
	}
#endif // Android
#elif defined(_WIN32)
	// if the platform is Windows, use their own syscall too
	if (BCryptGenRandom(NULL, buffer, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
		return 0;
	}
#elif defined(__APPLE__)
	// if the platform is Apple, use their own syscall too
	if (SecRandomCopyBytes(kSecRandomDefault, len, buffer) != errSecSuccess) {
		return 0;
	}
#endif
	return len;
}