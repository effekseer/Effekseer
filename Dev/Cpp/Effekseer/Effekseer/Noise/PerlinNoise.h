#ifndef __EFFEKSEER_PERLIN_NOISE_H__
#define __EFFEKSEER_PERLIN_NOISE_H__

#include <array>
#include <random>

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

public:
	constexpr PerlinNoise() = default;
	explicit PerlinNoise(const std::uint_fast32_t seed_) { this->setSeed(seed_); }

	void setSeed(const std::uint_fast32_t seed_)
	{
		for (std::size_t i{}; i < 256; ++i)
			this->p[i] = static_cast<Pint>(i);
		std::shuffle(this->p.begin(), this->p.begin() + 256, std::default_random_engine(seed_));
		for (std::size_t i{}; i < 256; ++i)
			this->p[256 + i] = this->p[i];
	}

private:
	constexpr float GetFade(const float t) const noexcept { return t * t * t * (t * (t * 6 - 15) + 10); }

	constexpr float GetLerp(const float t, const float a, const float b) const noexcept { return a + t * (b - a); }

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

public:
	float SetNoise(float x, float y, float z) const noexcept
	{
		const std::size_t x_int{static_cast<std::size_t>(static_cast<std::size_t>(std::floor(x)) & 255)};
		const std::size_t y_int{static_cast<std::size_t>(static_cast<std::size_t>(std::floor(y)) & 255)};
		const std::size_t z_int{static_cast<std::size_t>(static_cast<std::size_t>(std::floor(z)) & 255)};
		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);
		const float u{this->GetFade(x)};
		const float v{this->GetFade(y)};
		const float w{this->GetFade(z)};
		const std::size_t a0{static_cast<std::size_t>(this->p[x_int] + y_int)};
		const std::size_t a1{static_cast<std::size_t>(this->p[a0] + z_int)};
		const std::size_t a2{static_cast<std::size_t>(this->p[a0 + 1] + z_int)};
		const std::size_t b0{static_cast<std::size_t>(this->p[x_int + 1] + y_int)};
		const std::size_t b1{static_cast<std::size_t>(this->p[b0] + z_int)};
		const std::size_t b2{static_cast<std::size_t>(this->p[b0 + 1] + z_int)};

		const auto v111 = this->GetGrad(this->p[a1], x, y, z);
		const auto v011 = this->GetGrad(this->p[b1], x - 1, y, z);
		const auto v101 = this->GetGrad(this->p[a2], x, y - 1, z);
		const auto v001 = this->GetGrad(this->p[b2], x - 1, y - 1, z);
		const auto v110 = this->GetGrad(this->p[a1 + 1], x, y, z - 1);
		const auto v010 = this->GetGrad(this->p[b1 + 1], x - 1, y, z - 1);
		const auto v100 = this->GetGrad(this->p[a2 + 1], x, y - 1, z - 1);
		const auto v000 = this->GetGrad(this->p[b2 + 1], x - 1, y - 1, z - 1);

		const auto v11 = this->GetLerp(u, v111, v011);
		const auto v01 = this->GetLerp(u, v101, v001);
		const auto v10 = this->GetLerp(u, v110, v010);
		const auto v00 = this->GetLerp(u, v100, v000);

		return this->GetLerp(w, this->GetLerp(v, v11, v01), this->GetLerp(v, v10, v00));
	}

	template <typename... Args> float Noise(const Args... args_) const noexcept { return this->SetNoise(args_...) * 0.5 + 0.5; }


private:
	float SetOctaveNoise(const std::size_t octaves_, float x_, float y_, float z_) const noexcept
	{
		float noise_value{};
		float amp{1.0};
		for (std::size_t i{}; i < octaves_; ++i)
		{
			noise_value += this->SetNoise(x_, y_, z_) * amp;
			x_ *= 2.0;
			y_ *= 2.0;
			z_ *= 2.0;
			amp *= 0.5;
		}
		return noise_value;
	}

public:
	template <typename... Args> float OctaveNoise(const std::size_t octaves_, const Args... args_) const noexcept
	{
		return this->SetOctaveNoise(octaves_, args_...) * 0.5 + 0.5;
	}

};

} // namespace Effekseer

#endif