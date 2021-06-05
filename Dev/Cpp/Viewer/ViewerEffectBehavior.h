#pragma once

#include <stdint.h>
#include "efk.Base.h"

namespace Effekseer
{
namespace Tool
{

class ViewerEffectBehavior
{
public:
	int32_t CountX = 1;
	int32_t CountY = 1;
	int32_t CountZ = 1;

	int32_t TimeSpan = 0;

	uint8_t AllColorR = 255;
	uint8_t AllColorG = 255;
	uint8_t AllColorB = 255;
	uint8_t AllColorA = 255;

	float Distance = 5.0f;

	int32_t RemovedTime = 0xffff;

	float PositionX = 0.0f;
	float PositionY = 0.0f;
	float PositionZ = 0.0f;

	float RotationX = 0.0f;
	float RotationY = 0.0f;
	float RotationZ = 0.0f;

	float ScaleX = 1.0f;
	float ScaleY = 1.0f;
	float ScaleZ = 1.0f;

	float PositionVelocityX = 0.0f;
	float PositionVelocityY = 0.0f;
	float PositionVelocityZ = 0.0f;

	float RotationVelocityX = 0.0f;
	float RotationVelocityY = 0.0f;
	float RotationVelocityZ = 0.0f;

	float ScaleVelocityX = 0.0f;
	float ScaleVelocityY = 0.0f;
	float ScaleVelocityZ = 0.0f;

	float TargetPositionX = 0.0f;
	float TargetPositionY = 0.0f;
	float TargetPositionZ = 0.0f;

	float DynamicInput1 = 0.0f;
	float DynamicInput2 = 0.0f;
	float DynamicInput3 = 0.0f;
	float DynamicInput4 = 0.0f;

	float PlaybackSpeed = 1.0f;
};

} // namespace Tool
} // namespace Effekseer