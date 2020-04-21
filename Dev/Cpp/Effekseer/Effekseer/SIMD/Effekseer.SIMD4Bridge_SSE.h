#ifndef __EFFEKSEER_SIMD4BRIDGE_SSE_H__
#define __EFFEKSEER_SIMD4BRIDGE_SSE_H__

#include "Effekseer.SIMD4f_SSE.h"
#include "Effekseer.SIMD4i_SSE.h"
#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_SSE2)

namespace Effekseer
{

inline SIMD4i SIMD4f::Convert4i() const { return _mm_cvtps_epi32(s); }

inline SIMD4i SIMD4f::Cast4i() const { return _mm_castps_si128(s); }

inline SIMD4f SIMD4i::Convert4f() const { return _mm_cvtepi32_ps(s); }

inline SIMD4f SIMD4i::Cast4f() const { return _mm_castsi128_ps(s); }

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SIMD4F_SSE_H__
