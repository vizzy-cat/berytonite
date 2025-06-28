// berytonite internal attribute

#pragma once

#if defined(__GNUC__)||defined(__clang__)
	#define PURE_ATT __attribute__((pure))
	#define INLINE_ATT __attribute__((always_inline))
	#define ZERO_OPTIMIZE_ATT __attribute__((optimize("O0")))
#else
	#define PURE_ATT
	#define INLINE_ATT
#endif
