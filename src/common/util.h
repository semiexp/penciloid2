#pragma once

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace penciloid
{
inline int PopCount(unsigned int n)
{
#ifdef _MSC_VER
	return __popcnt(n);
#else
#ifdef __GNUC__
	return __builtin_popcount(n);
#else
	n = (n & 0x55555555u) + ((n >>  1) & 0x55555555u);
	n = (n & 0x33333333u) + ((n >>  2) & 0x33333333u);
	n = (n & 0x0f0f0f0fu) + ((n >>  4) & 0x0f0f0f0fu);
	n = (n & 0x00ff00ffu) + ((n >>  8) & 0x00ff00ffu);
	n = (n & 0x0000ffffu) + ((n >> 16) & 0x0000ffffu);
	return n;
#endif
#endif
}
}