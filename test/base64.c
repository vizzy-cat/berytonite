#include <stdio.h>
#include <string.h>
#include "beryton/base64.h"

#define ASSERT_EQ_STR(expected, actual) \
	{ \
		if (strcmp(expected, actual) != 0) { \
			printf("FAILED: %s != %s\n", expected, actual); \
			return 1; \
		} \
	}

#define ASSERT_EQ_MEM(expected, actual, size) \
	{ \
		if (memcmp(expected, actual, size) != 0) { \
			printf("FAILED : memory mismatch (size = %i)", size); \
			return 1; \
		} \
	} \

int main() {
	// Base64 Encoding & Decoding Tester
	const char* input1 = "Man";
	char encoded[32];
	uint8_t decoded[32];

	// Case 1: Encoding test
	beryton_base64_encode(encoded, (void*)input1, strlen(input1));

	ASSERT_EQ_STR("TWFu", encoded);

	// Case 2: Decoding test
	beryton_base64_decode(decoded, encoded);

	ASSERT_EQ_MEM("Man", decoded, 3);

	// Case 3: Encoder padding test (1 byte)
	const char* input2 = "M";

	beryton_base64_encode(encoded, (void*)input2, strlen(input2));

	ASSERT_EQ_STR("TQ==", encoded);

	// Case 4: Decoder padding test (1 byte)
	beryton_base64_decode(decoded, encoded);

	ASSERT_EQ_MEM("M", decoded, 1);

	printf("All Base64 tests passed.\n");
	return 0;
}
