#ifndef __EFFEKSEER_SIMD4BRIDGE_NEON_H__
#define __EFFEKSEER_SIMD4BRIDGE_NEON_H__

#include "Effekseer.SIMD4f_NEON.h"
#include "Effekseer.SIMD4i_NEON.h"
#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_NEON)

namespace Effekseer
{

inline SIMD4i SIMD4f::Convert4i() const { return vcvtq_s32_f32(s); }

inline SIMD4i SIMD4f::Cast4i() const { return vreinterpretq_s32_f32(s); }

inline SIMD4f SIMD4i::Convert4f() const { return vcvtq_f32_s32(s); }

inline SIMD4f SIMD4i::Cast4f() const { return vreinterpretq_f32_s32(s); }

} // namespace Effekseer

#endif
#endif // __EFFEKSEER_SIMD4F_NEON_H__
