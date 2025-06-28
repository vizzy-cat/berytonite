// berytonite internal attribute

#pragma once

#if defined(__GNUC__)||defined(__clang__)
   #define PURE_ATT __attribute__((pure))
#else
   #define PURE_ATT
#endif
