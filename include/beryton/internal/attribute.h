// berytonite internal attribute

#pragma once

#if defined(__GNUC__)||defined(__clang__)
	#define PURE_ATT __attribute__((pure))
	#define INLINE_ATT __attribute__((always_inline))
#else
	#define PURE_ATT
	#define INLINE_ATT
#endif
