#ifndef __EFFEKSEER_SIMD4BRIDGE_GEN_H__
#define __EFFEKSEER_SIMD4BRIDGE_GEN_H__

#include "Effekseer.SIMD4f_Gen.h"
#include "Effekseer.SIMD4i_Gen.h"
#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_GEN)

namespace Effekseer
{

inline SIMD4i SIMD4f::Convert4i() const { return SIMD4i((int32_t)vf[0], (int32_t)vf[1], (int32_t)vf[2], (int32_t)vf[3]); }

inline SIMD4i SIMD4f::Cast4i() const { return SIMD4i(vu[0], vu[1], vu[2], vu[3]); }

inline SIMD4f SIMD4i::Convert4f() const { return SIMD4f((float)vi[0], (float)vi[1], (float)vi[2], (float)vi[3]); }

inline SIMD4f SIMD4i::Cast4f() const { return SIMD4f(vf[0], vf[1], vf[2], vf[3]); }

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SIMD4F_GEN_H__