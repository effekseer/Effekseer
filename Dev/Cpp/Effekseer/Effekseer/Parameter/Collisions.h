#pragma once

#include "../SIMD/Vec3f.h"
#include "../Utils/BinaryVersion.h"

namespace Effekseer
{

enum class WorldCoordinateSyatemType : int32_t
{
	Local,
	Global,
};

struct CollisionsState
{
};

struct CollisionsParameter
{
	bool IsEnabled = false;
	float Bounce = 1.0f;
	float Height = 0.0f;
	WorldCoordinateSyatemType WorldCoordinateSyatem = WorldCoordinateSyatemType::Local;
	void Load(unsigned char*& pos, int version);
};

struct CollisionsFunctions
{
	static std::tuple<SIMD::Vec3f, SIMD::Vec3f> Update(
		CollisionsState& state,
		const CollisionsParameter& parameter,
		const SIMD::Vec3f& next_position_global,
		const SIMD::Vec3f& position_global,
		const SIMD::Vec3f& velocity_global,
		const SIMD::Vec3f& position_center_local,
		float magnificationScale);
};

} // namespace Effekseer
