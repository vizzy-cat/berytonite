#include "util.h"
#include "internal/attribute.h"
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
	#include <windows.h>
#endif

void beryton_bytetohex(char* out, const uint8_t* in, size_t len) {
	for (size_t i = 0; i < len; i++) {
		sprintf(&out[i * 2], "%02x", in[i]);
	}
	out[len * 2] = '\0';
}

void beryton_memzero(void* ptr, size_t len) {
#if defined(__GNUC__)||defined(__clang__)
	volatile unsigned char* p = (volatile unsigned char*)ptr;
	while (len--) *p++ = 0;
#elif defined(_MSC_VER)
	SecureZeroMemory(ptr, len);
#else
	memset(ptr, 0, len);

	asm volatile ("" : : "r"(ptr) : "memory");
#endif
}

