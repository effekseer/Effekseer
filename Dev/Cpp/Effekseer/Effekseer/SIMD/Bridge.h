
#ifndef __EFFEKSEER_SIMD_BRIDGE_H__
#define __EFFEKSEER_SIMD_BRIDGE_H__

#include "Base.h"
#include <cstdint>

#if defined(EFK_SIMD_NEON)
#include "Bridge_NEON.h"
#elif defined(EFK_SIMD_SSE2)
#include "Bridge_SSE.h"
#else
#include "Bridge_Gen.h"
#endif

#endif // __EFFEKSEER_SIMD_BRIDGE_H__