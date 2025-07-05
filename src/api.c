#include "api.h"
#include "internal/api.h"
#include <stdlib.h>

void bt_init(bt_ctx* ctx, bt_algo* algo) {
	ctx->algo_ctx = (void*)malloc(algo->ctx_size);
	ctx->algo = algo;
	
	ctx->algo->init(ctx->algo_ctx);
}

void bt_update(bt_ctx* ctx, const uint8_t* in, size_t len, uint8_t* out) {
	ctx->algo->update(ctx->algo_ctx, in, len, out);
}

void bt_final(bt_ctx* ctx, uint8_t* out) {
	ctx->algo->final(ctx->algo_ctx, out);
}

void bt_free(bt_ctx* ctx) {
	free(ctx->algo_ctx);
}

inline void bt_reuse(bt_ctx* ctx) {
	bt_free(ctx);
	bt_init(ctx, ctx->algo);
}
