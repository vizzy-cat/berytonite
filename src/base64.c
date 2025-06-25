#include "base64.h"
#include "error.h"
#include <stdint.h>
#include <string.h>

static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int find(char c) {
	for (int i = 0; i < 64; i++) {
		if (BASE64_TABLE[i] == c) {
			return i;
		}
	}
	return -1;
}

// base64_encode
size_t beryton_base64_encode(char* out, const void* data, const size_t data_size) {
	if (!data) {
		beryton_set_error(BERYTON_ERR_INVALID_SOURCE);
		return 0;
	} else if (!out) {
		beryton_set_error(BERYTON_ERR_INVALID_DEST);
		return 0;
	} else if (data_size == 0) {
		beryton_set_error(BERYTON_ERR_INVALID_LENGTH);
		return 0;
	}

	const uint8_t* bytes = (const uint8_t*)data;
	size_t i = 0, j = 0;

	while (i < data_size) {
		size_t start = i;
		// take 3 bytes
		uint8_t a = (i < data_size) ? bytes[i++] : 0;
		uint8_t b = (i < data_size) ? bytes[i++] : 0;
		uint8_t c = (i < data_size) ? bytes[i++] : 0;
		size_t remaining = data_size - start;

		// combine into single 24-bit
		unsigned _BitInt(24) combined = (unsigned _BitInt(24))((a << 16) | (b << 8) | c);

		// split into 4 group of 6-bit
		unsigned _BitInt(6) group1 = (unsigned _BitInt(6))((combined >> 18) & 0x3F);
		unsigned _BitInt(6) group2 = (unsigned _BitInt(6))((combined >> 12) & 0x3F);
		unsigned _BitInt(6) group3 = (unsigned _BitInt(6))((combined >> 6) & 0x3F);
		unsigned _BitInt(6) group4 = (unsigned _BitInt(6))(combined & 0x3F);
		
		// translate it into Base64
		out[j++] = BASE64_TABLE[(size_t)group1];
		out[j++] = BASE64_TABLE[(size_t)group2];
		out[j++] = (remaining > 1) ? BASE64_TABLE[(size_t)group3] : '=';
		out[j++] = (remaining > 2) ? BASE64_TABLE[(size_t)group4] : '=';
	}
	
	out[j] = '\0';
	beryton_clear_error();
	return j;
}

// base64_decode
size_t beryton_base64_decode(void* out, const char* encoded, size_t data_size) {
	if (!encoded) {
		beryton_set_error(BERYTON_ERR_INVALID_SOURCE);
		return 0;
	} else if (!out) {
		beryton_set_error(BERYTON_ERR_INVALID_DEST);
		return 0;
	} else if (data_size == 0) {
		beryton_set_error(BERYTON_ERR_INVALID_LENGTH);
		return 0;
	}

	uint8_t* bytes = (uint8_t*)out;
	size_t i = 0, j = 0;
	unsigned _BitInt(24) buffer = 0;
	int pad = 0;
	int group = 0;

	while (encoded[i]) {
		char c = encoded[i++];

		// translate it into bytes
		if (c == '=') {
			// if it's a padding, skip
			pad++;
			buffer <<= 6;
			group++;
		} else {
			// take the value from lookup table
			int val = find(c);

			if (val < 0) continue;
			buffer = (buffer << 6) | val;

			group++;
		}
		
		// convert it into 3 bytes
		if (group == 4) {
			bytes[j++] = (buffer >> 16) & 0xFF;
			if (pad < 2) bytes[j++] = (buffer >> 8) & 0xFF;
			if (pad < 1) bytes[j++] = buffer & 0xFF;

			buffer = 0;
			pad = 0;
			group = 0;
		}
	}

	beryton_clear_error();
	return j;
}
