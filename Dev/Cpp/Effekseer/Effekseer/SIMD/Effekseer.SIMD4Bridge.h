
#ifndef __EFFEKSEER_SIMD4BRIDGE_H__
#define __EFFEKSEER_SIMD4BRIDGE_H__

#include <cstdint>
#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_NEON)
#include "Effekseer.SIMD4Bridge_NEON.h"
#elif defined(EFK_SIMD_SSE2)
#include "Effekseer.SIMD4Bridge_SSE.h"
#else
#include "Effekseer.SIMD4Bridge_Gen.h"
#endif

#endif // __EFFEKSEER_SIMD4F_H__