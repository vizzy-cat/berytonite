#include "hash_stream.h"
#include "internal/stream.h"
#include "util.h"
#include <stdlib.h>

void bt_hash_init(bt_hash_ctx* ctx, bt_hash* hash_algo) {
	if (!ctx || !hash_algo) return;

	ctx->internal_ctx = aligned_alloc(16, hash_algo->ctx_size);
	if (!ctx->internal_ctx) return;

	ctx->hash_algo = hash_algo;

	hash_algo->init(ctx->internal_ctx);
}

void bt_hash_update(bt_hash_ctx* restrict ctx, const uint8_t* restrict in, size_t len) {
	if(!ctx || !in || !len) return;

	ctx->hash_algo->update(ctx->internal_ctx, in, len);
}

void bt_hash_final(bt_hash_ctx* ctx, uint8_t* out) {
	if (!ctx || !out) return;

	ctx->hash_algo->final(ctx->internal_ctx, out);
}

void bt_hash_free(bt_hash_ctx* ctx) {
	if (!ctx) return;

	bt_memzero(ctx->internal_ctx, ctx->hash_algo->ctx_size);
	free(ctx->internal_ctx);
}