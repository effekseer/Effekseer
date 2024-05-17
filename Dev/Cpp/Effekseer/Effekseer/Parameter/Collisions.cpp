#include "Collisions.h"
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

		memcpy(&WorldCoordinateSyatem, pos, sizeof(int));
		pos += sizeof(int);
	}
}

std::tuple<SIMD::Vec3f, SIMD::Vec3f> CollisionsFunctions::Update(
	CollisionsState& state,
	const CollisionsParameter& parameter,
	const SIMD::Vec3f& next_position_global,
	const SIMD::Vec3f& position_global,
	const SIMD::Vec3f& velocity_global,
	const SIMD::Vec3f& position_center_local,
	float magnification)
{
	if (!parameter.IsEnabled)
	{
		return {
			SIMD::Vec3f(0, 0, 0), SIMD::Vec3f(0, 0, 0)};
	}

	auto next_position = next_position_global;
	auto current_position = position_global;

	if (parameter.WorldCoordinateSyatem == WorldCoordinateSyatemType::Local)
	{
		next_position -= position_center_local;
		current_position -= position_center_local;
	}

	const auto height = parameter.Height * magnification;

	if (next_position.GetY() < height && current_position.GetY() >= height)
	{
		auto diff = next_position - current_position;
		auto pos_diff = diff * (current_position.GetY() - height) / diff.GetY();
		return {SIMD::Vec3f(0, -velocity_global.GetY() * (1.0f + parameter.Bounce), 0), pos_diff};
	}
	else
	{
		return {
			SIMD::Vec3f(0, 0, 0), SIMD::Vec3f(0, 0, 0)};
	}
}

} // namespace Effekseer
