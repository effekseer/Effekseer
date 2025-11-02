#include "Effekseer.Collisions.h"
#include <string.h>

namespace Effekseer
{

void CollisionsParameter::Load(unsigned char*& pos, int version)
{
	if (version >= Version18Alpha1)
	{
		int isEnabled = 0;
		memcpy(&isEnabled, pos, sizeof(int));
		pos += sizeof(int);

		IsEnabled = isEnabled > 0;

		memcpy(&Bounce, pos, sizeof(float));
		pos += sizeof(float);

		memcpy(&Height, pos, sizeof(float));
		pos += sizeof(float);

		if (version >= Version18Alpha2)
		{
			memcpy(&Friction, pos, sizeof(float));
			pos += sizeof(float);
		}

		memcpy(&WorldCoordinateSyatem, pos, sizeof(int));
		pos += sizeof(int);
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

		float friction = parameter.Friction;
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
			-velocityGlobal.GetY() * (1.0f + parameter.Bounce),
			-velocityGlobal.GetZ() * friction);

		return {velocityChange, positionDiff};
	}
	else
	{
		return {
			SIMD::Vec3f(0, 0, 0), SIMD::Vec3f(0, 0, 0)};
	}
}

} // namespace Effekseer
