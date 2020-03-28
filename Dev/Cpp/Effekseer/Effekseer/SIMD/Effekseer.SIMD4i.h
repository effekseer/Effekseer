
#ifndef __EFFEKSEER_SIMD4I_H__
#define __EFFEKSEER_SIMD4I_H__

#include <cstdint>
#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_NEON)
#include "Effekseer.SIMD4i_NEON.h"
#elif defined(EFK_SIMD_SSE2)
#include "Effekseer.SIMD4i_SSE.h"
#else
#include "Effekseer.SIMD4i_Gen.h"
#endif

#endif // __EFFEKSEER_SIMD4I_H__