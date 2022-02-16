#pragma once

#include "../Effekseer.Base.h"

#include <stdint.h>

namespace Effekseer
{

struct ScalingState
{
	union
	{
		struct
		{
			SIMD::Vec3f scale;
		} fixed;

		struct
		{
			SIMD::Vec3f scale;
			SIMD::Vec3f velocity;
			SIMD::Vec3f acceleration;
		} random;

		InstanceEasing<SIMD::Vec3f> easing;

		/*
		struct
		{
			SIMD::Vec3f start;
			SIMD::Vec3f end;
		} easing;
		*/

		struct
		{
			float scale;
			float velocity;
			float acceleration;
		} single_random;

		InstanceEasing<float> single_easing;

		struct
		{
			SIMD::Vec3f offset;
		} fcruve;

		struct
		{
			float offset;
		} single_fcruve;
	};
};

enum class ParameterScalingType : int32_t
{
	ParameterScalingType_Fixed = 0,
	ParameterScalingType_PVA = 1,
	ParameterScalingType_Easing = 2,
	ParameterScalingType_SinglePVA = 3,
	ParameterScalingType_SingleEasing = 4,
	ParameterScalingType_FCurve = 5,
	ParameterScalingType_SingleFCurve = 6,

	ParameterScalingType_None = 0x7fffffff - 1,
};

struct ParameterScalingFixed
{
	int32_t RefEq = -1;

	Vector3D Position;
};

struct ParameterScalingPVA
{
	RefMinMax RefEqP;
	RefMinMax RefEqV;
	RefMinMax RefEqA;

	random_vector3d Position;
	random_vector3d Velocity;
	random_vector3d Acceleration;
};

struct ParameterScalingEasing
{
	RefMinMax RefEqS;
	RefMinMax RefEqE;
	easing_vector3d Position;
};

struct ParameterScalingSinglePVA
{
	random_float Position;
	random_float Velocity;
	random_float Acceleration;
};

struct ScalingParameter
{
	ParameterScalingType ScalingType;
	ParameterScalingFixed ScalingFixed;
	ParameterScalingPVA ScalingPVA;
	ParameterEasingSIMDVec3 ScalingEasing;
	// ParameterScalingEasing ScalingEasing;
	ParameterScalingSinglePVA ScalingSinglePVA;
	ParameterEasingFloat ScalingSingleEasing{Version16Alpha9, Version16Alpha9};
	FCurveVector3D* ScalingFCurve = nullptr;
	FCurveScalar* ScalingSingleFCurve = nullptr;
};

} // namespace Effekseer
