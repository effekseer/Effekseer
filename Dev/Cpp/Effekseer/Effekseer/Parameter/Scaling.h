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
	ParameterScalingType ScalingType = ParameterScalingType::ParameterScalingType_None;
	ParameterScalingFixed ScalingFixed;
	ParameterScalingPVA ScalingPVA;
	ParameterEasingSIMDVec3 ScalingEasing;
	// ParameterScalingEasing ScalingEasing;
	ParameterScalingSinglePVA ScalingSinglePVA;
	ParameterEasingFloat ScalingSingleEasing{Version16Alpha9, Version16Alpha9};
	std::unique_ptr<FCurveVector3D> ScalingFCurve = nullptr;
	std::unique_ptr<FCurveScalar> ScalingSingleFCurve = nullptr;

	void Load(unsigned char*& pos, int version)
	{
		int32_t size = 0;

		memcpy(&ScalingType, pos, sizeof(int));
		pos += sizeof(int);
		EffekseerPrintDebug("ScalingType %d\n", ScalingType);
		if (ScalingType == ParameterScalingType::ParameterScalingType_Fixed)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			if (version >= 14)
			{
				assert(size == sizeof(ParameterScalingFixed));
				memcpy(&ScalingFixed, pos, size);
				pos += size;
			}
			else
			{
				memcpy(&ScalingFixed.Position, pos, size);
				pos += size;
			}

			// make invalid
			if (ScalingFixed.RefEq < 0 && ScalingFixed.Position.X == 1.0f && ScalingFixed.Position.Y == 1.0f &&
				ScalingFixed.Position.Z == 1.0f)
			{
				ScalingType = ParameterScalingType::ParameterScalingType_None;
				EffekseerPrintDebug("ScalingType Change None\n");
			}
		}
		else if (ScalingType == ParameterScalingType::ParameterScalingType_PVA)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);
			if (version >= 14)
			{
				assert(size == sizeof(ParameterScalingPVA));
				memcpy(&ScalingPVA, pos, size);
			}
			else
			{
				memcpy(&ScalingPVA.Position, pos, size);
			}
			pos += size;
		}
		else if (ScalingType == ParameterScalingType::ParameterScalingType_Easing)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);
			ScalingEasing.Load(pos, size, version);
			pos += size;
		}
		else if (ScalingType == ParameterScalingType::ParameterScalingType_SinglePVA)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);
			assert(size == sizeof(ParameterScalingSinglePVA));
			memcpy(&ScalingSinglePVA, pos, size);
			pos += size;
		}
		else if (ScalingType == ParameterScalingType::ParameterScalingType_SingleEasing)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			ScalingSingleEasing.Load(pos, size, version);
			pos += size;
		}
		else if (ScalingType == ParameterScalingType::ParameterScalingType_FCurve)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			ScalingFCurve = std::make_unique<FCurveVector3D>();
			pos += ScalingFCurve->Load(pos, version);
			ScalingFCurve->X.SetDefaultValue(1.0f);
			ScalingFCurve->Y.SetDefaultValue(1.0f);
			ScalingFCurve->Z.SetDefaultValue(1.0f);
		}
		else if (ScalingType == ParameterScalingType::ParameterScalingType_SingleFCurve)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			ScalingSingleFCurve = std::make_unique<FCurveScalar>();
			pos += ScalingSingleFCurve->Load(pos, version);
			ScalingSingleFCurve->S.SetDefaultValue(1.0f);
		}
	}
};

struct ScalingFunctions
{

	static void ApplyDynamicParameterToFixedScaling(ScalingState& scaling_values,
													const ScalingParameter& scalingParam,
													RandObject& rand,
													const Effect* effect,
													const InstanceGlobal* instanceGlobal,
													const Instance* m_pParent,
													const DynamicFactorParameter& dynamicFactor)
	{
		if (scalingParam.ScalingFixed.RefEq >= 0)
		{
			scaling_values.fixed.scale = ApplyEq(effect,
												 instanceGlobal,
												 m_pParent,
												 &rand,
												 scalingParam.ScalingFixed.RefEq,
												 scalingParam.ScalingFixed.Position,
												 dynamicFactor.Scale,
												 dynamicFactor.ScaleInv);
		}
	}

	static void InitScaling(ScalingState& scaling_values, const ScalingParameter& scalingParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
	{
		if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Fixed)
		{
			scaling_values.fixed.scale = scalingParam.ScalingFixed.Position;
			ApplyDynamicParameterToFixedScaling(scaling_values, scalingParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_PVA)
		{
			auto rvl = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   scalingParam.ScalingPVA.RefEqP,
							   scalingParam.ScalingPVA.Position,
							   dynamicFactor.Scale,
							   dynamicFactor.ScaleInv);
			auto rvv = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   scalingParam.ScalingPVA.RefEqV,
							   scalingParam.ScalingPVA.Velocity,
							   dynamicFactor.Scale,
							   dynamicFactor.ScaleInv);
			auto rva = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   scalingParam.ScalingPVA.RefEqA,
							   scalingParam.ScalingPVA.Acceleration,
							   dynamicFactor.Scale,
							   dynamicFactor.ScaleInv);

			scaling_values.random.scale = rvl.getValue(rand);
			scaling_values.random.velocity = rvv.getValue(rand);
			scaling_values.random.acceleration = rva.getValue(rand);
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Easing)
		{
			scalingParam.ScalingEasing.Init(scaling_values.easing, effect, instanceGlobal, m_pParent, &rand, dynamicFactor.Scale, dynamicFactor.ScaleInv);
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SinglePVA)
		{
			scaling_values.single_random.scale = scalingParam.ScalingSinglePVA.Position.getValue(rand);
			scaling_values.single_random.velocity = scalingParam.ScalingSinglePVA.Velocity.getValue(rand);
			scaling_values.single_random.acceleration = scalingParam.ScalingSinglePVA.Acceleration.getValue(rand);
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleEasing)
		{
			scalingParam.ScalingSingleEasing.Init(scaling_values.single_easing, effect, instanceGlobal, m_pParent, &rand);
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_FCurve)
		{
			assert(scalingParam.ScalingFCurve != nullptr);

			scaling_values.fcruve.offset = scalingParam.ScalingFCurve->GetOffsets(rand);
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleFCurve)
		{
			assert(scalingParam.ScalingSingleFCurve != nullptr);

			scaling_values.single_fcruve.offset = scalingParam.ScalingSingleFCurve->S.GetOffset(rand);
		}
	}

	static SIMD::Vec3f UpdateScaling(ScalingState& scaling_values, const ScalingParameter& scalingParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
	{
		SIMD::Vec3f localScaling;

		if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_None)
		{
			localScaling = {1.0f, 1.0f, 1.0f};
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Fixed)
		{
			ApplyDynamicParameterToFixedScaling(scaling_values, scalingParam, rand, effect, instanceGlobal, m_pParent, dynamicFactor);

			localScaling = scaling_values.fixed.scale;
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_PVA)
		{
			localScaling = scaling_values.random.scale + (scaling_values.random.velocity * m_LivingTime) +
						   (scaling_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_Easing)
		{
			localScaling = scalingParam.ScalingEasing.GetValue(scaling_values.easing, Clamp(m_LivingTime / m_LivedTime, 1.0F, 0.0F));
			/*
			localScaling = m_pEffectNode->ScalingEasing.Position.getValue(
				scaling_values.easing.start, scaling_values.easing.end, m_LivingTime / m_LivedTime);
			*/
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SinglePVA)
		{
			float s = scaling_values.single_random.scale + scaling_values.single_random.velocity * m_LivingTime +
					  scaling_values.single_random.acceleration * m_LivingTime * m_LivingTime * 0.5f;
			localScaling = {s, s, s};
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleEasing)
		{
			float s = scalingParam.ScalingSingleEasing.GetValue(scaling_values.single_easing, Clamp(m_LivingTime / m_LivedTime, 1.0F, 0.0F));
			localScaling = {s, s, s};
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_FCurve)
		{
			assert(scalingParam.ScalingFCurve != nullptr);
			auto fcurve = scalingParam.ScalingFCurve->GetValues(m_LivingTime, m_LivedTime);
			localScaling = fcurve + scaling_values.fcruve.offset;
		}
		else if (scalingParam.ScalingType == ParameterScalingType::ParameterScalingType_SingleFCurve)
		{
			assert(scalingParam.ScalingSingleFCurve != nullptr);
			auto s = scalingParam.ScalingSingleFCurve->GetValues(m_LivingTime, m_LivedTime) + scaling_values.single_fcruve.offset;
			localScaling = {s, s, s};
		}

		return localScaling;
	}
};

} // namespace Effekseer
