#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "beryton/sha256.h"
#include "beryton/util.h"

typedef struct {
    const char *input;
    const char *expected_hex;
} test_vector_t;

static const test_vector_t tests[] = {
    {
        "",
        "e3b0c44298fc1c149afbf4c8996fb924"
        "27ae41e4649b934ca495991b7852b855"
    },
    {
        "abc",
        "ba7816bf8f01cfea414140de5dae2223"
        "b00361a396177a9cb410ff61f20015ad"
    },
    {
        "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
        "248d6a61d20638b8e5c026930c3e6039"
        "a33ce45964ff2167f6ecedd419db06c1"
    },
    {
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "ffe054fe7ae0cb6dc65c3af9b61d5209"
        "f439851db43d0ba5997337df154668eb"
    }
};

int main(void) {
    uint8_t hash[BERYTON_SHA256_DIGEST_SIZE];
    char hex[BERYTON_SHA256_DIGEST_SIZE * 2 + 1];
    int fail = 0;

    for (size_t i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        const char *input = tests[i].input;
        const char *expected = tests[i].expected_hex;

        beryton_sha256_digest(hash, (const uint8_t *)input, strlen(input));
        beryton_bytetohex(hex, hash, BERYTON_SHA256_DIGEST_SIZE);

        if (strcmp(hex, expected) == 0) {
            printf("[PASS] Test %lu: input \"%s\"\n", i, input);
        } else {
            printf("[FAIL] Test %lu: input \"%s\"\n", i, input);
            printf("       Got:      %s\n", hex);
            printf("       Expected: %s\n", expected);
            fail++;
        }
    }

    if (fail == 0) {
        printf("All SHA-256 tests passed.\n");
        return 0;
    } else {
        printf("%d test(s) failed.\n", fail);
        return 1;
    }
}
