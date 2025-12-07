#include "stream.h"
#include "internal/api.h"
#include <stdlib.h>

// init
void bt_init(bt_ctx* ctx, bt_algo* algo) {
	// check parameter
	if (!ctx || !algo) {
		return;
	}

	// allocate new memory for internal ctx and insert the algorithm into the ctx
	ctx->algo_ctx = (void*)malloc(algo->ctx_size);
	ctx->algo = algo;
	
	// initial the internal ctx
	ctx->algo->init(ctx->algo_ctx);
}

// update
void bt_update(bt_ctx* ctx, const uint8_t* in, size_t len, uint8_t* out /* some algorithm might not give anything */) {
	// check parameter
	if (!ctx || !in || !len) {
		return;
	}

	// update the ctx
	ctx->algo->update(ctx->algo_ctx, in, len, out);
}

// final
void bt_final(bt_ctx* ctx, uint8_t* out) {
	// check parameter
	if (!ctx || !out) {
		return;
	}

	// finalize the ctx and give the result
	ctx->algo->final(ctx->algo_ctx, out);
}

// free
void bt_free(bt_ctx* ctx) {
	// check parameter
	if (!ctx) {
		return;
	}

	// free the internal ctx
	free(ctx->algo_ctx);
}

// why do we even need these comments?
// these functions are very clear right? right?????