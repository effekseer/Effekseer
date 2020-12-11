#ifndef __EFFEKSEER_SIMD4F_NEON_H__
#define __EFFEKSEER_SIMD4F_NEON_H__

#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_NEON)

namespace Effekseer
{

inline float Sqrt(float x)
{
	return sqrt(x);
}

inline float Rsqrt(float x)
{
	return 1.0f / sqrt(x);
}

struct SIMD4i;

/**
 @brief    simd class for sse
 */

struct alignas(16) SIMD4f
{
	float32x4_t s;
	
	SIMD4f() = default;
	SIMD4f(const SIMD4f& rhs) = default;
	SIMD4f(float32x4_t rhs) { s = rhs; }
	SIMD4f(uint32x4_t rhs) { s = vreinterpretq_f32_u32(rhs); }
	SIMD4f(float x, float y, float z, float w) { const float f[4] = {x, y, z, w}; s = vld1q_f32(f); }
	SIMD4f(float i) { s = vdupq_n_f32(i); }
	
	float GetX() const { return vgetq_lane_f32(s, 0); }
	float GetY() const { return vgetq_lane_f32(s, 1); }
	float GetZ() const { return vgetq_lane_f32(s, 2); }
	float GetW() const { return vgetq_lane_f32(s, 3); }
	
	void SetX(float i) { s = vsetq_lane_f32(i, s, 0); }
	void SetY(float i) { s = vsetq_lane_f32(i, s, 1); }
	void SetZ(float i) { s = vsetq_lane_f32(i, s, 2); }
	void SetW(float i) { s = vsetq_lane_f32(i, s, 3); }
	
	template <size_t LANE>
	SIMD4f Dup();
	
	SIMD4i Convert4i() const;
	SIMD4i Cast4i() const;
	
	SIMD4f& operator+=(const SIMD4f& rhs);
	SIMD4f& operator-=(const SIMD4f& rhs);
	SIMD4f& operator*=(const SIMD4f& rhs);
	SIMD4f& operator*=(float rhs);
	SIMD4f& operator/=(const SIMD4f& rhs);
	SIMD4f& operator/=(float rhs);
	
	static SIMD4f Load2(const void* mem);
	static void Store2(void* mem, const SIMD4f& i);
	static SIMD4f Load3(const void* mem);
	static void Store3(void* mem, const SIMD4f& i);
	static SIMD4f Load4(const void* mem);
	static void Store4(void* mem, const SIMD4f& i);
	
	static SIMD4f SetZero();
	static SIMD4f SetInt(int32_t x, int32_t y, int32_t z, int32_t w);
	static SIMD4f SetUInt(uint32_t x, uint32_t y, uint32_t z, uint32_t w);
	static SIMD4f Sqrt(const SIMD4f& in);
	static SIMD4f Rsqrt(const SIMD4f& in);
	static SIMD4f Abs(const SIMD4f& in);
	static SIMD4f Min(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f Max(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f Floor(const SIMD4f& in);
	static SIMD4f Ceil(const SIMD4f& in);
	static SIMD4f MulAdd(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);
	static SIMD4f MulSub(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);
	
	template<size_t LANE>
	static SIMD4f MulLane(const SIMD4f& lhs, const SIMD4f& rhs);
	template<size_t LANE>
	static SIMD4f MulAddLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);
	template<size_t LANE>
	static SIMD4f MulSubLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c);
	template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
	static SIMD4f Swizzle(const SIMD4f& v);
	
	static SIMD4f Dot3(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f Cross3(const SIMD4f& lhs, const SIMD4f& rhs);
	
	template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
	static SIMD4f Mask();
	static uint32_t MoveMask(const SIMD4f& in);
	static SIMD4f Select(const SIMD4f& mask, const SIMD4f& sel1, const SIMD4f& sel2);
	static SIMD4f Equal(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f NotEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f LessThan(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f LessEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f GreaterThan(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f GreaterEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f NearEqual(const SIMD4f& lhs, const SIMD4f& rhs, float epsilon = DefaultEpsilon);
	static SIMD4f IsZero(const SIMD4f& in, float epsilon = DefaultEpsilon);
	static void Transpose(SIMD4f& s0, SIMD4f& s1, SIMD4f& s2, SIMD4f& s3);
	
private:
	static SIMD4f SwizzleYZX(const SIMD4f& in);
	static SIMD4f SwizzleZXY(const SIMD4f& in);
};

} // namespace Effekseer

namespace Effekseer
{

template <size_t LANE>
SIMD4f SIMD4f::Dup()
{
	return (LANE < 2) ?
		vdupq_lane_f32(vget_low_f32(s), LANE) :
		vdupq_lane_f32(vget_high_f32(s), LANE & 1);
}

inline SIMD4f operator+(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vaddq_f32(lhs.s, rhs.s);
}

inline SIMD4f operator-(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vsubq_f32(lhs.s, rhs.s);
}

inline SIMD4f operator*(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vmulq_f32(lhs.s, rhs.s);
}

inline SIMD4f operator*(const SIMD4f& lhs, float rhs)
{
	return vmulq_n_f32(lhs.s, rhs);
}

inline SIMD4f operator/(const SIMD4f& lhs, const SIMD4f& rhs)
{
#if defined(_M_ARM64) || __aarch64__
	return vdivq_f32(lhs.s, rhs.s);
#else
	float32x4_t recp = vrecpeq_f32(rhs.s);
	float32x4_t s = vrecpsq_f32(recp, rhs.s);
	recp = vmulq_f32(s, recp);
	s = vrecpsq_f32(recp, rhs.s);
	recp = vmulq_f32(s, recp);
	return vmulq_f32(lhs.s, recp);
#endif
}

inline SIMD4f operator/(const SIMD4f& lhs, float rhs)
{
	return lhs * (1.0f / rhs);
}

inline SIMD4f operator&(const SIMD4f& lhs, const SIMD4f& rhs)
{
	uint32x4_t lhsi = vreinterpretq_u32_f32(lhs.s);
	uint32x4_t rhsi = vreinterpretq_u32_f32(rhs.s);
	return vreinterpretq_f32_u32(vandq_u32(lhsi, rhsi));
}

inline SIMD4f operator|(const SIMD4f& lhs, const SIMD4f& rhs)
{
	uint32x4_t lhsi = vreinterpretq_u32_f32(lhs.s);
	uint32x4_t rhsi = vreinterpretq_u32_f32(rhs.s);
	return vreinterpretq_f32_u32(vorrq_u32(lhsi, rhsi));
}

inline SIMD4f operator^(const SIMD4f& lhs, const SIMD4f& rhs)
{
	uint32x4_t lhsi = vreinterpretq_u32_f32(lhs.s);
	uint32x4_t rhsi = vreinterpretq_u32_f32(rhs.s);
	return vreinterpretq_f32_u32(veorq_u32(lhsi, rhsi));
}

inline bool operator==(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f::MoveMask(SIMD4f::Equal(lhs, rhs)) == 0xf;
}

inline bool operator!=(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f::MoveMask(SIMD4f::Equal(lhs, rhs)) != 0xf;
}

inline SIMD4f& SIMD4f::operator+=(const SIMD4f& rhs) { return *this = *this + rhs; }
inline SIMD4f& SIMD4f::operator-=(const SIMD4f& rhs) { return *this = *this - rhs; }
inline SIMD4f& SIMD4f::operator*=(const SIMD4f& rhs) { return *this = *this * rhs; }
inline SIMD4f& SIMD4f::operator*=(float rhs) { return *this = *this * rhs; }
inline SIMD4f& SIMD4f::operator/=(const SIMD4f& rhs) { return *this = *this / rhs; }
inline SIMD4f& SIMD4f::operator/=(float rhs) { return *this = *this / rhs; }

inline SIMD4f SIMD4f::Load2(const void* mem)
{
	float32x2_t low = vld1_f32((const float*)mem);
	float32x2_t high = vdup_n_f32(0.0f);
	return vcombine_f32(low, high);
}

inline void SIMD4f::Store2(void* mem, const SIMD4f& i)
{
	vst1_f32((float*)mem, vget_low_f32(i.s));
}

inline SIMD4f SIMD4f::Load3(const void* mem)
{
	float32x2_t low = vld1_f32((const float*)mem);
	float32x2_t high = vld1_lane_f32((const float*)mem + 2, vdup_n_f32(0.0f), 0);
	return vcombine_f32(low, high);
}

inline void SIMD4f::Store3(void* mem, const SIMD4f& i)
{
	vst1_f32((float*)mem, vget_low_f32(i.s));
	vst1q_lane_f32((float*)mem + 2, i.s, 2);
}

inline SIMD4f SIMD4f::Load4(const void* mem)
{
	return vld1q_f32((const float*)mem);
}

inline void SIMD4f::Store4(void* mem, const SIMD4f& i)
{
	vst1q_f32((float*)mem, i.s);
}

inline SIMD4f SIMD4f::SetZero()
{
	return vdupq_n_f32(0.0f);
}

inline SIMD4f SIMD4f::SetInt(int32_t x, int32_t y, int32_t z, int32_t w)
{
	const int32_t i[4] = {x, y, z, w};
	return vreinterpretq_f32_s32(vld1q_s32(i));
}

inline SIMD4f SIMD4f::SetUInt(uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
	const uint32_t i[4] = {x, y, z, w};
	return vreinterpretq_u32_f32(vld1q_u32(i));
}

inline SIMD4f SIMD4f::Sqrt(const SIMD4f& in)
{
#if defined(_M_ARM64) || __aarch64__
	return vsqrtq_f32(in.s);
#else
	return SIMD4f(1.0f) / SIMD4f::Rsqrt(in);
#endif
}

inline SIMD4f SIMD4f::Rsqrt(const SIMD4f& in)
{
	float32x4_t s0 = vrsqrteq_f32(in.s);
	float32x4_t p0 = vmulq_f32(in.s, s0);
	float32x4_t r0 = vrsqrtsq_f32(p0, s0);
	float32x4_t s1 = vmulq_f32(s0, r0);
	return s1;
}

inline SIMD4f SIMD4f::Abs(const SIMD4f& in)
{
	return vabsq_f32(in.s);
}

inline SIMD4f SIMD4f::Min(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vminq_f32(lhs.s, rhs.s);
}

inline SIMD4f SIMD4f::Max(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vmaxq_f32(lhs.s, rhs.s);
}

inline SIMD4f SIMD4f::Floor(const SIMD4f& in)
{
#if defined(_M_ARM64) || __aarch64__
	return vrndmq_f32(in.s);
#else
	int32x4_t in_i = vcvtq_s32_f32(in.s);
	float32x4_t result = vcvtq_f32_s32(in_i);
	float32x4_t larger = vcgtq_f32(result, in.s);
	larger = vcvtq_f32_s32(larger);
	return vaddq_f32(result, larger);
#endif
}

inline SIMD4f SIMD4f::Ceil(const SIMD4f& in)
{
#if defined(_M_ARM64) || __aarch64__
	return vrndpq_f32(in.s);
#else
	int32x4_t in_i = vcvtq_s32_f32(in.s);
	float32x4_t result = vcvtq_f32_s32(in_i);
	float32x4_t smaller = vcltq_f32(result, in.s);
	smaller = vcvtq_f32_s32(smaller);
	return vsubq_f32(result, smaller);
#endif
}

inline SIMD4f SIMD4f::MulAdd(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	return vmlaq_f32(a.s, b.s, c.s);
}

inline SIMD4f SIMD4f::MulSub(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	return vmlsq_f32(a.s, b.s, c.s);
}

template<size_t LANE>
inline SIMD4f SIMD4f::MulLane(const SIMD4f& lhs, const SIMD4f& rhs)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	float32x2_t rhs2 = (LANE < 2) ? vget_low_f32(rhs.s) : vget_high_f32(rhs.s);
	return vmulq_lane_f32(lhs.s, rhs2, LANE & 1);
}

template<size_t LANE>
inline SIMD4f SIMD4f::MulAddLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	float32x2_t c2 = (LANE < 2) ? vget_low_f32(c.s) : vget_high_f32(c.s);
	return vmlaq_lane_f32(a.s, b.s, c2, LANE & 1);
}

template<size_t LANE>
inline SIMD4f SIMD4f::MulSubLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	float32x2_t c2 = (LANE < 2) ? vget_low_f32(c.s) : vget_high_f32(c.s);
	return vmlsq_lane_f32(a.s, b.s, c2, LANE & 1);
}

//template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
//inline SIMD4f SIMD4f::Swizzle(const SIMD4f& v)
//{
//	static_assert(indexX < 4, "indexX is must be less than 4.");
//	static_assert(indexY < 4, "indexY is must be less than 4.");
//	static_assert(indexZ < 4, "indexZ is must be less than 4.");
//	static_assert(indexW < 4, "indexW is must be less than 4.");
//}

inline SIMD4f SIMD4f::Dot3(const SIMD4f& lhs, const SIMD4f& rhs)
{
	float32x4_t mul = vmulq_f32(lhs.s, rhs.s);
	float32x2_t xy = vpadd_f32(vget_low_f32(mul), vget_low_f32(mul));
	float32x2_t dot = vadd_f32(xy, vget_high_f32(mul));
	return vcombine_f32(dot, vdup_n_f32(0.0f));
}

inline SIMD4f SIMD4f::Cross3(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return MulSub(SwizzleYZX(lhs.s) * SwizzleZXY(rhs.s), SwizzleZXY(lhs.s), SwizzleYZX(rhs.s));
}

template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
inline SIMD4f SIMD4f::Mask()
{
	static_assert(X >= 2, "indexX is must be set 0 or 1.");
	static_assert(Y >= 2, "indexY is must be set 0 or 1.");
	static_assert(Z >= 2, "indexZ is must be set 0 or 1.");
	static_assert(W >= 2, "indexW is must be set 0 or 1.");
	const uint32_t in[4] = {0xffffffff * X, 0xffffffff * Y, 0xffffffff * Z, 0xffffffff * W};
	return vld1q_f32((const float*)in);
}

inline uint32_t SIMD4f::MoveMask(const SIMD4f& in)
{
	uint16x4_t u16x4 = vmovn_u32(vreinterpretq_u32_f32(in.s));
	uint16_t u16[4];
	vst1_u16(u16, u16x4);
	return (u16[0] & 1) | (u16[1] & 2) | (u16[2] & 4) | (u16[3] & 8);
}

inline SIMD4f SIMD4f::Select(const SIMD4f& mask, const SIMD4f& sel1, const SIMD4f& sel2)
{
	uint32x4_t maski = vreinterpretq_u32_f32(mask.s);
	return vbslq_f32(maski, sel1.s, sel2.s);
}

inline SIMD4f SIMD4f::Equal(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vceqq_f32(lhs.s, rhs.s);
}

inline SIMD4f SIMD4f::NotEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vmvnq_u32(vceqq_f32(lhs.s, rhs.s));
}

inline SIMD4f SIMD4f::LessThan(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vcltq_f32(lhs.s, rhs.s);
}

inline SIMD4f SIMD4f::LessEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vcleq_f32(lhs.s, rhs.s);
}

inline SIMD4f SIMD4f::GreaterThan(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vcgtq_f32(lhs.s, rhs.s);
}

inline SIMD4f SIMD4f::GreaterEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return vcgeq_f32(lhs.s, rhs.s);
}

inline SIMD4f SIMD4f::NearEqual(const SIMD4f& lhs, const SIMD4f& rhs, float epsilon)
{
	return LessEqual(Abs(lhs - rhs), SIMD4f(epsilon));
}

inline SIMD4f SIMD4f::IsZero(const SIMD4f& in, float epsilon)
{
	return LessEqual(Abs(in), SIMD4f(epsilon));
}

inline void SIMD4f::Transpose(SIMD4f& s0, SIMD4f& s1, SIMD4f& s2, SIMD4f& s3)
{
	float32x4x2_t t0 = vzipq_f32(s0.s, s2.s);
	float32x4x2_t t1 = vzipq_f32(s1.s, s3.s);
	float32x4x2_t t2 = vzipq_f32(t0.val[0], t1.val[0]);
	float32x4x2_t t3 = vzipq_f32(t0.val[1], t1.val[1]);
	
	s0 = t2.val[0];
	s1 = t2.val[1];
	s2 = t3.val[0];
	s3 = t3.val[1];
}

inline SIMD4f SIMD4f::SwizzleYZX(const SIMD4f& in)
{
	float32x4_t ex = vextq_f32(in.s, in.s, 1);
	return vsetq_lane_f32(vgetq_lane_f32(ex, 3), ex, 2);
}

inline SIMD4f SIMD4f::SwizzleZXY(const SIMD4f& in)
{
	float32x4_t ex = vextq_f32(in.s, in.s, 3);
	return vsetq_lane_f32(vgetq_lane_f32(ex, 3), ex, 0);
}

} // namespace Effekseer

#endif
#endif // __EFFEKSEER_SIMD4F_NEON_H__
