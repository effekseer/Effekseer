#pragma once

#include "../Effekseer.InternalStruct.h"
#include "../Effekseer.Vector3D.h"
#include "../SIMD/Vec3f.h"
#include "../Utils/Effekseer.BinaryVersion.h"
#include <functional>

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
	bool CollidedThisFrame = false;
	float LifetimeReduction = 0.0f;
};

struct CollisionsParameter
{
	bool IsGroundCollisionEnabled = false;
	bool IsSceneCollisionWithExternal = false;
	random_float Bounce = {1.0f, 1.0f};
	float Height = 0.0f;
	random_float Friction = {0.0f, 0.0f};
	WorldCoordinateSyatemType WorldCoordinateSyatem = WorldCoordinateSyatemType::Local;
	random_float LifetimeReductionPerCollision = {0.0f, 0.0f};

	CollisionsParameter()
	{
		Bounce.max = 1.0f;
		Bounce.min = 1.0f;
		Friction.max = 0.0f;
		Friction.min = 0.0f;
		LifetimeReductionPerCollision.max = 0.0f;
		LifetimeReductionPerCollision.min = 0.0f;
	}

	void Load(unsigned char*& pos, int version);
};

struct CollisionsFunctions
{
	using ExternalCollisionCallback = std::function<bool(const Vector3D& startPosition, const Vector3D& endPosition, Vector3D& collisionPosition)>;

	static void Initialize(CollisionsState& state, const CollisionsParameter& parameter, RandObject& rand);

	static std::tuple<SIMD::Vec3f, SIMD::Vec3f> Update(
		CollisionsState& state,
		const CollisionsParameter& parameter,
		const SIMD::Vec3f& nextPositionGlobal,
		const SIMD::Vec3f& positionGlobal,
		const SIMD::Vec3f& velocityGlobal,
		const SIMD::Vec3f& positionCenterLocal,
		float magnificationScale,
		const ExternalCollisionCallback& externalCollision);
};

} // namespace Effekseer
