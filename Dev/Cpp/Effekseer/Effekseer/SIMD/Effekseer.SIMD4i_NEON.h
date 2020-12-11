
#ifndef __EFFEKSEER_SIMD4I_NEON_H__
#define __EFFEKSEER_SIMD4I_NEON_H__

#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_NEON)

namespace Effekseer
{

struct SIMD4f;

/**
 @brief    simd class for sse
 */

struct alignas(16) SIMD4i
{
	int32x4_t s;
	
	SIMD4i() = default;
	SIMD4i(const SIMD4i& rhs) = default;
	SIMD4i(int32x4_t rhs) { s = rhs; }
	SIMD4i(int32_t x, int32_t y, int32_t z, int32_t w) { const int32_t v[4] = {x, y, z, w}; s = vld1q_s32(v); }
	SIMD4i(int32_t i) { s = vdupq_n_s32(i); }
	
	int32_t GetX() const { return vgetq_lane_s32(s, 0); }
	int32_t GetY() const { return vgetq_lane_s32(s, 1); }
	int32_t GetZ() const { return vgetq_lane_s32(s, 2); }
	int32_t GetW() const { return vgetq_lane_s32(s, 3); }
	
	void SetX(int32_t i) { s = vsetq_lane_s32(i, s, 0); }
	void SetY(int32_t i) { s = vsetq_lane_s32(i, s, 1); }
	void SetZ(int32_t i) { s = vsetq_lane_s32(i, s, 2); }
	void SetW(int32_t i) { s = vsetq_lane_s32(i, s, 3); }
	
	SIMD4f Convert4f() const;
	SIMD4f Cast4f() const;
	
	SIMD4i& operator+=(const SIMD4i& rhs);
	SIMD4i& operator-=(const SIMD4i& rhs);
	SIMD4i& operator*=(const SIMD4i& rhs);
	SIMD4i& operator*=(int32_t rhs);
	SIMD4i& operator/=(const SIMD4i& rhs);
	SIMD4i& operator/=(int32_t rhs);
	
	static SIMD4i Load2(const void* mem);
	static void Store2(void* mem, const SIMD4i& i);
	static SIMD4i Load3(const void* mem);
	static void Store3(void* mem, const SIMD4i& i);
	static SIMD4i Load4(const void* mem);
	static void Store4(void* mem, const SIMD4i& i);
	
	static SIMD4i SetZero();
	static SIMD4i Abs(const SIMD4i& in);
	static SIMD4i Min(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i Max(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i MulAdd(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);
	static SIMD4i MulSub(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);
	
	template<size_t LANE>
	static SIMD4i MulLane(const SIMD4i& lhs, const SIMD4i& rhs);
	template<size_t LANE>
	static SIMD4i MulAddLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);
	template<size_t LANE>
	static SIMD4i MulSubLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c);
	template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
	static SIMD4i Swizzle(const SIMD4i& v);
	
	template <int COUNT>
	static SIMD4i ShiftL(const SIMD4i& in);
	template <int COUNT>
	static SIMD4i ShiftR(const SIMD4i& in);
	template <int COUNT>
	static SIMD4i ShiftRA(const SIMD4i& in);
	
	template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
	static SIMD4i Mask();
	static uint32_t MoveMask(const SIMD4i& in);
	static SIMD4i Equal(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i NotEqual(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i LessThan(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i LessEqual(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i GreaterThan(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i GreaterEqual(const SIMD4i& lhs, const SIMD4i& rhs);
	static SIMD4i NearEqual(const SIMD4i& lhs, const SIMD4i& rhs, int32_t epsilon = DefaultEpsilon);
	static SIMD4i IsZero(const SIMD4i& in, int32_t epsilon = DefaultEpsilon);
	static void Transpose(SIMD4i& s0, SIMD4i& s1, SIMD4i& s2, SIMD4i& s3);
	
private:
	static SIMD4i SwizzleYZX(const SIMD4i& in);
	static SIMD4i SwizzleZXY(const SIMD4i& in);
};

} // namespace Effekseer

namespace Effekseer
{

inline SIMD4i operator+(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vaddq_s32(lhs.s, rhs.s);
}

inline SIMD4i operator-(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vsubq_s32(lhs.s, rhs.s);
}

inline SIMD4i operator*(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vmulq_s32(lhs.s, rhs.s);
}

inline SIMD4i operator*(const SIMD4i& lhs, int32_t rhs)
{
	return vmulq_n_s32(lhs.s, rhs);
}

inline SIMD4i operator/(const SIMD4i& lhs, const SIMD4i& rhs)
{
#if defined(EFK_NEON_ARM64)
	return vdivq_s32(lhs.s, rhs.s);
#else
	return SIMD4i(
		lhs.GetX() / rhs.GetX(),
		lhs.GetY() / rhs.GetY(),
		lhs.GetZ() / rhs.GetZ(),
		lhs.GetW() / rhs.GetW());
#endif
}

inline SIMD4i operator/(const SIMD4i& lhs, int32_t rhs)
{
	return lhs * (1.0f / rhs);
}

inline SIMD4i operator&(const SIMD4i& lhs, const SIMD4i& rhs)
{
	uint32x4_t lhsi = vreinterpretq_u32_s32(lhs.s);
	uint32x4_t rhsi = vreinterpretq_u32_s32(rhs.s);
	return vreinterpretq_s32_u32(vandq_u32(lhsi, rhsi));
}

inline SIMD4i operator|(const SIMD4i& lhs, const SIMD4i& rhs)
{
	uint32x4_t lhsi = vreinterpretq_u32_s32(lhs.s);
	uint32x4_t rhsi = vreinterpretq_u32_s32(rhs.s);
	return vreinterpretq_s32_u32(vorrq_u32(lhsi, rhsi));
}

inline bool operator==(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i::MoveMask(SIMD4i::Equal(lhs, rhs)) == 0xf;
}

inline bool operator!=(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i::MoveMask(SIMD4i::Equal(lhs, rhs)) != 0xf;
}

inline SIMD4i& SIMD4i::operator+=(const SIMD4i& rhs) { return *this = *this + rhs; }
inline SIMD4i& SIMD4i::operator-=(const SIMD4i& rhs) { return *this = *this - rhs; }
inline SIMD4i& SIMD4i::operator*=(const SIMD4i& rhs) { return *this = *this * rhs; }
inline SIMD4i& SIMD4i::operator*=(int32_t rhs) { return *this = *this * rhs; }
inline SIMD4i& SIMD4i::operator/=(const SIMD4i& rhs) { return *this = *this / rhs; }
inline SIMD4i& SIMD4i::operator/=(int32_t rhs) { return *this = *this / rhs; }

inline SIMD4i SIMD4i::Load2(const void* mem)
{
	int32x2_t low = vld1_s32((const int32_t*)mem);
	int32x2_t high = vdup_n_s32(0.0f);
	return vcombine_s32(low, high);
}

inline void SIMD4i::Store2(void* mem, const SIMD4i& i)
{
	vst1_s32((int32_t*)mem, vget_low_s32(i.s));
}

inline SIMD4i SIMD4i::Load3(const void* mem)
{
	int32x2_t low = vld1_s32((const int32_t*)mem);
	int32x2_t high = vld1_lane_s32((const int32_t*)mem + 2, vdup_n_s32(0.0f), 0);
	return vcombine_s32(low, high);
}

inline void SIMD4i::Store3(void* mem, const SIMD4i& i)
{
	vst1_s32((int32_t*)mem, vget_low_s32(i.s));
	vst1q_lane_s32((int32_t*)mem + 2, i.s, 2);
}

inline SIMD4i SIMD4i::Load4(const void* mem)
{
	return vld1q_s32((const int32_t*)mem);
}

inline void SIMD4i::Store4(void* mem, const SIMD4i& i)
{
	vst1q_s32((int32_t*)mem, i.s);
}

inline SIMD4i SIMD4i::SetZero()
{
	return vdupq_n_s32(0.0f);
}

inline SIMD4i SIMD4i::Abs(const SIMD4i& in)
{
	return vabsq_s32(in.s);
}

inline SIMD4i SIMD4i::Min(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vminq_s32(lhs.s, rhs.s);
}

inline SIMD4i SIMD4i::Max(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vmaxq_s32(lhs.s, rhs.s);
}

inline SIMD4i SIMD4i::MulAdd(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	return vmlaq_s32(a.s, b.s, c.s);
}

inline SIMD4i SIMD4i::MulSub(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	return vmlsq_s32(a.s, b.s, c.s);
}

template<size_t LANE>
inline SIMD4i SIMD4i::MulLane(const SIMD4i& lhs, const SIMD4i& rhs)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	int32x2_t rhs2 = (LANE < 2) ? vget_low_s32(rhs.s) : vget_high_s32(rhs.s);
	return vmulq_lane_s32(lhs.s, rhs2, LANE & 1);
}

template<size_t LANE>
inline SIMD4i SIMD4i::MulAddLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	int32x2_t c2 = (LANE < 2) ? vget_low_s32(c.s) : vget_high_s32(c.s);
	return vmlaq_lane_s32(a.s, b.s, c2, LANE & 1);
}

template<size_t LANE>
inline SIMD4i SIMD4i::MulSubLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	int32x2_t c2 = (LANE < 2) ? vget_low_s32(c.s) : vget_high_s32(c.s);
	return vmlsq_lane_s32(a.s, b.s, c2, LANE & 1);
}

//template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
//inline SIMD4i SIMD4i::Swizzle(const SIMD4i& v)
//{
//	static_assert(indexX < 4, "indexX is must be less than 4.");
//	static_assert(indexY < 4, "indexY is must be less than 4.");
//	static_assert(indexZ < 4, "indexZ is must be less than 4.");
//	static_assert(indexW < 4, "indexW is must be less than 4.");
//}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftL(const SIMD4i& lhs)
{
	return vreinterpretq_s32_u32(vshlq_n_u32(vreinterpretq_u32_s32(lhs.s), COUNT));
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftR(const SIMD4i& lhs)
{
	return vreinterpretq_s32_u32(vshrq_n_u32(vreinterpretq_u32_s32(lhs.s), COUNT));
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftRA(const SIMD4i& lhs)
{
	return vshrq_n_s32(lhs.s, COUNT);
}

template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
inline SIMD4i SIMD4i::Mask()
{
	static_assert(X >= 2, "indexX is must be set 0 or 1.");
	static_assert(Y >= 2, "indexY is must be set 0 or 1.");
	static_assert(Z >= 2, "indexZ is must be set 0 or 1.");
	static_assert(W >= 2, "indexW is must be set 0 or 1.");
	const uint32_t in[4] = {0xffffffff * X, 0xffffffff * Y, 0xffffffff * Z, 0xffffffff * W};
	return vld1q_u32(in);
}

inline uint32_t SIMD4i::MoveMask(const SIMD4i& in)
{
	uint16x4_t u16x4 = vmovn_u32(vreinterpretq_u32_s32(in.s));
	uint16_t u16[4];
	vst1_u16(u16, u16x4);
	return (u16[0] & 1) | (u16[1] & 2) | (u16[2] & 4) | (u16[3] & 8);
}

inline SIMD4i SIMD4i::Equal(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vreinterpretq_s32_u32(vceqq_s32(lhs.s, rhs.s));
}

inline SIMD4i SIMD4i::NotEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vreinterpretq_s32_u32(vmvnq_u32(vceqq_s32(lhs.s, rhs.s)));
}

inline SIMD4i SIMD4i::LessThan(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vreinterpretq_s32_u32(vcltq_s32(lhs.s, rhs.s)); }

inline SIMD4i SIMD4i::LessEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vreinterpretq_s32_u32(vcleq_s32(lhs.s, rhs.s)); 
}

inline SIMD4i SIMD4i::GreaterThan(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vreinterpretq_s32_u32(vcgtq_s32(lhs.s, rhs.s)); }

inline SIMD4i SIMD4i::GreaterEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return vreinterpretq_s32_u32(vcgeq_s32(lhs.s, rhs.s)); 
}

inline SIMD4i SIMD4i::NearEqual(const SIMD4i& lhs, const SIMD4i& rhs, int32_t epsilon)
{
	return LessEqual(Abs(lhs - rhs), SIMD4i(epsilon));
}

inline SIMD4i SIMD4i::IsZero(const SIMD4i& in, int32_t epsilon)
{
	return LessEqual(Abs(in), SIMD4i(epsilon));
}

inline void SIMD4i::Transpose(SIMD4i& s0, SIMD4i& s1, SIMD4i& s2, SIMD4i& s3)
{
	int32x4x2_t t0 = vzipq_s32(s0.s, s2.s);
	int32x4x2_t t1 = vzipq_s32(s1.s, s3.s);
	int32x4x2_t t2 = vzipq_s32(t0.val[0], t1.val[0]);
	int32x4x2_t t3 = vzipq_s32(t0.val[1], t1.val[1]);
	
	s0 = t2.val[0];
	s1 = t2.val[1];
	s2 = t3.val[0];
	s3 = t3.val[1];
}

inline SIMD4i SIMD4i::SwizzleYZX(const SIMD4i& in)
{
	int32x4_t ex = vextq_s32(in.s, in.s, 1);
	return vsetq_lane_s32(vgetq_lane_s32(ex, 3), ex, 2);
}

inline SIMD4i SIMD4i::SwizzleZXY(const SIMD4i& in)
{
	int32x4_t ex = vextq_s32(in.s, in.s, 3);
	return vsetq_lane_s32(vgetq_lane_s32(ex, 3), ex, 0);
}

} // namespace Effekseer

#endif
#endif // __EFFEKSEER_SIMD4I_NEON_H__
