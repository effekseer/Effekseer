#pragma once

#include "../Effekseer.Base.h"

#include <stdint.h>

namespace Effekseer
{

struct RotationState
{
	union
	{
		struct
		{
			SIMD::Vec3f rotation;
		} fixed;

		struct
		{
			SIMD::Vec3f rotation;
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
			float rotation;
			SIMD::Vec3f axis;

			union
			{
				struct
				{
					float rotation;
					float velocity;
					float acceleration;
				} random;

				InstanceEasing<float> easing;
			};
		} axis;

		struct
		{
			SIMD::Vec3f offset;
		} fcruve;
	};
};


enum class ParameterRotationType : int32_t
{
	ParameterRotationType_Fixed = 0,
	ParameterRotationType_PVA = 1,
	ParameterRotationType_Easing = 2,
	ParameterRotationType_AxisPVA = 3,
	ParameterRotationType_AxisEasing = 4,

	ParameterRotationType_FCurve = 5,

	ParameterRotationType_None = 0x7fffffff - 1,
};

struct ParameterRotationFixed
{
	int32_t RefEq = -1;
	Vector3D Position;
};

struct ParameterRotationPVA
{
	RefMinMax RefEqP;
	RefMinMax RefEqV;
	RefMinMax RefEqA;
	random_vector3d rotation;
	random_vector3d velocity;
	random_vector3d acceleration;
};

struct ParameterRotationEasing
{
	RefMinMax RefEqS;
	RefMinMax RefEqE;
	easing_vector3d rotation;
};

struct ParameterRotationAxisPVA
{
	random_vector3d axis;
	random_float rotation;
	random_float velocity;
	random_float acceleration;
};

struct ParameterRotationAxisEasing
{
	random_vector3d axis;
	ParameterEasingFloat easing{Version16Alpha9, Version16Alpha9};
};

struct RotationParameter
{
	ParameterRotationType RotationType;
	ParameterRotationFixed RotationFixed;
	ParameterRotationPVA RotationPVA;

	ParameterEasingSIMDVec3 RotationEasing;
	FCurveVector3D* RotationFCurve = nullptr;

	ParameterRotationAxisPVA RotationAxisPVA;
	ParameterRotationAxisEasing RotationAxisEasing;
};

}
