
#ifndef __EFFEKSEER_SIMD4F_H__
#define __EFFEKSEER_SIMD4F_H__

#include <cstdint>
#include <cmath>

#if defined(__ARM_NEON__) || defined(__ARM_NEON)
#include "Effekseer.SIMD4f_NEON.h"
#elif (defined(_M_AMD64) || defined(_M_X64)) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(__SSE__)
#include "Effekseer.SIMD4f_SSE.h"
#else
#include "Effekseer.SIMD4f_Gen.h"
#endif

#endif // __EFFEKSEER_SIMD4F_H__