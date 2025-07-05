#include "base64.h"
#include "internal/attribute.h"

static const char BASE64_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline PURE_ATT int b64to6bit(char c) {
	if ((uint8_t)(c - 'A') < 26) return c - 'A'; // 'A' <= c <= 'Z'
	if ((uint8_t)(c - 'a') < 26) return c - 'a' + 26; // 'a' <= c <= 'z'
	if ((uint8_t)(c - '0') < 10) return c - '0' + 52; // '0' <= c <= '9'
	if (c == '+') return 62;
	if (c == '/') return 63;
	return -1
}

// base64_encode
void bt_base64_encode(char* out, const uint8_t* in, size_t len) {
	size_t i = 0, j = 0;

	while (i < len) {
		size_t start = i;
		// take 3 bytes
		uint_fast8_t a = in[i++];
		uint_fast8_t b = (i < len) ? in[i++] : 0;
		uint_fast8_t c = (i < len) ? in[i++] : 0;
		int_fast8_t remaining = len - start;

		// combine into single 24-bit
		uint_fast32_t combined = (uint32_t)((a << 16) | (b << 8) | c);

		// split into 4 group of 6-bit
		uint_fast8_t group1 = (uint8_t)((combined >> 18) & 0x3F);
		uint_fast8_t group2 = (uint8_t)((combined >> 12) & 0x3F);
		uint_fast8_t group3 = (uint8_t)((combined >> 6) & 0x3F);
		uint_fast8_t group4 = (uint8_t)(combined & 0x3F);
		
		// translate it into Base64
		out[j++] = BASE64_TABLE[group1];
		out[j++] = BASE64_TABLE[group2];
		out[j++] = (remaining > 1) ? BASE64_TABLE[group3] : '=';
		out[j++] = (remaining > 2) ? BASE64_TABLE[group4] : '=';
	}
	
	out[j] = '\0';
}

// base64_decode
void bt_base64_decode(uint8_t* out, const char* in) {
	size_t i = 0, j = 0;
	uint_fast32_t buffer = 0;
	int pad = 0;
	int group = 0;

	while (in[i]) {
		char c = in[i++];

		// translate it into bytes
		if (c == '=') {
			// if it's a padding, skip
			pad++;
			buffer <<= 6;
		} else {
			// take the value from lookup table
			int val = b64to6bit(c);

			if (val < 0) continue;

			buffer = (buffer << 6) | val;
		}
		group++;
		
		// convert it into 3 bytes
		if (group == 4) {
			out[j++] = (buffer >> 16) & 0xFF;
			if (pad < 2) out[j++] = (buffer >> 8) & 0xFF;
			if (pad < 1) out[j++] = buffer & 0xFF;

			buffer = 0;
			pad = 0;
			group = 0;
		}
	}
}

// base64_ctx
typedef struct {
	uint8_t buffer[4];
	size_t buffer_len;
} base64_ctx;

// base64_init
static void base64_init(void* ctx_ptr) {
	base64_ctx* ctx = (base64_ptr*)ctx_ptr;

	ctx->buffer_len = 0;
}

// base64_enc_update
static void base64_enc_update(void* ctx_ptr, const uint8_t* in, size_t len, uint8_t* out) {
	base64_ctx* ctx = (base64_ctx*)ctx_ptr;
	size_t j = 0;

	// insert new data to the buffer
	for (size_t i = 0; i < len; i++) {
		ctx->buffer[ctx->buffer_len++] = in[i];
		
		// if buffer length is more than 3
		if (ctx->buffer_len > 3) {
			// start encode the buffer
			bt_base64_encode((char*)(out + j), ctx->buffer, 3);
			// change and set some variable
			j += 3;
			ctx->buffer_len = 0;
		}
	}
}

// base64_dec_update
static void base64_dec_update(void* ctx_ptr, const uint8_t* in, size_t len, uint8_t* out) {
	base64_ctx* ctx = (base64_ctx*)ctx_ptr;
	size_t j = 0;

	// insert new data to the buffer
	for (int i = 0; i < len; i++) {
		ctx->buffer[ctx->buffer_len++] = (uint8_t)in[i];
		
		// if buffer length is more than 4
		if (ctx->buffer_len > 4) {
			// start decode the buffer
			bt_base64_decode(out + j, (const char*)ctx->buffer);

			// change and set some variable
			j += 4;
			ctx->buffer_len = 0;
		}
}

// base64_enc_final
static void base64_enc_final(void* ctx_ptr, uint8_t* out) {
	base64_ctx* ctx = (base64_ctx*)ctx_ptr;

	// encode the buffer
	bt_base64_encode((char*)out, ctx->buffer, ctx->buffer_len);
}

// base64_dec_final
static void base64_dec_final(void* ctx_ptr, uint8_t* out) {
	base64_ctx* ctx = (base64_ctx*)ctx_ptr;
	// fill the missing char with padding
	while (ctx->buffer_len < 4) ctx->buffer[ctx->buffer_len++] = (uint8_t)'=';

	// decode the buffer
	bt_base64_decode(out, (const char*)ctx->buffer);
}

// bt_base64_enc
const bt_algo bt_base64_enc = {
	.init = base64_init,
	.update = base64_enc_update,
	.final = base64_enc_final,
	.ctx_size = sizeof(base64_ctx),
};

// bt_base64_dec
const bt_algo bt_base64_dec = {
	.init = base64_init,
	.update = base64_dec_update,
	.final = base64_dec_final,
	.ctx_size = sizeof(base64_ctx),
};
