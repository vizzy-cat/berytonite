#include "hash_filestream.h"
#include <stdio.h>
#include <stdlib.h>

int bt_hash_file_digest(const char* path, bt_hash* hash_algo, uint8_t* restrict out) {
	// Check parameter
    if (!path || !hash_algo || !out) return -1;

    // Open file and check if there is an error
    FILE* fp = fopen(path, "rb");
    if (!fp) return -2;

    // Initialize the context
    bt_hash_ctx ctx;
    bt_hash_init(&ctx, hash_algo);

    // Start to read the entire file
    uint8_t buffer[4096];
    size_t size;
    while ((size = fread(buffer, 1, sizeof(buffer), fp))) {
        bt_hash_update(&ctx, buffer, size);
    }

    // Finalize and free context, and also close the file
    bt_hash_final(&ctx, out);
    bt_hash_free(&ctx);
    fclose(fp);
    return 0;
}

int bt_hash_file_update(bt_hash_ctx* restrict ctx, const char* path) {
	// Check parameter
    if (!ctx || !path) return -1;

    // Initialize the context
    FILE* fp = fopen(path, "rb");
    if (!fp) return -2;

    uint8_t buffer[4096];
    size_t size;
    while ((size = fread(buffer, 1, sizeof(buffer), fp))) {
        bt_hash_update(ctx, buffer, size); 
    }

    // Close the file
    fclose(fp);
    return 0;
}
