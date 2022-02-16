#pragma once

#include "../Effekseer.Base.h"
#include "../Effekseer.FCurves.h"
#include "../Utils/Compatiblity.h"
#include "Easing.h"

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
	ParameterRotationType RotationType = ParameterRotationType::ParameterRotationType_Fixed;
	ParameterRotationFixed RotationFixed;
	ParameterRotationPVA RotationPVA;

	ParameterEasingSIMDVec3 RotationEasing;
	FCurveVector3D* RotationFCurve = nullptr;

	ParameterRotationAxisPVA RotationAxisPVA;
	ParameterRotationAxisEasing RotationAxisEasing;

	void Load(unsigned char*& pos, int version)
	{
		int32_t size = 0;

		memcpy(&RotationType, pos, sizeof(int));
		pos += sizeof(int);
		EffekseerPrintDebug("RotationType %d\n", RotationType);
		if (RotationType == ParameterRotationType::ParameterRotationType_None)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			if (version >= 14)
			{
				assert(size == sizeof(ParameterRotationFixed));
				memcpy(&RotationFixed, pos, size);
			}
			else
			{
				memcpy(&RotationFixed.Position, pos, size);
			}
			pos += size;

			// make invalid
			if (RotationFixed.RefEq < 0 && RotationFixed.Position.X == 0.0f && RotationFixed.Position.Y == 0.0f &&
				RotationFixed.Position.Z == 0.0f)
			{
				RotationType = ParameterRotationType::ParameterRotationType_None;
				EffekseerPrintDebug("RotationType Change None\n");
			}
		}
		else if (RotationType == ParameterRotationType::ParameterRotationType_PVA)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);
			if (version >= 14)
			{
				assert(size == sizeof(ParameterRotationPVA));
				memcpy(&RotationPVA, pos, size);
			}
			else
			{
				memcpy(&RotationPVA.rotation, pos, size);
			}
			pos += size;
		}
		else if (RotationType == ParameterRotationType::ParameterRotationType_Easing)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);
			RotationEasing.Load(pos, size, version);
			pos += size;
		}
		else if (RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);
			assert(size == sizeof(ParameterRotationAxisPVA));
			memcpy(&RotationAxisPVA, pos, size);
			pos += size;
		}
		else if (RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			memcpy(&RotationAxisEasing.axis, pos, sizeof(RotationAxisEasing.axis));
			pos += sizeof(RotationAxisEasing.axis);

			LoadFloatEasing(RotationAxisEasing.easing, pos, version);
		}
		else if (RotationType == ParameterRotationType::ParameterRotationType_FCurve)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			RotationFCurve = new FCurveVector3D();
			pos += RotationFCurve->Load(pos, version);
		}
	}
};

struct RotationFunctions
{
	static void ApplyDynamicParameterToFixedRotation(RotationState& rotation_values,
													 const RotationParameter& rotationParam,
													 RandObject& rand,
													 const Effect* effect,
													 const InstanceGlobal* instanceGlobal,
													 const Instance* m_pParent,
													 const DynamicFactorParameter& dynamicFactor)
	{
		if (rotationParam.RotationFixed.RefEq >= 0)
		{
			rotation_values.fixed.rotation = ApplyEq(effect,
													 instanceGlobal,
													 m_pParent,
													 &rand,
													 rotationParam.RotationFixed.RefEq,
													 rotationParam.RotationFixed.Position,
													 dynamicFactor.Rot,
													 dynamicFactor.RotInv);
		}
	}

	static void InitRotation(RotationState& rotation_values, const RotationParameter& rotationParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
	{
		// Rotation
		if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed)
		{
			rotation_values.fixed.rotation = rotationParam.RotationFixed.Position;
			ApplyDynamicParameterToFixedRotation(rotation_values, rotationParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA)
		{
			auto rvl = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   rotationParam.RotationPVA.RefEqP,
							   rotationParam.RotationPVA.rotation,
							   dynamicFactor.Rot,
							   dynamicFactor.RotInv);
			auto rvv = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   rotationParam.RotationPVA.RefEqV,
							   rotationParam.RotationPVA.velocity,
							   dynamicFactor.Rot,
							   dynamicFactor.RotInv);
			auto rva = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   rotationParam.RotationPVA.RefEqA,
							   rotationParam.RotationPVA.acceleration,
							   dynamicFactor.Rot,
							   dynamicFactor.RotInv);

			rotation_values.random.rotation = rvl.getValue(rand);
			rotation_values.random.velocity = rvv.getValue(rand);
			rotation_values.random.acceleration = rva.getValue(rand);
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing)
		{
			rotationParam.RotationEasing.Init(rotation_values.easing, effect, instanceGlobal, m_pParent, &rand, dynamicFactor.Rot, dynamicFactor.RotInv);
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
		{
			rotation_values.axis.random.rotation = rotationParam.RotationAxisPVA.rotation.getValue(rand);
			rotation_values.axis.random.velocity = rotationParam.RotationAxisPVA.velocity.getValue(rand);
			rotation_values.axis.random.acceleration = rotationParam.RotationAxisPVA.acceleration.getValue(rand);
			rotation_values.axis.rotation = rotation_values.axis.random.rotation;
			rotation_values.axis.axis = rotationParam.RotationAxisPVA.axis.getValue(rand);
			if (rotation_values.axis.axis.GetLength() < 0.001f)
			{
				rotation_values.axis.axis = SIMD::Vec3f(0, 1, 0);
			}
			rotation_values.axis.axis.Normalize();
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
		{
			rotation_values.axis.easing.start = rotationParam.RotationAxisEasing.easing.start.getValue(rand);
			rotation_values.axis.easing.end = rotationParam.RotationAxisEasing.easing.end.getValue(rand);
			rotation_values.axis.rotation = rotation_values.axis.easing.start;
			rotation_values.axis.axis = rotationParam.RotationAxisEasing.axis.getValue(rand);
			if (rotation_values.axis.axis.GetLength() < 0.001f)
			{
				rotation_values.axis.axis = SIMD::Vec3f(0, 1, 0);
			}
			rotation_values.axis.axis.Normalize();
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
		{
			assert(rotationParam.RotationFCurve != nullptr);

			rotation_values.fcruve.offset = rotationParam.RotationFCurve->GetOffsets(rand);
		}
	}

	static void CalculateRotation(SIMD::Vec3f& localAngle, RotationState& rotation_values, const RotationParameter& rotationParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
	{

		/* 回転の更新(時間から直接求めれるよう対応済み) */
		if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_None)
		{
			localAngle = {0, 0, 0};
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed)
		{
			ApplyDynamicParameterToFixedRotation(rotation_values, rotationParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);
			localAngle = rotation_values.fixed.rotation;
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA)
		{
			/* 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)*/
			localAngle = rotation_values.random.rotation + (rotation_values.random.velocity * m_LivingTime) +
						 (rotation_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing)
		{
			localAngle = rotationParam.RotationEasing.GetValue(rotation_values.easing, m_LivingTime / m_LivedTime);
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
		{
			rotation_values.axis.rotation = rotation_values.axis.random.rotation + rotation_values.axis.random.velocity * m_LivingTime +
											rotation_values.axis.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f);
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
		{
			rotation_values.axis.rotation = rotationParam.RotationAxisEasing.easing.GetValue(rotation_values.axis.easing, m_LivingTime / m_LivedTime);
		}
		else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
		{
			assert(rotationParam.RotationFCurve != nullptr);
			auto fcurve = rotationParam.RotationFCurve->GetValues(m_LivingTime, m_LivedTime);
			localAngle = fcurve + rotation_values.fcruve.offset;
		}
	}
};

} // namespace Effekseer
