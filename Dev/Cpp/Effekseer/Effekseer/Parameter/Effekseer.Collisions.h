#pragma once

#include "../SIMD/Vec3f.h"
#include "../Utils/Effekseer.BinaryVersion.h"

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
	float Friction = 0.0f;
	WorldCoordinateSyatemType WorldCoordinateSyatem = WorldCoordinateSyatemType::Local;
	void Load(unsigned char*& pos, int version);
};

struct CollisionsFunctions
{
	static std::tuple<SIMD::Vec3f, SIMD::Vec3f> Update(
		CollisionsState& state,
		const CollisionsParameter& parameter,
		const SIMD::Vec3f& nextPositionGlobal,
		const SIMD::Vec3f& positionGlobal,
		const SIMD::Vec3f& velocityGlobal,
		const SIMD::Vec3f& positionCenterLocal,
		float magnificationScale);
};

} // namespace Effekseer
