#ifndef __EFFEKSEER_PERLIN_NOISE_H__
#define __EFFEKSEER_PERLIN_NOISE_H__

#include <array>
#include <random>

#include "../SIMD/Effekseer.SIMD4Bridge.h"

namespace Effekseer
{

/**
	@brief
	Perlin noise class
	@note
	These codes are based on https://qiita.com/Gaccho/items/ba7d715901a0e572b0e9
*/

class PerlinNoise
{
	using Pint = std::uint_fast8_t;
	std::array<Pint, 512> p{{}};

	uint32_t seed_ = 0;

	float GetRand()
	{
		const int a = 1103515245;
		const int c = 12345;
		const int m = 2147483647;

		seed_ = (seed_ * a + c) & m;
		auto ret = seed_ % 0x7fff;

		return (float)ret / (float)(0x7fff - 1);
	}

	float GetRand(int32_t min_, int32_t max_)
	{
		return GetRand() * (max_ - min_) + min_;
	}

public:
	constexpr PerlinNoise() = default;
	explicit PerlinNoise(const std::uint_fast32_t seed)
	{
		this->setSeed(seed);
	}

	void setSeed(const std::uint_fast32_t seed)
	{
		seed_ = seed;

		for (std::size_t i{}; i < 256; ++i)
			this->p[i] = static_cast<Pint>(i);

		for (std::size_t i{}; i < 256; ++i)
		{
			auto target = static_cast<std::size_t>(GetRand(0, 255));
			std::swap(p[i], p[target]);
		}

		for (std::size_t i{}; i < 256; ++i)
			this->p[256 + i] = this->p[i];
	}

private:
	constexpr float GetFade(const float t) const noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	SIMD4f GetFadeFast(const SIMD4f in) const noexcept
	{
		const SIMD4f c6(6.0f);
		const SIMD4f c15(15.0f);
		const SIMD4f c10(10.0f);

		SIMD4f t3 = in * in * in;
		// SIMD4f t6_15_10 = _mm_add_ps(_mm_mul_ps(in, _mm_sub_ps(_mm_mul_ps(in, c6), c15)), c10);
		SIMD4f t6_15_10 = (in * ((in * c6) - c15)) + c10;
		return t3 * t6_15_10;
	}

	constexpr float GetLerp(const float t, const float a, const float b) const noexcept
	{
		return a + t * (b - a);
	}

	SIMD4f GetLerpFast(const SIMD4f t, const SIMD4f a, const SIMD4f b) const noexcept
	{
		return a + t * (b - a);
	}

	constexpr float MakeGrad(const Pint hashnum, const float u, const float v) const noexcept
	{
		return (((hashnum & 1) == 0) ? u : -u) + (((hashnum & 2) == 0) ? v : -v);
	}

	constexpr float MakeGrad(const Pint hashnum, const float x, const float y, const float z) const noexcept
	{
		return this->MakeGrad(hashnum, hashnum < 8 ? x : y, hashnum < 4 ? y : hashnum == 12 || hashnum == 14 ? x : z);
	}

	constexpr float GetGrad(const Pint hashnum, const float x, const float y, const float z) const noexcept
	{
		return this->MakeGrad(hashnum & 15, x, y, z);
	}

	float MakeGradFast(const Pint hashnum, const float u, const float v) const noexcept
	{
		union {
			float f;
			uint32_t i;
		} u_bits, v_bits;

		u_bits.f = u;
		v_bits.f = v;

		u_bits.i ^= ((hashnum & 1) << 31);
		v_bits.i ^= ((hashnum & 2) << 30);

		return u_bits.f + v_bits.f;
	}

	float MakeGradFast(const Pint hashnum, const float x, const float y, const float z) const noexcept
	{
		return this->MakeGradFast(hashnum, hashnum < 8 ? x : y, hashnum < 4 ? y : hashnum == 12 || hashnum == 14 ? x : z);
	}

	float GetGradFast(const Pint hashnum, const float x, const float y, const float z) const noexcept
	{
		return this->MakeGradFast(hashnum & 15, x, y, z);
	}

	SIMD4f MakeGradFast(const SIMD4i hashnum, const SIMD4f u, const SIMD4f v) const noexcept
	{
		SIMD4i hashBits1 = hashnum & SIMD4i(1);
		SIMD4i hashBits2 = hashnum & SIMD4i(2);

		return (u ^ SIMD4i::ShiftL<31>(hashBits1).Cast4f()) + (v ^ SIMD4i::ShiftL<30>(hashBits2).Cast4f());
	}

	SIMD4f MakeGradFast(const SIMD4i hashnum, const SIMD4f x, const SIMD4f y, const SIMD4f z) const noexcept
	{
		SIMD4f in1_mask = SIMD4i::LessThan(hashnum, SIMD4i(8)).Cast4f();
		SIMD4f in1 = SIMD4f::Select(in1_mask, x, y);

		SIMD4f in2_mask1 = (SIMD4i::LessThan(hashnum, SIMD4i(4))).Cast4f();
		SIMD4f in2_mask2 = (SIMD4i::Equal(hashnum, SIMD4i(12)) | SIMD4i::Equal(hashnum, SIMD4i(14))).Cast4f();
		SIMD4f in2 = SIMD4f::Select(in2_mask1, y, SIMD4f::Select(in2_mask2, x, z));

		return this->MakeGradFast(hashnum, in1, in2);
	}

	SIMD4f GetGradFast(const SIMD4i hashnum, const SIMD4f x, const SIMD4f y, const SIMD4f z) const noexcept
	{
		return this->MakeGradFast(hashnum & SIMD4i(15), x, y, z);
	}

public:
	float SetNoise(Vec3f position) const noexcept
	{
		SIMD4f in = position.s;
		SIMD4f flin = SIMD4f::Floor(in);

		SIMD4i xyz_int = flin.Convert4i() & SIMD4i(0xff);
		uint32_t x_int{(uint32_t)xyz_int.GetX()};
		uint32_t y_int{(uint32_t)xyz_int.GetY()};
		uint32_t z_int{(uint32_t)xyz_int.GetZ()};

		in -= flin;

		SIMD4f uvw = GetFadeFast(in);
		const float u{uvw.GetX()};
		const float v{uvw.GetY()};
		const float w{uvw.GetZ()};

		const uint32_t a0{this->p[x_int] + y_int};
		const uint32_t a1{this->p[a0] + z_int};
		const uint32_t a2{this->p[a0 + 1] + z_int};
		const uint32_t b0{this->p[x_int + 1] + y_int};
		const uint32_t b1{this->p[b0] + z_int};
		const uint32_t b2{this->p[b0 + 1] + z_int};

		SIMD4i vp1(p[a1], p[a2], p[a1 + 1], p[a2 + 1]);
		SIMD4i vp2(p[b1], p[b2], p[b1 + 1], p[b2 + 1]);

		SIMD4f vx1 = in.Dup<0>();
		SIMD4f vx2 = vx1 - SIMD4f(1.0f);
		SIMD4f vy = in.Dup<1>() - SIMD4f(0.0f, 1.0f, 0.0f, 1.0f);
		SIMD4f vz = in.Dup<2>() - SIMD4f(0.0f, 0.0f, 1.0f, 1.0f);

		SIMD4f vv1 = GetGradFast(vp1, vx1, vy, vz);
		SIMD4f vv2 = GetGradFast(vp2, vx2, vy, vz);
		SIMD4f vv = GetLerpFast(SIMD4f(u), vv1, vv2);

		return this->GetLerp(w, this->GetLerp(v, vv.GetX(), vv.GetY()), this->GetLerp(v, vv.GetZ(), vv.GetW()));
	}

	float Noise(Vec3f position) const noexcept
	{
		return this->SetNoise(position) * 0.5f + 0.5f;
	}

public:
	float OctaveNoise(const std::size_t octaves_, Vec3f position) const noexcept
	{
		float noise_value{};
		float amp{1.0};
		for (std::size_t i{}; i < octaves_; ++i)
		{
			noise_value += this->SetNoise(position) * amp;
			position *= 2.0f;
			amp *= 0.5f;
		}
		return noise_value * 0.5f + 0.5f;
	}
};

} // namespace Effekseer

#endif
