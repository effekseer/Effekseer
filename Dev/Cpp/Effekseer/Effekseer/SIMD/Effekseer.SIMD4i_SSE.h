
#ifndef __EFFEKSEER_SIMD4I_SSE_H__
#define __EFFEKSEER_SIMD4I_SSE_H__

#include "Effekseer.SIMDType.h"

#if defined(EFK_SIMD_SSE2)

namespace Effekseer
{

struct SIMD4f;

/**
	@brief	simd class for sse
*/

struct alignas(16) SIMD4i
{
	__m128i s;

	SIMD4i() = default;
	SIMD4i(const SIMD4i& rhs) = default;
	SIMD4i(__m128i rhs) { s = rhs; }
	SIMD4i(__m128 rhs) { s = _mm_castps_si128(rhs); }
	SIMD4i(int32_t x, int32_t y, int32_t z, int32_t w) { s = _mm_setr_epi32((int)x, (int)y, (int)z, (int)w); }
	SIMD4i(int32_t i) { s = _mm_set1_epi32((int)i); }

	int32_t GetX() const { return _mm_cvtsi128_si32(s); }
	int32_t GetY() const { return _mm_cvtsi128_si32(Swizzle<1,1,1,1>(s).s); }
	int32_t GetZ() const { return _mm_cvtsi128_si32(Swizzle<2,2,2,2>(s).s); }
	int32_t GetW() const { return _mm_cvtsi128_si32(Swizzle<3,3,3,3>(s).s); }

	void SetX(int32_t i) { s = _mm_castps_si128(_mm_move_ss(_mm_castsi128_ps(s), _mm_castsi128_ps(_mm_cvtsi32_si128(i)))); }
	void SetY(int32_t i) { s = Swizzle<1,0,2,3>(_mm_castps_si128(_mm_move_ss(_mm_castsi128_ps(Swizzle<1,0,2,3>(s).s), _mm_castsi128_ps(_mm_cvtsi32_si128(i))))).s; }
	void SetZ(int32_t i) { s = Swizzle<2,1,0,3>(_mm_castps_si128(_mm_move_ss(_mm_castsi128_ps(Swizzle<2,1,0,3>(s).s), _mm_castsi128_ps(_mm_cvtsi32_si128(i))))).s; }
	void SetW(int32_t i) { s = Swizzle<3,1,2,0>(_mm_castps_si128(_mm_move_ss(_mm_castsi128_ps(Swizzle<3,1,2,0>(s).s), _mm_castsi128_ps(_mm_cvtsi32_si128(i))))).s; }

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
};

} // namespace Effekseer

namespace Effekseer
{

inline SIMD4i operator+(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_add_epi32(lhs.s, rhs.s)};
}

inline SIMD4i operator-(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_sub_epi32(lhs.s, rhs.s)};
}

inline SIMD4i operator*(const SIMD4i& lhs, const SIMD4i& rhs)
{
#if defined(EFK_SIMD_SSE4_1)
	return _mm_mullo_epi32(lhs.s, rhs.s);
#else
	__m128i tmp1 = _mm_mul_epu32(lhs.s, rhs.s);
	__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(lhs.s, 4), _mm_srli_si128(rhs.s, 4));
	return _mm_unpacklo_epi32(
		_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0,0,2,0)),
		_mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0,0,2,0)));
#endif
}

inline SIMD4i operator*(const SIMD4i& lhs, int32_t rhs)
{
#if defined(EFK_SIMD_SSE4_1)
	return _mm_mullo_epi32(lhs.s, _mm_set1_epi32(rhs));
#else
	__m128i tmp1 = _mm_mul_epu32(lhs.s, _mm_set1_epi32(rhs));
	__m128i tmp2 = _mm_mul_epu32(_mm_srli_si128(lhs.s, 4), _mm_set1_epi32(rhs));
	return _mm_unpacklo_epi32(
		_mm_shuffle_epi32(tmp1, _MM_SHUFFLE(0,0,2,0)),
		_mm_shuffle_epi32(tmp2, _MM_SHUFFLE(0,0,2,0)));
#endif
}

inline SIMD4i operator/(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i(
		lhs.GetX() * rhs.GetX(),
		lhs.GetY() * rhs.GetY(),
		lhs.GetZ() * rhs.GetZ(),
		lhs.GetW() * rhs.GetW());
}

inline SIMD4i operator/(const SIMD4i& lhs, int32_t rhs)
{
	return SIMD4i(
		lhs.GetX() * rhs,
		lhs.GetY() * rhs,
		lhs.GetZ() * rhs,
		lhs.GetW() * rhs);
}

inline SIMD4i operator&(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_and_si128(lhs.s, rhs.s)};
}

inline SIMD4i operator|(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_or_si128(lhs.s, rhs.s)};
}

inline SIMD4i operator^(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_xor_si128(lhs.s, rhs.s)};
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
	__m128 x = _mm_load_ss((const float*)mem + 0);
	__m128 y = _mm_load_ss((const float*)mem + 1);
	return _mm_castps_si128(_mm_unpacklo_ps(x, y));
}

inline void SIMD4i::Store2(void* mem, const SIMD4i& i)
{
	SIMD4i t1 = Swizzle<1,1,1,1>(i);
	_mm_store_ss((float*)mem + 0, _mm_castsi128_ps(i.s));
	_mm_store_ss((float*)mem + 1, _mm_castsi128_ps(t1.s));
}

inline SIMD4i SIMD4i::Load3(const void* mem)
{
	__m128 x = _mm_load_ss((const float*)mem + 0);
	__m128 y = _mm_load_ss((const float*)mem + 1);
	__m128 z = _mm_load_ss((const float*)mem + 2);
	__m128 xy = _mm_unpacklo_ps(x, y);
	return _mm_castps_si128(_mm_movelh_ps(xy, z));
}

inline void SIMD4i::Store3(void* mem, const SIMD4i& i)
{
	SIMD4i t1 = Swizzle<1,1,1,1>(i);
	SIMD4i t2 = Swizzle<2,2,2,2>(i);
	_mm_store_ss((float*)mem + 0, _mm_castsi128_ps(i.s));
	_mm_store_ss((float*)mem + 1, _mm_castsi128_ps(t1.s));
	_mm_store_ss((float*)mem + 2, _mm_castsi128_ps(t2.s));
}

inline SIMD4i SIMD4i::Load4(const void* mem)
{
	return _mm_loadu_si128((const __m128i*)mem);
}

inline void SIMD4i::Store4(void* mem, const SIMD4i& i)
{
	_mm_storeu_si128((__m128i*)mem, i.s);
}

inline SIMD4i SIMD4i::SetZero()
{
	return _mm_setzero_si128();
}

inline SIMD4i SIMD4i::Abs(const SIMD4i& in)
{
#if defined(EFK_SIMD_SSSE3)
	return _mm_abs_epi32(in.s);
#else
	__m128i sign = _mm_srai_epi32(in.s, 31);
	return _mm_sub_epi32(_mm_xor_si128(in.s, sign), sign);
#endif
}

inline SIMD4i SIMD4i::Min(const SIMD4i& lhs, const SIMD4i& rhs)
{
#if defined(EFK_SIMD_SSE4_1)
	return _mm_min_epi32(lhs.s, rhs.s);
#else
	__m128i mask = _mm_cmplt_epi32(lhs.s, rhs.s);
	return _mm_or_si128(_mm_and_si128(mask, lhs.s), _mm_andnot_si128(mask, rhs.s));
#endif
}

inline SIMD4i SIMD4i::Max(const SIMD4i& lhs, const SIMD4i& rhs)
{
#if defined(EFK_SIMD_SSE4_1)
	return _mm_max_epi32(lhs.s, rhs.s);
#else
	__m128i mask = _mm_cmpgt_epi32(lhs.s, rhs.s);
	return _mm_or_si128(_mm_and_si128(mask, lhs.s), _mm_andnot_si128(mask, rhs.s));
#endif
}

inline SIMD4i SIMD4i::MulAdd(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	return a + b * c;
}

inline SIMD4i SIMD4i::MulSub(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	return a - b * c;
}

template<size_t LANE>
SIMD4i SIMD4i::MulLane(const SIMD4i& lhs, const SIMD4i& rhs)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return lhs * SIMD4i::Swizzle<LANE,LANE,LANE,LANE>(rhs);
}

template<size_t LANE>
SIMD4i SIMD4i::MulAddLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return a + b * SIMD4i::Swizzle<LANE,LANE,LANE,LANE>(c);
}

template<size_t LANE>
SIMD4i SIMD4i::MulSubLane(const SIMD4i& a, const SIMD4i& b, const SIMD4i& c)
{
	static_assert(LANE < 4, "LANE is must be less than 4.");
	return a - b * SIMD4i::Swizzle<LANE,LANE,LANE,LANE>(c);
}

template <uint32_t indexX, uint32_t indexY, uint32_t indexZ, uint32_t indexW>
SIMD4i SIMD4i::Swizzle(const SIMD4i& v)
{
	static_assert(indexX < 4, "indexX is must be less than 4.");
	static_assert(indexY < 4, "indexY is must be less than 4.");
	static_assert(indexZ < 4, "indexZ is must be less than 4.");
	static_assert(indexW < 4, "indexW is must be less than 4.");
	return SIMD4i{_mm_shuffle_epi32(v.s, _MM_SHUFFLE(indexW, indexZ, indexY, indexX))};
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftL(const SIMD4i& lhs)
{
	return _mm_slli_epi32(lhs.s, COUNT);
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftR(const SIMD4i& lhs)
{
	return _mm_srli_epi32(lhs.s, COUNT);
}

template <int COUNT>
inline SIMD4i Effekseer::SIMD4i::ShiftRA(const SIMD4i& lhs)
{
	return _mm_srai_epi32(lhs.s, COUNT);
}

template <uint32_t X, uint32_t Y, uint32_t Z, uint32_t W>
inline SIMD4i SIMD4i::Mask()
{
	static_assert(X >= 2, "indexX is must be set 0 or 1.");
	static_assert(Y >= 2, "indexY is must be set 0 or 1.");
	static_assert(Z >= 2, "indexZ is must be set 0 or 1.");
	static_assert(W >= 2, "indexW is must be set 0 or 1.");
	return _mm_setr_epi32(
		(int)(0xffffffff * X), 
		(int)(0xffffffff * Y), 
		(int)(0xffffffff * Z), 
		(int)(0xffffffff * W));
}

inline uint32_t SIMD4i::MoveMask(const SIMD4i& in)
{
	return (uint32_t)_mm_movemask_ps(_mm_castsi128_ps(in.s));
}

inline SIMD4i SIMD4i::Equal(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_cmpeq_epi32(lhs.s, rhs.s)};
}

inline SIMD4i SIMD4i::NotEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_andnot_si128(_mm_cmpeq_epi32(lhs.s, rhs.s), _mm_set1_epi32(-1))};
}

inline SIMD4i SIMD4i::LessThan(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_cmplt_epi32(lhs.s, rhs.s)};
}

inline SIMD4i SIMD4i::LessEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_andnot_si128(_mm_cmpgt_epi32(lhs.s, rhs.s), _mm_set1_epi32(-1))};
}

inline SIMD4i SIMD4i::GreaterThan(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_cmpgt_epi32(lhs.s, rhs.s)};
}

inline SIMD4i SIMD4i::GreaterEqual(const SIMD4i& lhs, const SIMD4i& rhs)
{
	return SIMD4i{_mm_andnot_si128(_mm_cmplt_epi32(lhs.s, rhs.s), _mm_set1_epi32(-1))};
}

} // namespace Effekseer

#endif

#endif // __EFFEKSEER_SIMD4I_SSE_H__
