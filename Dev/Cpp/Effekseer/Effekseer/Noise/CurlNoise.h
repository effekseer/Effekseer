#ifndef __EFFEKSEER_CURL_NOISE_H__
#define __EFFEKSEER_CURL_NOISE_H__

#include "../SIMD/Effekseer.SIMD4f.h"
#include "../SIMD/Effekseer.SIMD4i.h"
#include "../SIMD/Effekseer.Vec3f.h"
#include "PerlinNoise.h"

namespace Effekseer
{

class CurlNoise
{
private:
	PerlinNoise xnoise_;
	PerlinNoise ynoise_;
	PerlinNoise znoise_;

public:
	float Scale = 1.0f;
	int32_t Octave = 2;

	CurlNoise(int32_t seed)
		: xnoise_(seed)
		, ynoise_(seed * (seed % 1949 + 5))
		, znoise_(seed * (seed % 3541 + 10))
	{
	}

	Vec3f Get(Vec3f pos) const
	{
		pos *= Scale;

		const float e = 1.0f / 1024.0f;

		const Vec3f dx = Vec3f(e, 0.0, 0.0);
		const Vec3f dy = Vec3f(0.0, e, 0.0);
		const Vec3f dz = Vec3f(0.0, 0.0, e);

		auto noise_x = [this](Vec3f v) -> Vec3f {
			return Vec3f(0.0f,
						 ynoise_.OctaveNoise(Octave, v),
						 znoise_.OctaveNoise(Octave, v));
		};

		auto noise_y = [this](Vec3f v) -> Vec3f {
			return Vec3f(xnoise_.OctaveNoise(Octave, v),
						 0.0f,
						 znoise_.OctaveNoise(Octave, v));
		};

		auto noise_z = [this](Vec3f v) -> Vec3f {
			return Vec3f(xnoise_.OctaveNoise(Octave, v),
						 ynoise_.OctaveNoise(Octave, v),
						 0.0f);
		};

		Vec3f p_x = noise_x(pos + dx) - noise_x(pos - dx);
		Vec3f p_y = noise_y(pos + dy) - noise_y(pos - dy);
		Vec3f p_z = noise_z(pos + dz) - noise_z(pos - dz);

		float x = p_y.GetZ() - p_z.GetY();
		float y = p_z.GetX() - p_x.GetZ();
		float z = p_x.GetY() - p_y.GetX();

		return Vec3f(x, y, z) * (1.0f / (e * 2.0f));
	}
};

} // namespace Effekseer

#endif