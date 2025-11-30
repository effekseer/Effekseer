
#ifndef __EFFEKSEER_MATH_H__
#define __EFFEKSEER_MATH_H__

#include <cmath>
#include <cstdint>

namespace Effekseer
{

/**
	@brief
	\~English	Type cast that preserves bit array
	\~Japanese	ビット列を維持した型キャスト
*/
template <class To, class From>
inline To BitCast(From from)
{
	static_assert(sizeof(From) == sizeof(To), "number of bits not match");

	union
	{
		From from;
		To to;
	} v;
	v.from = from;
	return v.to;
}

/**
	@brief
	\~English	Get fractional part
	\~Japanese	小数部分を取得する
*/
inline float Frac(float x)
{
	x -= (int)x;
	int32_t ofs = (BitCast<int32_t>(x) >> 31) & 0x3F800000;
	return x + BitCast<float>(ofs);
}

/**
	@brief
	\~English	Normalize angle to [-pi, pi]
	\~Japanese	角度を[-π, π]の範囲に正規化する
*/
inline float NormalizeAngle(float angle)
{
	int32_t ofs = (BitCast<int32_t>(angle) & 0x80000000) | 0x3F000000;
	return angle - ((int)(angle * 0.159154943f + BitCast<float>(ofs)) * 6.283185307f);
}

/**
	@brief
	\~English	Calculate sine and cosine using Taylor series expansion
	\~Japanese	Taylor級数展開を用いて正弦と余弦を計算する
*/
inline void SinCos(float x, float& s, float& c)
{
	x = NormalizeAngle(x);
	float x2 = x * x;
	float x4 = x * x * x * x;
	float x6 = x * x * x * x * x * x;
	float x8 = x * x * x * x * x * x * x * x;
	float x10 = x * x * x * x * x * x * x * x * x * x;
	s = x * (1.0f - x2 / 6.0f + x4 / 120.0f - x6 / 5040.0f + x8 / 362880.0f - x10 / 39916800.0f);
	c = 1.0f - x2 / 2.0f + x4 / 24.0f - x6 / 720.0f + x8 / 40320.0f - x10 / 3628800.0f;
}

} // namespace Effekseer

#endif // __EFFEKSEER_MATH_H__
