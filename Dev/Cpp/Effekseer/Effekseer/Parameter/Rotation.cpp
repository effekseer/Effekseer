#include "Rotation.h"
#include "../Effekseer.Instance.h"
#include "../Effekseer.Setting.h"
#include "../Utils/Compatiblity.h"

namespace Effekseer
{

void RotationParameter::Load(unsigned char*& pos, int version)
{
	int32_t size = 0;

	memcpy(&RotationType, pos, sizeof(int));
	pos += sizeof(int);
	EffekseerPrintDebug("RotationType %d\n", RotationType);
	if (RotationType == ParameterRotationType::ParameterRotationType_Fixed)
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

		RotationFCurve = std::make_unique<FCurveVector3D>();
		pos += RotationFCurve->Load(pos, version);
	}
	else if (RotationType == ParameterRotationType::ParameterRotationType_RotateToViewpoint)
	{
		memcpy(&size, pos, sizeof(int));
		pos += sizeof(int);
	}
}

void RotationParameter::MakeCoordinateSystemLH()
{
	if (RotationType == ParameterRotationType::ParameterRotationType_Fixed)
	{
		RotationFixed.Position.X *= -1.0f;
		RotationFixed.Position.Y *= -1.0f;
	}
	else if (RotationType == ParameterRotationType::ParameterRotationType_PVA)
	{
		RotationPVA.rotation.max.x *= -1.0f;
		RotationPVA.rotation.min.x *= -1.0f;
		RotationPVA.rotation.max.y *= -1.0f;
		RotationPVA.rotation.min.y *= -1.0f;
		RotationPVA.velocity.max.x *= -1.0f;
		RotationPVA.velocity.min.x *= -1.0f;
		RotationPVA.velocity.max.y *= -1.0f;
		RotationPVA.velocity.min.y *= -1.0f;
		RotationPVA.acceleration.max.x *= -1.0f;
		RotationPVA.acceleration.min.x *= -1.0f;
		RotationPVA.acceleration.max.y *= -1.0f;
		RotationPVA.acceleration.min.y *= -1.0f;
	}
	else if (RotationType == ParameterRotationType::ParameterRotationType_Easing)
	{
		RotationEasing.start.max.x *= -1.0f;
		RotationEasing.start.min.x *= -1.0f;
		RotationEasing.start.max.y *= -1.0f;
		RotationEasing.start.min.y *= -1.0f;
		RotationEasing.end.max.x *= -1.0f;
		RotationEasing.end.min.x *= -1.0f;
		RotationEasing.end.max.y *= -1.0f;
		RotationEasing.end.min.y *= -1.0f;
	}
	else if (RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
	{
		RotationAxisPVA.axis.max.z *= -1.0f;
		RotationAxisPVA.axis.min.z *= -1.0f;
	}
	else if (RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
	{
		RotationAxisEasing.axis.max.z *= -1.0f;
		RotationAxisEasing.axis.min.z *= -1.0f;
	}
	else if (RotationType == ParameterRotationType::ParameterRotationType_FCurve)
	{
		RotationFCurve->X.ChangeCoordinate();
		RotationFCurve->Y.ChangeCoordinate();
	}
}

void RotationFunctions::ApplyDynamicParameterToFixedRotation(RotationState& rotation_values,
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

void RotationFunctions::InitRotation(RotationState& rotation_values, const RotationParameter& rotationParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor)
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
		rotation_values.axis.axis = rotation_values.axis.axis.Normalize();
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
		rotation_values.axis.axis = rotation_values.axis.axis.Normalize();
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
	{
		assert(rotationParam.RotationFCurve != nullptr);

		rotation_values.fcruve.offset = rotationParam.RotationFCurve->GetOffsets(rand);
	}
}

SIMD::Mat43f RotationFunctions::CalculateRotation(RotationState& rotation_values, const RotationParameter& rotationParam, RandObject& rand, const Effect* effect, const InstanceGlobal* instanceGlobal, float m_LivingTime, float m_LivedTime, const Instance* m_pParent, const DynamicFactorParameter& dynamicFactor, const Vector3D& viewpoint)
{
	SIMD::Vec3f localAngle;

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
		localAngle = rotation_values.random.rotation + (rotation_values.random.velocity * m_LivingTime) +
					 (rotation_values.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f));
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing)
	{
		localAngle = rotationParam.RotationEasing.GetValue(rotation_values.easing, Clamp(m_LivingTime / m_LivedTime, 1.0F, 0.0));
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA)
	{
		rotation_values.axis.rotation = rotation_values.axis.random.rotation + rotation_values.axis.random.velocity * m_LivingTime +
										rotation_values.axis.random.acceleration * (m_LivingTime * m_LivingTime * 0.5f);
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
	{
		rotation_values.axis.rotation = rotationParam.RotationAxisEasing.easing.GetValue(rotation_values.axis.easing, Clamp(m_LivingTime / m_LivedTime, 1.0F, 0.0F));
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
	{
		assert(rotationParam.RotationFCurve != nullptr);
		auto fcurve = rotationParam.RotationFCurve->GetValues(m_LivingTime, m_LivedTime);
		localAngle = fcurve + rotation_values.fcruve.offset;
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_RotateToViewpoint)
	{
		const auto globalMat = m_pParent->GetGlobalMatrix().GetCurrent();
		SIMD::Vec3f s;
		SIMD::Mat43f r;
		SIMD::Vec3f t;

		globalMat.GetSRT(s, r, t);

		SIMD::Mat44f r4(r);
		// Because of r4 contains only rotation matrix
		const auto r4inv = r4.Transpose();

		Matrix44 lookAt;
		if (effect->GetSetting()->GetCoordinateSystem() == CoordinateSystem::RH)
		{
			lookAt.LookAtRH(viewpoint, SIMD::ToStruct(t), {0, 1, 0});
		}
		else
		{
			lookAt.LookAtLH(SIMD::ToStruct(t), viewpoint, {0, 1, 0});
		}

		lookAt.Values[3][0] = 0.0f;
		lookAt.Values[3][1] = 0.0f;
		lookAt.Values[3][2] = 0.0f;

		const auto rot = r4inv * SIMD::Mat44f(lookAt).Transpose();
		SIMD::Mat43f ret;
		ret.X = rot.X;
		ret.Y = rot.Y;
		ret.Z = rot.Z;
		return ret;
	}

	SIMD::Mat43f matRot;
	if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Fixed || rotationParam.RotationType == ParameterRotationType::ParameterRotationType_PVA ||
		rotationParam.RotationType == ParameterRotationType::ParameterRotationType_Easing || rotationParam.RotationType == ParameterRotationType::ParameterRotationType_FCurve)
	{
		matRot = SIMD::Mat43f::RotationZXY(localAngle.GetZ(), localAngle.GetX(), localAngle.GetY());
	}
	else if (rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisPVA ||
			 rotationParam.RotationType == ParameterRotationType::ParameterRotationType_AxisEasing)
	{
		SIMD::Vec3f axis = rotation_values.axis.axis;

		matRot = SIMD::Mat43f::RotationAxis(axis, rotation_values.axis.rotation);
	}
	else
	{
		matRot = SIMD::Mat43f::Identity;
	}

	return matRot;
}

} // namespace Effekseer