#pragma once

#include <stdint.h>

namespace Effekseer::Tool
{

enum class CoordinateSystemType
{
	RH,
	LH,
};

enum class DeviceType
{
	DirectX11,
	OpenGL,
};

enum class DistortionType : int32_t
{
	Current,
	Effekseer120,
	Disabled,
};

enum class RenderingMethodType
{
	Normal,
	Wireframe,
	NormalWithWireframe,
	Overdraw,
};

struct PostEffectParameter
{
	bool BloomEnabled = false;
	float BoomIntensity = 1.0f;
	float BloomThreshold = 1.0f;
	float BloomSoftKnee = 0.5f;

	int ToneMapAlgorithm = 0;
	float ToneMapExposure = 1.0f;
};

struct ViewerParamater
{
	int32_t GuideWidth = 0;
	int32_t GuideHeight = 0;
	bool RendersGuide = false;

	Effekseer::Tool::DistortionType Distortion = Effekseer::Tool::DistortionType::Current;
	Effekseer::Tool::RenderingMethodType RenderingMode = Effekseer::Tool::RenderingMethodType::Normal;
};

struct ViewerEffectBehavior
{
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

	bool TriggerInput0 = false;
	bool TriggerInput1 = false;
	bool TriggerInput2 = false;
	bool TriggerInput3 = false;
};

class NativeStatus
{
private:
public:
	static bool IsDebugMode()
	{
#ifdef _DEBUG
		return true;
#else
		return false;
#endif
	}
};

} // namespace Effekseer::Tool