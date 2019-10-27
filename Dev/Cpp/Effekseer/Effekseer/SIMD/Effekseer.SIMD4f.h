
#ifndef __EFFEKSEER_SIMD4F_H__
#define __EFFEKSEER_SIMD4F_H__

#if defined(__ARM_NEON__)
#include "Effekseer.SIMD4f_NEON.h"
#elif (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(__SSE__)
#include "Effekseer.SIMD4f_SSE.h"
#else
#include "Effekseer.SIMD4f.h"
#endif

#endif // __EFFEKSEER_SIMD4F_H__