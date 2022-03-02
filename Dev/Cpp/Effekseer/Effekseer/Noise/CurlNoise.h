#ifndef __EFFEKSEER_CURL_NOISE_H__
#define __EFFEKSEER_CURL_NOISE_H__

#include "../SIMD/Float4.h"
#include "../SIMD/Int4.h"
#include "../SIMD/Vec3f.h"
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
	const float Scale = 1.0f;
	const int32_t Octave = 2;

	CurlNoise(int32_t seed, float scale, int32_t octave)
		: xnoise_(seed)
		, ynoise_(seed * (seed % 1949 + 5))
		, znoise_(seed * (seed % 3541 + 10))
		, Scale(scale)
		, Octave(octave)
	{
	}

	SIMD::Vec3f Get(SIMD::Vec3f pos) const;
};

class LightCurlNoise
{
private:
	static const int32_t GridSize = 8;
	static const int32_t GridBits = 8;
	static const int32_t GridBitMask = (1 << GridBits) - 1;
	uint32_t vectorField_[GridSize][GridSize][GridSize];

	uint32_t Pack(const SIMD::Vec3f v) const;

	SIMD::Vec3f Unpack(const uint32_t v) const;

public:
	const float Scale{};

	LightCurlNoise(int32_t seed, float scale, int32_t octave);

	SIMD::Vec3f Get(SIMD::Vec3f pos) const;
};

} // namespace Effekseer

#endif