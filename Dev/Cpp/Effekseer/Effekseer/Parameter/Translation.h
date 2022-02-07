#pragma once

#include "../Effekseer.Base.h"
#include "../Effekseer.Curve.h"
#include "../Effekseer.EffectImplemented.h"
#include "../Effekseer.FCurves.h"
#include "../Effekseer.InternalStruct.h"
#include "../Effekseer.Setting.h"
#include "DynamicParameter.h"
#include "Easing.h"

#include <stdint.h>

namespace Effekseer
{
enum ParameterTranslationType
{
	ParameterTranslationType_Fixed = 0,
	ParameterTranslationType_PVA = 1,
	ParameterTranslationType_Easing = 2,
	ParameterTranslationType_FCurve = 3,
	ParameterTranslationType_NurbsCurve = 4,
	ParameterTranslationType_ViewOffset = 5,

	ParameterTranslationType_None = 0x7fffffff - 1,

	ParameterTranslationType_DWORD = 0x7fffffff,
};

struct ParameterTranslationFixed
{
	int32_t RefEq = -1;

	Vector3D Position;
};

struct ParameterTranslationPVA
{
	RefMinMax RefEqP;
	RefMinMax RefEqV;
	RefMinMax RefEqA;
	random_vector3d location;
	random_vector3d velocity;
	random_vector3d acceleration;
};

struct ParameterTranslationEasing
{
	RefMinMax RefEqS;
	RefMinMax RefEqE;
	easing_vector3d location;
};

struct ParameterTranslationNurbsCurve
{
	int32_t Index;
	float Scale;
	float MoveSpeed;
	int32_t LoopType;
};

struct ParameterTranslationViewOffset
{
	random_float distance;
};

struct InstanceTranslationState
{
	union
	{
		struct
		{
			SIMD::Vec3f location;
		} fixed;

		struct
		{
			SIMD::Vec3f location;
			SIMD::Vec3f velocity;
			SIMD::Vec3f acceleration;
		} random;

		InstanceEasing<SIMD::Vec3f> easing;

		struct
		{
			SIMD::Vec3f offset;
		} fcruve;

		struct
		{
			float distance;
		} view_offset;
	};
};

class TranslationParameter
{
public:
	ParameterTranslationType TranslationType = ParameterTranslationType_None;
	ParameterTranslationFixed TranslationFixed;
	ParameterTranslationPVA TranslationPVA;
	ParameterEasingSIMDVec3 TranslationEasing;
	std::unique_ptr<FCurveVector3D> TranslationFCurve = nullptr;
	ParameterTranslationNurbsCurve TranslationNurbsCurve;
	ParameterTranslationViewOffset TranslationViewOffset;

	void MakeLeftCoordinate()
	{
		if (TranslationType == ParameterTranslationType_Fixed)
		{
			TranslationFixed.Position.Z *= -1.0f;
		}
		else if (TranslationType == ParameterTranslationType_PVA)
		{
			TranslationPVA.location.max.z *= -1.0f;
			TranslationPVA.location.min.z *= -1.0f;
			TranslationPVA.velocity.max.z *= -1.0f;
			TranslationPVA.velocity.min.z *= -1.0f;
			TranslationPVA.acceleration.max.z *= -1.0f;
			TranslationPVA.acceleration.min.z *= -1.0f;
		}
		else if (TranslationType == ParameterTranslationType_Easing)
		{
			TranslationEasing.start.max.z *= -1.0f;
			TranslationEasing.start.min.z *= -1.0f;
			TranslationEasing.end.max.z *= -1.0f;
			TranslationEasing.end.min.z *= -1.0f;
		}
	}

	void Load(unsigned char*& pos, const EffectImplemented* ef)
	{
		int32_t size = 0;
		memcpy(&TranslationType, pos, sizeof(int));
		pos += sizeof(int);

		if (TranslationType == ParameterTranslationType_Fixed)
		{
			int32_t translationSize = 0;
			memcpy(&translationSize, pos, sizeof(int));
			pos += sizeof(int);

			if (ef->GetVersion() >= 14)
			{
				memcpy(&TranslationFixed, pos, sizeof(ParameterTranslationFixed));
			}
			else
			{
				memcpy(&(TranslationFixed.Position), pos, sizeof(float) * 3);

				// make invalid
				if (TranslationFixed.Position.X == 0.0f && TranslationFixed.Position.Y == 0.0f && TranslationFixed.Position.Z == 0.0f)
				{
					TranslationType = ParameterTranslationType_None;
					EffekseerPrintDebug("LocationType Change None\n");
				}
			}

			pos += translationSize;
		}
		else if (TranslationType == ParameterTranslationType_PVA)
		{
			if (ef->GetVersion() >= 14)
			{
				memcpy(&size, pos, sizeof(int));
				pos += sizeof(int);
				assert(size == sizeof(ParameterTranslationPVA));
				memcpy(&TranslationPVA, pos, size);
				pos += size;
			}
			else
			{
				memcpy(&size, pos, sizeof(int));
				pos += sizeof(int);
				memcpy(&TranslationPVA.location, pos, size);
				pos += size;
			}
		}
		else if (TranslationType == ParameterTranslationType_Easing)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);
			TranslationEasing.Load(pos, size, ef->GetVersion());
			pos += size;
		}
		else if (TranslationType == ParameterTranslationType_FCurve)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			TranslationFCurve = std::make_unique<FCurveVector3D>();
			pos += TranslationFCurve->Load(pos, ef->GetVersion());
		}
		else if (TranslationType == ParameterTranslationType_NurbsCurve)
		{
			memcpy(&TranslationNurbsCurve, pos, sizeof(ParameterTranslationNurbsCurve));
			pos += sizeof(ParameterTranslationNurbsCurve);
		}
		else if (TranslationType == ParameterTranslationType_ViewOffset)
		{
			memcpy(&TranslationViewOffset, pos, sizeof(ParameterTranslationViewOffset));
			pos += sizeof(ParameterTranslationViewOffset);
		}
	}

	void ApplyDynamicParameterToFixedLocation(
		InstanceTranslationState& translation_values,
		RandObject& rand,
		const Effect* effect,
		const InstanceGlobal* instanceGlobal,
		const Instance* m_pParent,
		const DynamicFactorParameter& dynamicFactor)
	{
		if (TranslationFixed.RefEq >= 0)
		{
			translation_values.fixed.location = ApplyEq(effect,
														instanceGlobal,
														m_pParent,
														&rand,
														TranslationFixed.RefEq,
														TranslationFixed.Position,
														dynamicFactor.Tra,
														dynamicFactor.TraInv);
		}
	}

	void InitializeTranslationState(
		InstanceTranslationState& translation_values,
		SIMD::Vec3f& prevPosition_,
		SIMD::Vec3f& steeringVec_,
		RandObject& rand,
		const Effect* effect,
		const InstanceGlobal* instanceGlobal,
		float m_LivingTime,
		float m_LivedTime,
		const Instance* m_pParent,
		const DynamicFactorParameter& dynamicFactor)
	{
		steeringVec_ = SIMD::Vec3f(0, 0, 0);

		// Translation
		if (TranslationType == ParameterTranslationType_Fixed)
		{
			translation_values.fixed.location = TranslationFixed.Position;
			ApplyDynamicParameterToFixedLocation(translation_values, rand, effect, instanceGlobal, m_pParent, dynamicFactor);

			prevPosition_ = translation_values.fixed.location;
		}
		else if (TranslationType == ParameterTranslationType_PVA)
		{
			auto rvl = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   TranslationPVA.RefEqP,
							   TranslationPVA.location,
							   dynamicFactor.Tra,
							   dynamicFactor.TraInv);
			translation_values.random.location = rvl.getValue(rand);

			auto rvv = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   TranslationPVA.RefEqV,
							   TranslationPVA.velocity,
							   dynamicFactor.Tra,
							   dynamicFactor.TraInv);
			translation_values.random.velocity = rvv.getValue(rand);

			auto rva = ApplyEq(effect,
							   instanceGlobal,
							   m_pParent,
							   &rand,
							   TranslationPVA.RefEqA,
							   TranslationPVA.acceleration,
							   dynamicFactor.Tra,
							   dynamicFactor.TraInv);
			translation_values.random.acceleration = rva.getValue(rand);

			prevPosition_ = translation_values.random.location;

			steeringVec_ = translation_values.random.velocity;
		}
		else if (TranslationType == ParameterTranslationType_Easing)
		{
			TranslationEasing.Init(translation_values.easing, effect, instanceGlobal, m_pParent, &rand, dynamicFactor.Tra, dynamicFactor.TraInv);

			prevPosition_ = translation_values.easing.start;
		}
		else if (TranslationType == ParameterTranslationType_FCurve)
		{
			assert(TranslationFCurve != nullptr);
			const auto coordinateSystem = effect->GetSetting()->GetCoordinateSystem();

			translation_values.fcruve.offset = TranslationFCurve->GetOffsets(rand);

			prevPosition_ = translation_values.fcruve.offset + TranslationFCurve->GetValues(m_LivingTime, m_LivedTime);

			if (coordinateSystem == CoordinateSystem::LH)
			{
				prevPosition_.SetZ(-prevPosition_.GetZ());
			}
		}
		else if (TranslationType == ParameterTranslationType_NurbsCurve)
		{
			// TODO refactoring
			auto& NurbsCurveParam = TranslationNurbsCurve;
			CurveRef curve = static_cast<CurveRef>(effect->GetCurve(NurbsCurveParam.Index));
			if (curve != nullptr)
			{
				float moveSpeed = NurbsCurveParam.MoveSpeed;
				int32_t loopType = NurbsCurveParam.LoopType;

				float speed = 1.0f / (curve->GetLength() * NurbsCurveParam.Scale);

				float t = speed * m_LivingTime * moveSpeed;

				switch (loopType)
				{
				default:
				case 0:
					t = fmod(t, 1.0f);
					break;

				case 1:
					if (t > 1.0f)
					{
						t = 1.0f;
					}
					break;
				}

				prevPosition_ = curve->CalcuratePoint(t, NurbsCurveParam.Scale * effect->GetMaginification());
			}
			else
			{
				prevPosition_ = {0, 0, 0};
			}
		}
		else if (TranslationType == ParameterTranslationType_ViewOffset)
		{
			translation_values.view_offset.distance = TranslationViewOffset.distance.getValue(rand);
			prevPosition_ = {0, 0, 0};
		}
	}

	void CalculateTranslationState(
		InstanceTranslationState& translation_values,
		SIMD::Vec3f& localPosition,
		RandObject& rand,
		const Effect* effect,
		const InstanceGlobal* instanceGlobal,
		float m_LivingTime,
		float m_LivedTime,
		const Instance* m_pParent,
		CoordinateSystem coordinateSystem,
		const DynamicFactorParameter& dynamicFactor)
	{
		if (TranslationType == ParameterTranslationType_None)
		{
			localPosition = {0, 0, 0};
		}
		else if (TranslationType == ParameterTranslationType_Fixed)
		{
			ApplyDynamicParameterToFixedLocation(translation_values, rand, effect, instanceGlobal, m_pParent, dynamicFactor);

			localPosition = translation_values.fixed.location;
		}
		else if (TranslationType == ParameterTranslationType_PVA)
		{
			// 現在位置 = 初期座標 + (初期速度 * t) + (初期加速度 * t * t * 0.5)
			localPosition = translation_values.random.location + (translation_values.random.velocity * m_LivingTime) +
							(translation_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
		}
		else if (TranslationType == ParameterTranslationType_Easing)
		{
			localPosition = TranslationEasing.GetValue(translation_values.easing, m_LivingTime / m_LivedTime);
			//localPosition = m_pEffectNode->TranslationEasing.location.getValue(
			//	translation_values.easing.start, translation_values.easing.end, m_LivingTime / m_LivedTime);
		}
		else if (TranslationType == ParameterTranslationType_FCurve)
		{
			assert(TranslationFCurve != nullptr);
			auto fcurve = TranslationFCurve->GetValues(m_LivingTime, m_LivedTime);
			localPosition = fcurve + translation_values.fcruve.offset;

			if (coordinateSystem == CoordinateSystem::LH)
			{
				localPosition.SetZ(-localPosition.GetZ());
			}
		}
		else if (TranslationType == ParameterTranslationType_NurbsCurve)
		{
			auto& NurbsCurveParam = TranslationNurbsCurve;
			CurveRef curve = static_cast<CurveRef>(effect->GetCurve(NurbsCurveParam.Index));
			if (curve != nullptr)
			{
				float moveSpeed = NurbsCurveParam.MoveSpeed;
				int32_t loopType = NurbsCurveParam.LoopType;

				float speed = 1.0f / (curve->GetLength() * NurbsCurveParam.Scale);

				float t = speed * m_LivingTime * moveSpeed;

				switch (loopType)
				{
				default:
				case 0:
					t = fmod(t, 1.0f);
					break;

				case 1:
					if (t > 1.0f)
					{
						t = 1.0f;
					}
					break;
				}

				localPosition = curve->CalcuratePoint(t, NurbsCurveParam.Scale * effect->GetMaginification());
			}
			else
			{
				localPosition = {0, 0, 0};
			}
		}
		else if (TranslationType == ParameterTranslationType_ViewOffset)
		{
			localPosition = {0, 0, 0};
		}
	}
};

} // namespace Effekseer