#include "Effekseer.Collisions.h"
#include "../Effekseer.Random.h"
#include <algorithm>
#include <string.h>

namespace Effekseer
{

void CollisionsParameter::Load(unsigned char*& pos, int version)
{
	if (version < Version18Alpha2)
	{
		return;
	}

	int collisionEnabled = 0;
	memcpy(&collisionEnabled, pos, sizeof(int));
	pos += sizeof(int);

	int sceneCollisionEnabled = 0;
	memcpy(&sceneCollisionEnabled, pos, sizeof(int));
	pos += sizeof(int);

	IsGroundCollisionEnabled = collisionEnabled > 0;
	IsSceneCollisionWithExternal = sceneCollisionEnabled > 0;

	random_float bounceCandidate{};
	memcpy(&bounceCandidate, pos, sizeof(random_float));
	pos += sizeof(random_float);

	float heightCandidate = 0.0f;
	memcpy(&heightCandidate, pos, sizeof(float));
	pos += sizeof(float);

	random_float frictionCandidate{};
	memcpy(&frictionCandidate, pos, sizeof(random_float));
	pos += sizeof(random_float);

	random_float lifetimeReductionCandidate{};

	memcpy(&lifetimeReductionCandidate, pos, sizeof(random_float));
	pos += sizeof(random_float);

	int worldCandidate = 0;
	memcpy(&worldCandidate, pos, sizeof(int));
	pos += sizeof(int);

	Bounce = bounceCandidate;
	Height = heightCandidate;
	Friction = frictionCandidate;
	WorldCoordinateSyatem = static_cast<WorldCoordinateSyatemType>(worldCandidate);
	LifetimeReductionPerCollision = lifetimeReductionCandidate;
}

void CollisionsFunctions::Initialize(CollisionsState& state, const CollisionsParameter& parameter, RandObject& rand)
{
	if (parameter.IsGroundCollisionEnabled || parameter.IsSceneCollisionWithExternal)
	{
		state.Bounce = parameter.Bounce.getValue(rand);
		state.Friction = parameter.Friction.getValue(rand);
		state.LifetimeReduction = std::max(parameter.LifetimeReductionPerCollision.getValue(rand), 0.0f);
		state.CollidedThisFrame = false;

		if (state.Friction < 0.0f)
		{
			state.Friction = 0.0f;
		}
		else if (state.Friction > 1.0f)
		{
			state.Friction = 1.0f;
		}
	}
}

std::tuple<SIMD::Vec3f, SIMD::Vec3f> CollisionsFunctions::Update(
	CollisionsState& state,
	const CollisionsParameter& parameter,
	const SIMD::Vec3f& nextPositionGlobal,
	const SIMD::Vec3f& positionGlobal,
	const SIMD::Vec3f& velocityGlobal,
	const SIMD::Vec3f& positionCenterLocal,
	float magnificationScale,
	const ExternalCollisionCallback& externalCollision)
{
	state.CollidedThisFrame = false;

	if (!parameter.IsGroundCollisionEnabled && !parameter.IsSceneCollisionWithExternal)
	{
		return {
			SIMD::Vec3f(0, 0, 0), SIMD::Vec3f(0, 0, 0)};
	}

	auto nextPosition = nextPositionGlobal;
	auto currentPosition = positionGlobal;

	const auto currentPositionGlobal = positionGlobal;
	const auto nextPositionGlobalValue = nextPositionGlobal;

	if (parameter.WorldCoordinateSyatem == WorldCoordinateSyatemType::Local)
	{
		nextPosition -= positionCenterLocal;
		currentPosition -= positionCenterLocal;
	}

	const auto height = parameter.Height * magnificationScale;
	const auto diffGlobal = nextPositionGlobalValue - currentPositionGlobal;

	const auto resolveCollision = [&](const SIMD::Vec3f& collisionPosition)
	{
		float friction = state.Friction;
		if (friction < 0.0f)
		{
			friction = 0.0f;
		}
		if (friction > 1.0f)
		{
			friction = 1.0f;
		}

		SIMD::Vec3f velocityChange(
			-velocityGlobal.GetX() * friction,
			-velocityGlobal.GetY() * (1.0f + state.Bounce),
			-velocityGlobal.GetZ() * friction);

		state.CollidedThisFrame = true;

		return std::make_tuple(velocityChange, currentPositionGlobal - collisionPosition);
	};

	if (parameter.IsSceneCollisionWithExternal)
	{
		if (externalCollision)
		{
			Vector3D start(currentPositionGlobal.GetX(), currentPositionGlobal.GetY(), currentPositionGlobal.GetZ());
			Vector3D end(nextPositionGlobalValue.GetX(), nextPositionGlobalValue.GetY(), nextPositionGlobalValue.GetZ());
			Vector3D hit;
			if (externalCollision(start, end, hit))
			{
				SIMD::Vec3f collisionPosition(hit.X, hit.Y, hit.Z);
				return resolveCollision(collisionPosition);
			}
		}
	}

	if (parameter.IsGroundCollisionEnabled && nextPosition.GetY() < height && currentPosition.GetY() >= height && diffGlobal.GetY() != 0.0f)
	{
		const auto positionDiffRate = (currentPosition.GetY() - height) / diffGlobal.GetY();
		const auto collisionPosition = currentPositionGlobal - diffGlobal * positionDiffRate;

		return resolveCollision(collisionPosition);
	}
	else
	{
		return {
			SIMD::Vec3f(0, 0, 0), SIMD::Vec3f(0, 0, 0)};
	}
}

} // namespace Effekseer
