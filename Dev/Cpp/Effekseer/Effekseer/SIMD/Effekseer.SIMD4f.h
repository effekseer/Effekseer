
#ifndef __EFFEKSEER_SIMD4F_H__
#define __EFFEKSEER_SIMD4F_H__

#include <cstdint>
#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_NEON)
#include "Effekseer.SIMD4f_NEON.h"
#elif defined(EFK_SIMD_SSE2)
#include "Effekseer.SIMD4f_SSE.h"
#else
#include "Effekseer.SIMD4f_Gen.h"
#endif

#endif // __EFFEKSEER_SIMD4F_H__