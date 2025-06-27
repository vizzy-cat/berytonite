#include "util.h"
#include <stdio.h>
#include <string.h>

void beryton_bytetohex(char* out, const uint8_t* in, size_t len) {
	for (size_t i = 0; i < len; i++) {
		sprintf(&out[i * 2], "%02x", in[i]);
	}
	out[len * 2] = '\0';
}
