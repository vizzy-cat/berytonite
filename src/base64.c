#include "base64.h"
#include <stdint.h>
#include <stdalign.h>
#include <stddef.h>

// SSSE3
//#include <tmmintrin.h>

//#define ENCODE_BITMASK _mm_setr_epi8( 0,1,2, 1,2,3, 2,3,4, 3,4,5, 4,5,6, 5 )

static const char ENCODE_LOOKUP[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
	'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
	'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8','9', '+', '/'
};

static const int8_t DECODE_LOOKUP[256] = {
	[0 ... 255] = -1,
	['A'] = 0, ['B'] = 1, ['C'] = 2, ['D'] = 3, ['E'] = 4, ['F'] = 5, ['G'] = 6, ['H'] = 7, ['I'] = 8,
	['J'] = 9, ['K'] = 10, ['L'] = 11, ['M'] = 12, ['N'] = 13, ['O'] = 14, ['P'] = 15, ['Q'] = 16,
	['R'] = 17, ['S'] = 18, ['T'] = 19, ['U'] = 20, ['V'] = 21, ['W'] = 22, ['X'] = 23, ['Y'] = 24,
	['Z'] = 25, ['a'] = 26, ['b'] = 27, ['c'] = 28, ['d'] = 29, ['e'] = 30, ['f'] = 31, ['g'] = 32,
	['h'] = 33, ['i'] = 34, ['j'] = 35, ['k'] = 36, ['l'] = 37, ['m'] = 38, ['n'] = 39, ['o'] = 40,
	['p'] = 41, ['q'] = 42, ['r'] = 43, ['s'] = 44, ['t'] = 45, ['u'] = 46, ['v'] = 47, ['w'] = 48,
	['x'] = 49, ['y'] = 50, ['z'] = 51, ['0'] = 52, ['1'] = 53, ['2'] = 54, ['3'] = 55, ['4'] = 56,
	['5'] = 57, ['6'] = 58, ['7'] = 59, ['8'] = 60, ['9'] = 61, ['+'] = 62, ['/'] = 63
};

static size_t encode_scalar(char* restrict out, const uint8_t* restrict in, size_t len) {
	size_t i = 0;
	size_t j = 0;

	while (i < len) {
		size_t start = i;

		// Take 3 bytes from input
		uint_fast8_t a = in[i++];
		uint_fast8_t b = __builtin_expect(i < len, 1) ? in[i++] : 0;
		uint_fast8_t c = __builtin_expect(i < len, 1) ? in[i++] : 0;

		// Save the number of remaining bytes
		size_t remain = i - start;

		// Pack them into 24-bit
		uint_fast32_t packed = ((uint_fast32_t)a << 16) | ((uint_fast32_t)b << 8) | (uint_fast32_t)c;

		// Split the packed 24-bit into 4 6-bit integers and convert them into Base64 character
		out[j++] = ENCODE_LOOKUP[(packed >> 18) & 0x3F];
		out[j++] = ENCODE_LOOKUP[(packed >> 12) & 0x3F];
		out[j++] = __builtin_expect(remain > 1, 1) ? ENCODE_LOOKUP[(packed >> 6) & 0x3F] : '='; // Use the number of remaining bytes to padding it
		out[j++] = __builtin_expect(remain > 2, 1) ? ENCODE_LOOKUP[packed & 0x3F] : '=';
	}

	return j;
}

/*static inline size_t encode_sse(char* restrict out, const uint8_t* restrict in, size_t len) {
	size_t i = 0;

	while (i < len) {
		__m128i input = _mm_loadu_si128((__m128i*)in);

		__m128i block = _mm_shuffle_epi8(input, ENCODE_BITMASK);

		//
	}
}*/

static size_t decode_scalar(uint8_t* restrict out, const char* restrict in, size_t len) {
	size_t i = 0;
	size_t j = 0;

	while (i < len) {
		size_t start = i;

		// Take 4 characters from input
		int_fast8_t a = DECODE_LOOKUP[(int)in[i++]];
		int_fast8_t b = DECODE_LOOKUP[(int)in[i++]];
		int_fast8_t c = __builtin_expect(i < len && in[i] != '=', 1) ? DECODE_LOOKUP[(int)in[i++]] : 0;
		int_fast8_t d = __builtin_expect(i < len && in[i] != '=', 1) ? DECODE_LOOKUP[(int)in[i++]] : 0;

		// Save the number of padding
		size_t remain = i - start;

		// Are The taken 4 character just zeros?
		if (__builtin_expect(a < 0 || b < 0 || c < 0 || d < 0, 0)) {
			return 0;
		}

		// Pack them into 24-bit
		uint_fast32_t packed = ((uint_fast32_t)a << 18) | ((uint_fast32_t)b << 12) | ((uint_fast32_t)c << 6) | (uint_fast32_t)d;

		// Split them into 3 raw bytes
		out[j++] = (packed >> 16) & 0xFF;
		if (__builtin_expect(remain > 2, 1)) out[j++] = (packed >> 8) & 0xFF;
		if (__builtin_expect(remain > 3, 1)) out[j++] = packed & 0xFF;
	}

	return j;
}

size_t bt_base64_encode(char* restrict out, const uint8_t* restrict in, size_t len) {
	_Static_assert(sizeof(ENCODE_LOOKUP) / sizeof(ENCODE_LOOKUP[0]) == 64, "bad encode lookup");

	return encode_scalar(out, in, len);
}

size_t bt_base64_decode(uint8_t* restrict out, const char* restrict in, size_t len) {
	return decode_scalar(out, in, len);
}