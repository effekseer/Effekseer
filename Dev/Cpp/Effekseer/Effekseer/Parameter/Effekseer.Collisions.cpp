#include "Effekseer.Collisions.h"
#include "../Effekseer.Random.h"
#include <string.h>
#include <algorithm>

namespace Effekseer
{

void CollisionsParameter::Load(unsigned char*& pos, int version)
{
	if (version < Version18Alpha2)
	{
		return;
	}

	int isEnabled = 0;
	memcpy(&isEnabled, pos, sizeof(int));
	pos += sizeof(int);

	IsEnabled = isEnabled > 0;

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
	int worldCandidate = 0;
	{
		auto currentPos = pos;

		memcpy(&lifetimeReductionCandidate, pos, sizeof(random_float));
		pos += sizeof(random_float);

		memcpy(&worldCandidate, pos, sizeof(int));
		pos += sizeof(int);

		if (worldCandidate != static_cast<int>(WorldCoordinateSyatemType::Local) &&
			worldCandidate != static_cast<int>(WorldCoordinateSyatemType::Global))
		{
			// Fallback to old binary layout without lifetime reduction.
			pos = currentPos;
			memcpy(&worldCandidate, pos, sizeof(int));
			pos += sizeof(int);
			lifetimeReductionCandidate.max = 0.0f;
			lifetimeReductionCandidate.min = 0.0f;
		}
	}

	Bounce = bounceCandidate;
	Height = heightCandidate;
	Friction = frictionCandidate;
	WorldCoordinateSyatem = static_cast<WorldCoordinateSyatemType>(worldCandidate);
	LifetimeReductionPerCollision = lifetimeReductionCandidate;
}

void CollisionsFunctions::Initialize(CollisionsState& state, const CollisionsParameter& parameter, RandObject& rand)
{
	if (parameter.IsEnabled)
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
	float magnificationScale)
{
	state.CollidedThisFrame = false;

	if (!parameter.IsEnabled)
	{
		return {
			SIMD::Vec3f(0, 0, 0), SIMD::Vec3f(0, 0, 0)};
	}

	auto nextPosition = nextPositionGlobal;
	auto currentPosition = positionGlobal;

	if (parameter.WorldCoordinateSyatem == WorldCoordinateSyatemType::Local)
	{
		nextPosition -= positionCenterLocal;
		currentPosition -= positionCenterLocal;
	}

	const auto height = parameter.Height * magnificationScale;

	if (nextPosition.GetY() < height && currentPosition.GetY() >= height)
	{
		auto diff = nextPosition - currentPosition;
		auto positionDiff = diff * (currentPosition.GetY() - height) / diff.GetY();

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

		return {velocityChange, positionDiff};
	}
	else
	{
		return {
			SIMD::Vec3f(0, 0, 0), SIMD::Vec3f(0, 0, 0)};
	}
}

} // namespace Effekseer
