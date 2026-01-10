// berytonite internal attribute

#pragma once

#if !defined(NOPTIMIZE)&&(defined(__GNUC__)||defined(__clang__))
	#define DO_PRAGMA(X) _Pragma(#X)
	#define PURE_ATT __attribute__((pure))
	#define CONST_ATT __attribute__((const, always_inline))
	#define ALIGNED(n) __attribute__((aligned(n)))
	#define UNROLL(n) DO_PRAGMA(GCC unroll n)
#elif !defined(NOPTIMIZE)&&defined(_MSC_VER)
	#define PURE_ATT
	#define CONST_ATT __forceinline __fastcall
	#define ALIGNED(n) __declspec(align(n))
	#define UNROLL(n) __pragma(loop(unroll(n)))
#else
	#define PURE_ATT
	#define CONST_ATT
	#define ALIGNED(n)
	#define UNROLL(n)
#endif

#if defined(__GNUC__)||defined(__clang__)
	#define DEPRECATED_ATT __attribute__((deprecated))
	#define VDEFAULT_ATT __attribute__((visibility("default")))
	#define VHIDDEN_ATT __attribute__((visibility("hidden")))
	#define NOINLINE_ATT __attribute__((noinline))
#elif defined(_MSC_VER)
	#define DEPRECATED_ATT __declspec(deprecated)
	#define VDEFAULT_ATT __declspec(dllexport)
	#define VHIDDEN_ATT __declspec(dllimport)
	#define NOINLINE_ATT __declspec(noinline)
#else
	#define DEPRECATED_ATT
	#define VDEFAULT_ATT
	#define VHIDDEN_ATT
#endif
