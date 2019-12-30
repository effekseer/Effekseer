
#ifndef __EFFEKSEER_SIMD4F_SSE_H__
#define __EFFEKSEER_SIMD4F_SSE_H__

#include <stdint.h>
#ifdef _MSC_VER
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#include "../Effekseer.Math.h"

namespace Effekseer
{

inline float Sqrt(float x)
{
	_mm_store_ss(&x, _mm_sqrt_ss(_mm_load_ss(&x)));
	return x;
}
inline float Rsqrt(float x)
{
	_mm_store_ss(&x, _mm_rsqrt_ss(_mm_load_ss(&x)));
	return x;
}

/**
	@brief	simd class for sse
*/

struct alignas(16) SIMD4f
{
	__m128 s;

	SIMD4f() = default;
	SIMD4f(const SIMD4f& rhs) = default;
	SIMD4f(__m128 rhs) { s = rhs; }
	SIMD4f(__m128i rhs) { s = _mm_castsi128_ps(rhs); }
	SIMD4f(float x, float y, float z, float w) { s = _mm_setr_ps(x, y, z, w); }
	SIMD4f(float i) { s = _mm_set_ps1(i); }

	float GetX() const { return _mm_cvtss_f32(s); }
	float GetY() const { return _mm_cvtss_f32(Swizzle<1,1,1,1>(s).s); }
	float GetZ() const { return _mm_cvtss_f32(Swizzle<2,2,2,2>(s).s); }
	float GetW() const { return _mm_cvtss_f32(Swizzle<3,3,3,3>(s).s); }

	void SetX(float i) { s = _mm_move_ss(s, _mm_set_ss(i)); }
	void SetY(float i) { s = Swizzle<1,0,2,3>(_mm_move_ss(Swizzle<1,0,2,3>(s).s, _mm_set_ss(i))).s; }
	void SetZ(float i) { s = Swizzle<2,1,0,3>(_mm_move_ss(Swizzle<2,1,0,3>(s).s, _mm_set_ss(i))).s; }
	void SetW(float i) { s = Swizzle<3,1,2,0>(_mm_move_ss(Swizzle<3,1,2,0>(s).s, _mm_set_ss(i))).s; }

	SIMD4f& operator+=(const SIMD4f& rhs) { s = _mm_add_ps(s, rhs.s); return *this; }
	SIMD4f& operator-=(const SIMD4f& rhs) { s = _mm_sub_ps(s, rhs.s); return *this; }
	SIMD4f& operator*=(const SIMD4f& rhs) { s = _mm_mul_ps(s, rhs.s); return *this; }
	SIMD4f& operator*=(float rhs) { s = _mm_mul_ps(s, _mm_set1_ps(rhs)); return *this; }
	SIMD4f& operator/=(const SIMD4f& rhs) { s = _mm_div_ps(s, rhs.s); return *this; }
	SIMD4f& operator/=(float rhs) { s = _mm_div_ps(s, _mm_set1_ps(rhs)); return *this; }

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
	static SIMD4f Equal(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f NotEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f LessThan(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f LessEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f GreaterThan(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f GreaterEqual(const SIMD4f& lhs, const SIMD4f& rhs);
	static SIMD4f NearEqual(const SIMD4f& lhs, const SIMD4f& rhs, float epsilon = DefaultEpsilon);
	static SIMD4f IsZero(const SIMD4f& in, float epsilon = DefaultEpsilon);
	static void Transpose(SIMD4f& s0, SIMD4f& s1, SIMD4f& s2, SIMD4f& s3);
};

inline SIMD4f operator+(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_add_ps(lhs.s, rhs.s)};
}

inline SIMD4f operator-(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_sub_ps(lhs.s, rhs.s)};
}

inline SIMD4f operator*(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_mul_ps(lhs.s, rhs.s)};
}

inline SIMD4f operator*(const SIMD4f& lhs, float rhs)
{
	return SIMD4f{_mm_mul_ps(lhs.s, _mm_set1_ps(rhs))};
}

inline SIMD4f operator/(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_div_ps(lhs.s, rhs.s)};
}

inline SIMD4f operator/(const SIMD4f& lhs, float rhs)
{
	return SIMD4f{_mm_div_ps(lhs.s, _mm_set1_ps(rhs))};
}

inline SIMD4f operator&(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_and_ps(lhs.s, rhs.s)};
}

inline SIMD4f operator|(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_or_ps(lhs.s, rhs.s)};
}

inline bool operator==(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f::MoveMask(SIMD4f::Equal(lhs, rhs)) == 0xf;
}

inline bool operator!=(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f::MoveMask(SIMD4f::Equal(lhs, rhs)) != 0xf;
}

inline SIMD4f SIMD4f::Load2(const void* mem)
{
	__m128 x = _mm_load_ss((const float*)mem + 0);
	__m128 y = _mm_load_ss((const float*)mem + 1);
	return _mm_unpacklo_ps(x, y);
}

inline void SIMD4f::Store2(void* mem, const SIMD4f& i)
{
	SIMD4f t1 = Swizzle<1,1,1,1>(i.s);
	_mm_store_ss((float*)mem + 0, i.s);
	_mm_store_ss((float*)mem + 1, t1.s);
}

inline SIMD4f SIMD4f::Load3(const void* mem)
{
	__m128 x = _mm_load_ss((const float*)mem + 0);
	__m128 y = _mm_load_ss((const float*)mem + 1);
	__m128 z = _mm_load_ss((const float*)mem + 2);
	__m128 xy = _mm_unpacklo_ps(x, y);
	return _mm_movelh_ps(xy, z);
}

inline void SIMD4f::Store3(void* mem, const SIMD4f& i)
{
	SIMD4f t1 = Swizzle<1,1,1,1>(i.s);
	SIMD4f t2 = Swizzle<2,2,2,2>(i.s);
	_mm_store_ss((float*)mem + 0, i.s);
	_mm_store_ss((float*)mem + 1, t1.s);
	_mm_store_ss((float*)mem + 2, t2.s);
}

inline SIMD4f SIMD4f::Load4(const void* mem)
{
	return _mm_loadu_ps((const float*)mem);
}

inline void SIMD4f::Store4(void* mem, const SIMD4f& i)
{
	_mm_storeu_ps((float*)mem, i.s);
}

inline SIMD4f SIMD4f::SetZero()
{
	return _mm_setzero_ps();
}

inline SIMD4f SIMD4f::SetInt(int32_t x, int32_t y, int32_t z, int32_t w)
{
	return SIMD4f{_mm_setr_epi32((int)x, (int)y, (int)z, (int)w)};
}

inline SIMD4f SIMD4f::SetUInt(uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
	return SIMD4f{_mm_setr_epi32((int)x, (int)y, (int)z, (int)w)};
}

inline SIMD4f SIMD4f::Sqrt(const SIMD4f& in)
{
	return SIMD4f{_mm_sqrt_ps(in.s)};
}

inline SIMD4f SIMD4f::Rsqrt(const SIMD4f& in)
{
	return SIMD4f{_mm_rsqrt_ps(in.s)};
}

inline SIMD4f SIMD4f::Abs(const SIMD4f& in)
{
	return _mm_andnot_ps(_mm_set1_ps(-0.0f), in.s);
}

inline SIMD4f SIMD4f::Min(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_min_ps(lhs.s, rhs.s)};
}

inline SIMD4f SIMD4f::Max(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_max_ps(lhs.s, rhs.s)};
}

inline SIMD4f SIMD4f::MulAdd(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
#ifdef __AVX2__
	return SIMD4f{_mm_fmadd_ps(b.s, c.s, a.s)};
#else
	return SIMD4f{_mm_add_ps(a.s, _mm_mul_ps(b.s, c.s))};
#endif
}

inline SIMD4f SIMD4f::MulSub(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
#ifdef __AVX2__
	return SIMD4f{_mm_fnmadd_ps(b.s, c.s, a.s)};
#else
	return SIMD4f{_mm_sub_ps(a.s, _mm_mul_ps(b.s, c.s))};
#endif
}

template<size_t LANE>
static SIMD4f SIMD4f::MulLane(const SIMD4f& lhs, const SIMD4f& rhs)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return SIMD4f{_mm_mul_ps(lhs.s, _mm_shuffle_ps(rhs.s, rhs.s, _MM_SHUFFLE(LANE, LANE, LANE, LANE)))};
}

template<size_t LANE>
static SIMD4f SIMD4f::MulAddLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
#ifdef __AVX2__
	return SIMD4f{_mm_fmadd_ps(b.s, _mm_shuffle_ps(c.s, c.s, _MM_SHUFFLE(LANE, LANE, LANE, LANE)), a.s)};
#else
	return SIMD4f{_mm_add_ps(a.s, _mm_mul_ps(b.s, _mm_shuffle_ps(c.s, c.s, _MM_SHUFFLE(LANE, LANE, LANE, LANE))))};
#endif
}

template<size_t LANE>
static SIMD4f SIMD4f::MulSubLane(const SIMD4f& a, const SIMD4f& b, const SIMD4f& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
#ifdef __AVX2__
	return SIMD4f{_mm_fnmadd_ps(b.s, _mm_shuffle_ps(c.s, c.s, _MM_SHUFFLE(LANE, LANE, LANE, LANE)), a.s)};
#else
	return SIMD4f{_mm_sub_ps(a.s, _mm_mul_ps(b.s, _mm_shuffle_ps(c.s, c.s, _MM_SHUFFLE(LANE, LANE, LANE, LANE))))};
#endif
}

template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
static SIMD4f SIMD4f::Swizzle(const SIMD4f& v)
{
	static_assert(indexX < 4, "indexX is must be less than 4.");
	static_assert(indexY < 4, "indexY is must be less than 4.");
	static_assert(indexZ < 4, "indexZ is must be less than 4.");
	static_assert(indexW < 4, "indexW is must be less than 4.");
	return SIMD4f{_mm_shuffle_ps(v.s, v.s, _MM_SHUFFLE(indexW, indexZ, indexY, indexX))};
}

inline SIMD4f SIMD4f::Dot3(const SIMD4f& lhs, const SIMD4f& rhs)
{
	SIMD4f muled = lhs * rhs;
	return _mm_add_ss(_mm_add_ss(muled.s, SIMD4f::Swizzle<1,1,1,1>(muled).s), SIMD4f::Swizzle<2,2,2,2>(muled).s);
}

inline SIMD4f SIMD4f::Cross3(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f::Swizzle<1,2,0,3>(lhs) * SIMD4f::Swizzle<2,0,1,3>(rhs) -
		SIMD4f::Swizzle<2,0,1,3>(lhs) * SIMD4f::Swizzle<1,2,0,3>(rhs);
}

template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
inline SIMD4f SIMD4f::Mask()
{
	return _mm_setr_epi32(
		(int)(0xffffffff * X), 
		(int)(0xffffffff * Y), 
		(int)(0xffffffff * Z), 
		(int)(0xffffffff * W));
}

inline uint32_t SIMD4f::MoveMask(const SIMD4f& in)
{
	return (uint32_t)_mm_movemask_ps(in.s);
}

inline SIMD4f SIMD4f::Equal(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_cmpeq_ps(lhs.s, rhs.s)};
}

inline SIMD4f SIMD4f::NotEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_cmpneq_ps(lhs.s, rhs.s)};
}

inline SIMD4f SIMD4f::LessThan(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_cmplt_ps(lhs.s, rhs.s)};
}

inline SIMD4f SIMD4f::LessEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_cmple_ps(lhs.s, rhs.s)};
}

inline SIMD4f SIMD4f::GreaterThan(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_cmpgt_ps(lhs.s, rhs.s)};
}

inline SIMD4f SIMD4f::GreaterEqual(const SIMD4f& lhs, const SIMD4f& rhs)
{
	return SIMD4f{_mm_cmpge_ps(lhs.s, rhs.s)};
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
	_MM_TRANSPOSE4_PS(s0.s, s1.s, s2.s, s3.s);
}

} // namespace Effekseer

#endif // __EFFEKSEER_SIMD4F_SSE_H__