#pragma once

#include "../Effekseer.InternalStruct.h"
#include "../SIMD/Vec3f.h"
#include "../Utils/Effekseer.BinaryVersion.h"

namespace Effekseer
{

class RandObject;

enum class WorldCoordinateSyatemType : int32_t
{
	Local,
	Global,
};

struct CollisionsState
{
	float Bounce = 1.0f;
	float Friction = 0.0f;
};

struct CollisionsParameter
{
	bool IsEnabled = false;
	random_float Bounce = {1.0f, 1.0f};
	float Height = 0.0f;
	random_float Friction = {0.0f, 0.0f};
	WorldCoordinateSyatemType WorldCoordinateSyatem = WorldCoordinateSyatemType::Local;

	CollisionsParameter()
	{
		Bounce.max = 1.0f;
		Bounce.min = 1.0f;
		Friction.max = 0.0f;
		Friction.min = 0.0f;
	}

	void Load(unsigned char*& pos, int version);
};

struct CollisionsFunctions
{
	static void Initialize(CollisionsState& state, const CollisionsParameter& parameter, RandObject& rand);

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
