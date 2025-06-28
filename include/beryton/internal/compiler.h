// berytonite internal compilance

#pragma once

#if defined(__GNUC__)||defined(__clang__)
	#define DO_PRAGMA(X) _Pragma(#X)
	#define UNROLL(n) DO_PRAGMA(GCC unroll n)
#elif defined(_MSC_VER)
	#define UNROLL(n) __pragma(loop(unroll(n)))
#else
	#define UNROLL(n)
#endif
