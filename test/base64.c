#include <stdio.h>
#include <string.h>
#include "krypton/base64.h"
#include "krypton/error.h"

#define ASSERT_EQ_STR(expected, actual) \
	{ \
		if (strcmp(expected, actual) != 0) { \
			printf("FAILED: %s != %s\n", expected, actual); \
			return 1; \
		} \
	}

int main() {
	// Base64 Encoding & Decoding Tester
	const char* input1 = "Man";
	char encoded[32];
	char decoded[32];

	// Case 1: Encoding test
	if (krypton_base64_encode(encoded, (void*)input1, strlen(input1)) == 0) {
		printf("Encode 1 failed : %i\n", krypton_get_error());
		return 1;
	}

	ASSERT_EQ_STR("TWFu", encoded);

	// Case 2: Decoding test
	if (krypton_base64_decode((void*)decoded, encoded, strlen(encoded)) == 0) {
		printf("Decode 2 failed : %i\n", krypton_get_error());
		return 1;
	}

	ASSERT_EQ_STR("Man", decoded);

	// Case 3: Encoder padding test (1 byte)
	const char* input2 = "M";

	if (krypton_base64_encode(encoded, (void*)input2, strlen(input2)) == 0) {
		printf("Encode 3 failed : %i\n", krypton_get_error());
		return 1;
	}

	ASSERT_EQ_STR("TQ==", encoded);

	// Case 4: Decoder padding test (1 byte)
	if (krypton_base64_decode(decoded, encoded, strlen(encoded)) == 0) {
		printf("Decode 4 failed : %i\n", krypton_get_error());
		return 1;
	}

	ASSERT_EQ_STR("M", decoded);

	printf("All Base64 tests passed.\n");
	return 0;
}
