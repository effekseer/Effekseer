
#ifndef __EFFEKSEER_SIMD44F_H__
#define __EFFEKSEER_SIMD44F_H__

#if defined(__ARM_NEON__)
#include "Effekseer.SIMD44f_NEON.h"
#elif (defined(_M_IX86_FP) && _M_IX86_FP >= 2) || defined(__SSE__)
#include "Effekseer.SIMD44f_SSE.h"
#else
#include "Effekseer.SIMD44f_Gen.h"
#endif

#endif // __EFFEKSEER_SIMD4F_H__