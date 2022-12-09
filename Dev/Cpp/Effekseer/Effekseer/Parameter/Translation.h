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

	SIMD::Vec3f prevLocation;
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

	void Magnify(float scale, DynamicFactorParameter& DynamicFactor)
	{
		DynamicFactor.Tra[0] *= scale;
		DynamicFactor.Tra[1] *= scale;
		DynamicFactor.Tra[2] *= scale;

		if (TranslationType == ParameterTranslationType_Fixed)
		{
			TranslationFixed.Position *= scale;
		}
		else if (TranslationType == ParameterTranslationType_PVA)
		{
			TranslationPVA.location.min *= scale;
			TranslationPVA.location.max *= scale;
			TranslationPVA.velocity.min *= scale;
			TranslationPVA.velocity.max *= scale;
			TranslationPVA.acceleration.min *= scale;
			TranslationPVA.acceleration.max *= scale;
		}
		else if (TranslationType == ParameterTranslationType_Easing)
		{
			TranslationEasing.start.min *= scale;
			TranslationEasing.start.max *= scale;
			TranslationEasing.end.min *= scale;
			TranslationEasing.end.max *= scale;
		}
		else if (TranslationType == ParameterTranslationType_FCurve)
		{
			TranslationFCurve->X.Maginify(scale);
			TranslationFCurve->Y.Maginify(scale);
			TranslationFCurve->Z.Maginify(scale);
		}
	}

	void Load(unsigned char*& pos, int version)
	{
		int32_t size = 0;
		memcpy(&TranslationType, pos, sizeof(int));
		pos += sizeof(int);

		if (TranslationType == ParameterTranslationType_Fixed)
		{
			int32_t translationSize = 0;
			memcpy(&translationSize, pos, sizeof(int));
			pos += sizeof(int);

			if (version >= 14)
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
			if (version >= 14)
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
			TranslationEasing.Load(pos, size, version);
			pos += size;
		}
		else if (TranslationType == ParameterTranslationType_FCurve)
		{
			memcpy(&size, pos, sizeof(int));
			pos += sizeof(int);

			TranslationFCurve = std::make_unique<FCurveVector3D>();
			pos += TranslationFCurve->Load(pos, version);
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
		InstanceTranslationState& instanceState,
		SIMD::Vec3f& position,
		SIMD::Vec3f& steeringVec,
		RandObject& rand,
		const Effect* effect,
		const InstanceGlobal* instanceGlobal,
		float livingTime,
		float livedTime,
		const Instance* parent,
		const DynamicFactorParameter& dynamicFactor)
	{
		steeringVec = SIMD::Vec3f(0, 0, 0);

		// Translation
		if (TranslationType == ParameterTranslationType_Fixed)
		{
			instanceState.fixed.location = TranslationFixed.Position;
			ApplyDynamicParameterToFixedLocation(instanceState, rand, effect, instanceGlobal, parent, dynamicFactor);

			position = instanceState.fixed.location;
		}
		else if (TranslationType == ParameterTranslationType_PVA)
		{
			auto rvl = ApplyEq(effect,
							   instanceGlobal,
							   parent,
							   &rand,
							   TranslationPVA.RefEqP,
							   TranslationPVA.location,
							   dynamicFactor.Tra,
							   dynamicFactor.TraInv);
			instanceState.random.location = rvl.getValue(rand);

			auto rvv = ApplyEq(effect,
							   instanceGlobal,
							   parent,
							   &rand,
							   TranslationPVA.RefEqV,
							   TranslationPVA.velocity,
							   dynamicFactor.Tra,
							   dynamicFactor.TraInv);
			instanceState.random.velocity = rvv.getValue(rand);

			auto rva = ApplyEq(effect,
							   instanceGlobal,
							   parent,
							   &rand,
							   TranslationPVA.RefEqA,
							   TranslationPVA.acceleration,
							   dynamicFactor.Tra,
							   dynamicFactor.TraInv);
			instanceState.random.acceleration = rva.getValue(rand);

			position = instanceState.random.location;

			steeringVec = instanceState.random.velocity;
		}
		else if (TranslationType == ParameterTranslationType_Easing)
		{
			TranslationEasing.Init(instanceState.easing, effect, instanceGlobal, parent, &rand, dynamicFactor.Tra, dynamicFactor.TraInv);

			position = instanceState.easing.start;
		}
		else if (TranslationType == ParameterTranslationType_FCurve)
		{
			assert(TranslationFCurve != nullptr);
			const auto coordinateSystem = effect->GetSetting()->GetCoordinateSystem();

			instanceState.fcruve.offset = TranslationFCurve->GetOffsets(rand);

			position = instanceState.fcruve.offset + TranslationFCurve->GetValues(livingTime, livedTime);

			if (coordinateSystem == CoordinateSystem::LH)
			{
				position.SetZ(-position.GetZ());
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

				float t = speed * livingTime * moveSpeed;

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

				position = curve->CalcuratePoint(t, NurbsCurveParam.Scale * effect->GetMaginification());
			}
			else
			{
				position = {0, 0, 0};
			}
		}
		else if (TranslationType == ParameterTranslationType_ViewOffset)
		{
			instanceState.view_offset.distance = TranslationViewOffset.distance.getValue(rand);
			position = {0, 0, 0};
		}

		instanceState.prevLocation = position;
	}

	SIMD::Vec3f CalculateTranslationState(
		InstanceTranslationState& instanceState,
		RandObject& rand,
		const Effect* effect,
		const InstanceGlobal* instanceGlobal,
		float livingTime,
		float livedTime,
		const Instance* m_pParent,
		CoordinateSystem coordinateSystem,
		const DynamicFactorParameter& dynamicFactor)
	{
		SIMD::Vec3f localPosition;

		if (TranslationType == ParameterTranslationType_None)
		{
			localPosition = {0, 0, 0};
		}
		else if (TranslationType == ParameterTranslationType_Fixed)
		{
			ApplyDynamicParameterToFixedLocation(instanceState, rand, effect, instanceGlobal, m_pParent, dynamicFactor);

			localPosition = instanceState.fixed.location;
		}
		else if (TranslationType == ParameterTranslationType_PVA)
		{
			localPosition = instanceState.random.location + (instanceState.random.velocity * livingTime) +
							(instanceState.random.acceleration * (livingTime * livingTime * 0.5f));
		}
		else if (TranslationType == ParameterTranslationType_Easing)
		{
			localPosition = TranslationEasing.GetValue(instanceState.easing, Clamp(livingTime / livedTime, 1.0F, 0.0F));
		}
		else if (TranslationType == ParameterTranslationType_FCurve)
		{
			assert(TranslationFCurve != nullptr);
			auto fcurve = TranslationFCurve->GetValues(livingTime, livedTime);
			localPosition = fcurve + instanceState.fcruve.offset;

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

				float t = speed * livingTime * moveSpeed;

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
			return {0, 0, 0};
		}

		const auto vel = localPosition - instanceState.prevLocation;
		instanceState.prevLocation = localPosition;
		return vel;
	}
};

} // namespace Effekseer