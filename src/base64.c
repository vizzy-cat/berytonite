#include "base64.h"
#include "error.h"
#include <stdint.h>

static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const uint8_t BASE64_REVERSE_TABLE[256] = {
	[0 ... 255] = -1,
	['A'] = 0, ['B'] = 1, ['C'] = 2,
	['D'] = 3, ['E'] = 4, ['F'] = 5,
	['G'] = 6, ['H'] = 7, ['I'] = 8,
	['J'] = 9, ['K'] = 10, ['L'] = 11,
	['M'] = 12, ['N'] = 13, ['O'] = 14,
	['P'] = 15, ['Q'] = 16, ['R'] = 17,
	['S'] = 18, ['T'] = 19, ['U'] = 20,
	['V'] = 21, ['W'] = 22, ['X'] = 23,
	['Y'] = 24, ['Z'] = 25, ['a'] = 26,
	['b'] = 27, ['c'] = 28, ['d'] = 29,
	['e'] = 30, ['f'] = 31, ['g'] = 32,
	['h'] = 33, ['i'] = 34, ['j'] = 35,
	['k'] = 36, ['l'] = 37, ['m'] = 38,
	['n'] = 39, ['o'] = 40, ['p'] = 41,
	['q'] = 42, ['r'] = 43, ['s'] = 44,
	['t'] = 45, ['u'] = 46, ['v'] = 47,
	['w'] = 48, ['x'] = 49, ['y'] = 50,
	['z'] = 51, ['0'] = 52, ['1'] = 53,
	['2'] = 54, ['3'] = 55, ['4'] = 56,
	['5'] = 57, ['6'] = 58, ['7'] = 59,
	['8'] = 60, ['9'] = 61, ['+'] = 62,
	['/'] = 63
};

// base64_encode
size_t krypton_base64_encode(char* out, const void* data, const size_t data_size) {
	if (!data) {
		krypton_set_error(KRYPTON_ERR_INVALID_SOURCE);
		return 0;
	} else if (!out) {
		krypton_set_error(KRYPTON_ERR_INVALID_DEST);
		return 0;
	} else if (data_size == 0) {
		krypton_set_error(KRYPTON_ERR_INVALID_LENGTH);
		return 0;
	} else if (*data == NULL) {
		krypton_set_error(KRYPTON_ERR_EMPTY_SOURCE);
		return 0;
	}

	const uint8_t* bytes = (const uint8_t*)data;
	size_t i = 0, j = 0;

	while (i < data_size) {
		// take 3 bytes
		uint8_t a = (i < data_size) ? bytes[i++] : 0;
		uint8_t b = (i < data_size) ? bytes[i++] : 0;
		uint8_t c = (i < data_size) ? bytes[i++] : 0;

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
		out[j++] = (i - 2 <= data_size) ? BASE64_TABLE[(size_t)group3] : '=';
		out[j++] = (i - 1 <= data_size) ? BASE64_TABLE[(size_t)group4] : '=';
	}
	
	out[j] = '\0';
	krypton_clear_error();
	return j;
}

// base64_decode
size_t krypton_base64_decode(void* out, const char* encoded, size_t data_size) {
	if (!encoded) {
		krypton_set_error(KRYPTON_ERR_INVALID_SOURCE);
		return 0;
	} else if (!out) {
		krypton_set_error(KRYPTON_ERR_INVALID_DEST);
		return 0;
	} else if (data_size == 0) {
		krypton_set_error(KRYPTON_ERR_INVALID_LENGTH);
		return 0;
	} else if (*encoded == NULL) {
		krypton_set_error(KRYPTON_ERR_EMPTY_SOURCE);
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
			int8_t val = BASE64_REVERSE_TABLE[(unsigned char)c];

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

	krypton_clear_error();
	return j;
}
