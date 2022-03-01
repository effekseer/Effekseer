#include "../Effekseer.Math.h"
#include "../SIMD/Int4.h"
#include "../SIMD/Float4.h"
#include "../Effekseer.Random.h"
#include "CurlNoise.h"

namespace Effekseer
{

SIMD::Vec3f CurlNoise::Get(SIMD::Vec3f pos) const
{
	pos *= Scale;

	const float e = 1.0f / 1024.0f;

	const SIMD::Vec3f dx = SIMD::Vec3f(e, 0.0, 0.0);
	const SIMD::Vec3f dy = SIMD::Vec3f(0.0, e, 0.0);
	const SIMD::Vec3f dz = SIMD::Vec3f(0.0, 0.0, e);

	auto noise_x = [this](SIMD::Vec3f v) -> SIMD::Vec3f { return SIMD::Vec3f(0.0f, ynoise_.OctaveNoise(Octave, v), znoise_.OctaveNoise(Octave, v)); };

	auto noise_y = [this](SIMD::Vec3f v) -> SIMD::Vec3f { return SIMD::Vec3f(xnoise_.OctaveNoise(Octave, v), 0.0f, znoise_.OctaveNoise(Octave, v)); };

	auto noise_z = [this](SIMD::Vec3f v) -> SIMD::Vec3f { return SIMD::Vec3f(xnoise_.OctaveNoise(Octave, v), ynoise_.OctaveNoise(Octave, v), 0.0f); };

	SIMD::Vec3f p_x = noise_x(pos + dx) - noise_x(pos - dx);
	SIMD::Vec3f p_y = noise_y(pos + dy) - noise_y(pos - dy);
	SIMD::Vec3f p_z = noise_z(pos + dz) - noise_z(pos - dz);

	float x = p_y.GetZ() - p_z.GetY();
	float y = p_z.GetX() - p_x.GetZ();
	float z = p_x.GetY() - p_y.GetX();

	return SIMD::Vec3f(x, y, z) * (1.0f / (e * 2.0f));
}

LightCurlNoise::LightCurlNoise(int32_t seed, float scale, int32_t octave)
	: Scale(scale)
{
	PerlinNoise xnoise_(seed);
	PerlinNoise ynoise_(seed * (seed % 1949 + 5));
	PerlinNoise znoise_(seed * (seed % 3541 + 10));

#ifdef LIGHTCURL_OPTIMIZE
	RandObject random;
#endif

	for (int32_t z = 0; z < 8; z++)
	{
		for (int32_t y = 0; y < 8; y++)
		{
			for (int32_t x = 0; x < 8; x++)
			{
				SIMD::Vec3f v = SIMD::Vec3f(x, y, z) * (1.0f / 8.0f);

				auto fx = xnoise_.OctaveNoise(octave, v);
				auto fy = ynoise_.OctaveNoise(octave, v);
				auto fz = znoise_.OctaveNoise(octave, v);

				v = (SIMD::Vec3f(fx, fy, fz) - SIMD::Vec3f(0.5f)) * 2.0f;

				if (v.GetLength() < 0.00001f)
				{
#ifdef LIGHTCURL_OPTIMIZE
					v.SetX(random.GetRand(-1.0f, 1.0f));
					v.SetY(random.GetRand(-1.0f, 1.0f));
					v.SetZ(random.GetRand(-1.0f, 1.0f));
#else
					v.SetX(GetRand(-1.0f, 1.0f));
					v.SetY(GetRand(-1.0f, 1.0f));
					v.SetZ(GetRand(-1.0f, 1.0f));
#endif
				}

				if (v.GetLength() < 0.00001f)
				{
					v.SetZ(0.1f);
				}

				v.Normalize();

#ifdef LIGHTCURL_OPTIMIZE
				vectorField_[z][y][x] = Pack(v);
#else
				vectorField_x_[x + y * 8 + z * 8 * 8] = Pack(v.GetX());
				vectorField_y_[x + y * 8 + z * 8 * 8] = Pack(v.GetY());
				vectorField_z_[x + y * 8 + z * 8 * 8] = Pack(v.GetZ());
#endif
			}
		}
	}
}

#ifdef LIGHTCURL_OPTIMIZE
uint32_t LightCurlNoise::Pack(const SIMD::Vec3f v) const
{
	const auto packed = ((v.s + 1.0f) * 0.5f * GridBitMask).Convert4i();
	const auto x = static_cast<uint32_t>(std::max(0, std::min(GridBitMask, packed.GetX())));
	const auto y = static_cast<uint32_t>(std::max(0, std::min(GridBitMask, packed.GetY())));
	const auto z = static_cast<uint32_t>(std::max(0, std::min(GridBitMask, packed.GetZ())));
	return x | (y << GridBits) | (z << (GridBits * 2));
}

SIMD::Vec3f LightCurlNoise::Unpack(const uint32_t v) const
{
	const auto x = static_cast<int32_t>(v & GridBitMask);
	const auto y = static_cast<int32_t>((v >> GridBits) & GridBitMask);
	const auto z = static_cast<int32_t>((v >> (GridBits * 2)) & GridBitMask);
	return SIMD::Int4(x, y, z, 0.0f).Convert4f() * (2.0f / GridBitMask) - 1.0f;
}
#else
uint8_t LightCurlNoise::Pack(const float v) const
{
	const auto packed = (v + 1.0f) / 2.0f * 255.0f;
	return static_cast<uint8_t>(std::max(0.0f, std::min(255.0f, packed)));
}

float LightCurlNoise::Unpack(const uint8_t v) const
{
	return (static_cast<float>(v) / 255.0f - 0.5f) * 2.0f;
}
#endif

SIMD::Vec3f LightCurlNoise::Get(SIMD::Vec3f pos) const
{
	pos *= Scale;

	auto noise = [this](SIMD::Vec3f v) -> SIMD::Vec3f {
		v *= 8.0f;

#ifdef LIGHTCURL_OPTIMIZE
		auto Frac = [](SIMD::Float4 x) -> SIMD::Float4 {
			x -= x.Convert4i().Convert4f();
			SIMD::Int4 ofs = SIMD::Int4::ShiftRA<31>(x.Cast4i()) & SIMD::Int4(0x3F800000);
			return x + ofs.Cast4f();
		};

		SIMD::Float4 vf = Frac(v.s);
		SIMD::Int4 vi = (v.s - vf).Convert4i();
		SIMD::Int4 vi1 = vi & 0x07;
		SIMD::Int4 vi2 = (vi + 1) & 0x07;

		auto xi1 = static_cast<uint32_t>(vi1.GetX());
		auto yi1 = static_cast<uint32_t>(vi1.GetY());
		auto zi1 = static_cast<uint32_t>(vi1.GetZ());
		auto xi2 = static_cast<uint32_t>(vi2.GetX());
		auto yi2 = static_cast<uint32_t>(vi2.GetY());
		auto zi2 = static_cast<uint32_t>(vi2.GetZ());

		auto xf = vf.GetX();
		auto yf = vf.GetY();
		auto zf = vf.GetZ();

		const auto getValue = [this](uint32_t x, uint32_t y, uint32_t z) -> SIMD::Vec3f {
			return Unpack(vectorField_[z][y][x]);
		};

		auto v000 = getValue(xi1, yi1, zi1);
		auto v100 = getValue(xi2, yi1, zi1);
		auto v010 = getValue(xi1, yi2, zi1);
		auto v110 = getValue(xi2, yi2, zi1);
		auto v001 = getValue(xi1, yi1, zi2);
		auto v101 = getValue(xi2, yi1, zi2);
		auto v011 = getValue(xi1, yi2, zi2);
		auto v111 = getValue(xi2, yi2, zi2);

		auto v00 = v001 * (zf) + v000 * (1.0f - zf);
		auto v10 = v101 * (zf) + v100 * (1.0f - zf);
		auto v01 = v011 * (zf) + v010 * (1.0f - zf);
		auto v11 = v111 * (zf) + v110 * (1.0f - zf);

		auto v0 = v01 * (yf) + v00 * (1.0f - yf);
		auto v1 = v11 * (yf) + v10 * (1.0f - yf);

		return v1 * (xf) + v0 * (1.0f - xf);
#else
		auto xi = static_cast<int>(std::floor(v.GetX()));
		auto yi = static_cast<int>(std::floor(v.GetY()));
		auto zi = static_cast<int>(std::floor(v.GetZ()));

		auto xf = v.GetX() - xi;
		auto yf = v.GetY() - yi;
		auto zf = v.GetZ() - zi;

		const auto getValue = [this](const std::array<uint8_t, GridSize * GridSize * GridSize>& v, int x, int y, int z) -> float {
			x &= 0xff;
			y &= 0xff;
			z &= 0xff;

			return Unpack(v[(x % GridSize) + (y % GridSize) * GridSize + (z % GridSize) * GridSize * GridSize]);
		};

		const auto lerp = [this, &getValue](const std::array<uint8_t, GridSize * GridSize * GridSize>& v, int xi, int yi, int zi, float xf, float yf, float zf) -> float {
			auto v000 = getValue(v, xi, yi, zi);
			auto v100 = getValue(v, xi + 1, yi, zi);
			auto v010 = getValue(v, xi, yi + 1, zi);
			auto v110 = getValue(v, xi + 1, yi + 1, zi);
			auto v001 = getValue(v, xi, yi, zi + 1);
			auto v101 = getValue(v, xi + 1, yi, zi + 1);
			auto v011 = getValue(v, xi, yi + 1, zi + 1);
			auto v111 = getValue(v, xi + 1, yi + 1, zi + 1);

			auto v00 = v001 * (zf) + v000 * (1.0f - zf);
			auto v10 = v101 * (zf) + v100 * (1.0f - zf);
			auto v01 = v011 * (zf) + v010 * (1.0f - zf);
			auto v11 = v111 * (zf) + v110 * (1.0f - zf);

			auto v0 = v01 * (yf) + v00 * (1.0f - yf);
			auto v1 = v11 * (yf) + v10 * (1.0f - yf);

			return v1 * (xf) + v0 * (1.0f - xf);
		};

		auto dx = lerp(vectorField_x_, xi, yi, zi, xf, yf, zf);
		auto dy = lerp(vectorField_y_, xi, yi, zi, xf, yf, zf);
		auto dz = lerp(vectorField_z_, xi, yi, zi, xf, yf, zf);

		return SIMD::Vec3f(dx, dy, dz);
#endif
	};

	return noise(pos);
}

} // namespace Effekseer