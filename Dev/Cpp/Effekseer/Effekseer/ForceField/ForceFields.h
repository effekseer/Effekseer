#pragma once

#include <array>
#include <memory>
#include <random>

#include "../Effekseer.Vector3D.h"
#include "../Noise/CurlNoise.h"
#include "../Noise/PerlinNoise.h"
#include "../SIMD/Effekseer.Mat44f.h"
#include "../SIMD/Effekseer.Vec3f.h"
#include "../Utils/BinaryVersion.h"

namespace Effekseer
{

struct ForceFieldCommonParameter
{
	Vec3f Position;
	Vec3f PreviousVelocity;
	Vec3f PreviousSumVelocity;
	Vec3f FieldCenter;
	Vec3f TargetPosition;
	Mat44f FieldRotation;
	bool IsFieldRotated = false;
	float DeltaFrame;
};

struct ForceFieldFalloffCommonParameter
{
	float Power = 0.0f;
	float MinDistance = 0.0f;
	float MaxDistance = 0.0f;
};

struct ForceFieldFalloffSphereParameter
{
};

struct ForceFieldFalloffTubeParameter
{
	float RadiusPower = 0.0f;
	float MinRadius = 0.0f;
	float MaxRadius = 0.0f;
};

struct ForceFieldFalloffConeParameter
{
	float AnglePower = 0.0f;
	float MinAngle = 0.0f;
	float MaxAngle = 0.0f;
};

struct ForceFieldForceParameter
{
	float Power;
	bool Gravitation;
};

struct ForceFieldWindParameter
{
	float Power;
};

enum class ForceFieldVortexType : int32_t
{
	ConstantAngle = 0,
	ConstantSpeed = 1,
};

struct ForceFieldVortexParameter
{
	ForceFieldVortexType Type = ForceFieldVortexType::ConstantAngle;
	float Power;
};

enum class ForceFieldTurbulenceType : int32_t
{
	Simple = 0,
	Complicated = 1,
};

struct ForceFieldTurbulenceParameter
{
	float Power;

	std::unique_ptr<CurlNoise> Noise;
	std::unique_ptr<LightCurlNoise> LightNoise;

	ForceFieldTurbulenceParameter(ForceFieldTurbulenceType type, int32_t seed, float scale, float strength, int octave);
};

struct ForceFieldDragParameter
{
	float Power;
};

struct ForceFieldGravityParameter
{
	Vec3f Gravity;
};

struct ForceFieldAttractiveForceParameter
{
	float Force;
	float Control;
	float MinRange;
	float MaxRange;
};

class ForceFieldFalloff
{
public:
	//! Sphare
	float GetPower(float power,
				   const ForceFieldCommonParameter& ffc,
				   const ForceFieldFalloffCommonParameter& fffc,
				   const ForceFieldFalloffSphereParameter& fffs)
	{
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength();
		if (distance > fffc.MaxDistance)
		{
			return 0.0f;
		}

		if (distance < fffc.MinDistance)
		{
			return 0.0f;
		}

		return power / powf(distance - fffc.MinDistance, fffc.Power);
	}

	//! Tube
	float GetPower(float power,
				   const ForceFieldCommonParameter& ffc,
				   const ForceFieldFalloffCommonParameter& fffc,
				   const ForceFieldFalloffTubeParameter& ffft)
	{
		// Sphere
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength();
		if (distance > fffc.MaxDistance)
		{
			return 0.0f;
		}

		if (distance < fffc.MinDistance)
		{
			return 0.0f;
		}

		// Tube
		auto tubePos = localPos;
		tubePos.SetY(0);

		auto tubeRadius = tubePos.GetLength();

		if (tubeRadius > ffft.MaxRadius)
		{
			return 0.0f;
		}

		if (tubeRadius < ffft.MinRadius)
		{
			return 0.0f;
		}

		return power / powf(distance, fffc.Power) / powf(tubeRadius - ffft.MinRadius, ffft.RadiusPower);
	}

	float GetPower(float power,
				   const ForceFieldCommonParameter& ffc,
				   const ForceFieldFalloffCommonParameter& fffc,
				   const ForceFieldFalloffConeParameter& ffft)
	{
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength();
		if (distance > fffc.MaxDistance)
		{
			return 0.0f;
		}

		if (distance < fffc.MinDistance)
		{
			return 0.0f;
		}

		auto tubePos = localPos;
		tubePos.SetY(0);

		auto tubeRadius = tubePos.GetLength();

		auto angle = fabs(EFK_PI / 2.0f - atan2(localPos.GetY(), tubeRadius));

		if (angle > ffft.MaxAngle)
		{
			return 0.0f;
		}

		if (angle < ffft.MinAngle)
		{
			return 0.0f;
		}

		const auto e = 0.000001f;
		return power / powf(distance, fffc.Power) / powf((angle - ffft.MinAngle) / (ffft.MaxAngle - ffft.MinAngle + e), ffft.AnglePower);
	}
};

class ForceField
{
public:
	/**
		@brief	Force
	*/
	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldForceParameter& ffp)
	{
		float eps = 0.0000001f;
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength() + eps;
		auto dir = localPos / distance;

		if (ffp.Gravitation)
		{
			return dir * ffp.Power / distance;
		}

		return dir * ffp.Power;
	}

	/**
		@brief	Wind
	*/
	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldWindParameter& ffp)
	{
		auto dir = Vec3f(0, 1, 0);
		return dir * ffp.Power;
	}

	/**
		@brief	Vortex
	*/
	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldVortexParameter& ffp)
	{
		float eps = 0.0000001f;
		auto localPos = ffc.Position - ffc.FieldCenter;
		localPos.SetY(0.0f);
		auto distance = localPos.GetLength();

		if (distance < eps)
			return Vec3f(0.0f, 0.0f, 0.0f);
		if (abs(ffp.Power) < eps)
			return Vec3f(0.0f, 0.0f, 0.0f);

		localPos /= distance;

		auto axis = Vec3f(0, 1, 0);
		Vec3f front = Vec3f::Cross(axis, localPos);

		auto direction = 1.0f;
		if (ffp.Power < 0)
			direction = -1.0f;

		auto power = ffp.Power;

		if (ffp.Type == ForceFieldVortexType::ConstantAngle)
		{
			power *= distance;
		}

		auto xlen = power / distance * (power / 2.0f);
		auto flen = sqrt(power * power - xlen * xlen);
		return (front * flen - localPos * xlen) * direction - ffc.PreviousVelocity;
	}

	/**
		@brief	Turbulence
	*/
	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldTurbulenceParameter& ffp)
	{
		const float LightNoisePowerScale = 4.0f;

		auto localPos = ffc.Position - ffc.FieldCenter;
		Vec3f vel;

		if (ffp.Noise != nullptr)
		{
			vel = ffp.Noise->Get(localPos) * ffp.Power;
		}
		else if (ffp.LightNoise != nullptr)
		{
			vel = ffp.LightNoise->Get(localPos) * ffp.Power * LightNoisePowerScale;		
		}

		auto acc = vel - ffc.PreviousVelocity;
		return acc;
	}

	/**
		@brief	Drag
	*/
	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldDragParameter& ffp)
	{
		return -ffc.PreviousSumVelocity * ffp.Power;
	}

	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldGravityParameter& ffp)
	{
		return ffp.Gravity;
	}

	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldAttractiveForceParameter& ffp)
	{
		const Vec3f targetDifference = ffc.TargetPosition - ffc.Position;
		const float targetDistance = targetDifference.GetLength();

		if (targetDistance > 0.0f)
		{
			const Vec3f targetDirection = targetDifference / targetDistance;
			float force = ffp.Force;

			if (ffp.MinRange > 0.0f || ffp.MaxRange > 0.0f)
			{
				if (targetDistance >= ffp.MaxRange)
				{
					force = 0.0f;
				}
				else if (targetDistance > ffp.MinRange)
				{
					force *= 1.0f - (targetDistance - ffp.MinRange) / (ffp.MaxRange - ffp.MinRange);
				}
			}

			if (ffc.DeltaFrame > 0)
			{
				float eps = 0.0001f;
				auto ret = Vec3f(0.0f, 0.0f, 0.0f);
				auto vel = ffc.PreviousVelocity;
				vel += targetDirection * force * ffc.DeltaFrame;
				float currentVelocity = vel.GetLength() + eps;
				Vec3f currentDirection = vel / currentVelocity;

				vel = (targetDirection * ffp.Control + currentDirection * (1.0f - ffp.Control)) * currentVelocity;
				return vel - ffc.PreviousVelocity;
			}
		}

		return Vec3f(0.0f, 0.0f, 0.0f);
	}
};

enum class LocalForceFieldFalloffType : int32_t
{
	None = 0,
	Sphere = 1,
	Tube = 2,
	Cone = 3,
};

enum class LocalForceFieldType : int32_t
{
	None = 0,
	Force = 2,
	Wind = 3,
	Vortex = 4,
	Turbulence = 1,
	Drag = 7,
	Gravity = 8,
	AttractiveForce = 9,
};

struct LocalForceFieldElementParameter
{
	Vector3D Position;
	Mat44f Rotation;
	Mat44f InvRotation;
	bool IsRotated = false;
	bool IsGlobal = false;

	std::unique_ptr<ForceFieldForceParameter> Force;
	std::unique_ptr<ForceFieldWindParameter> Wind;
	std::unique_ptr<ForceFieldVortexParameter> Vortex;
	std::unique_ptr<ForceFieldTurbulenceParameter> Turbulence;
	std::unique_ptr<ForceFieldDragParameter> Drag;
	std::unique_ptr<ForceFieldGravityParameter> Gravity;
	std::unique_ptr<ForceFieldAttractiveForceParameter> AttractiveForce;

	std::unique_ptr<ForceFieldFalloffCommonParameter> FalloffCommon;
	std::unique_ptr<ForceFieldFalloffSphereParameter> FalloffSphere;
	std::unique_ptr<ForceFieldFalloffTubeParameter> FalloffTube;
	std::unique_ptr<ForceFieldFalloffConeParameter> FalloffCone;

	bool HasValue = false;

	bool Load(uint8_t*& pos, int32_t version);
};

struct LocalForceFieldParameter
{
	std::array<LocalForceFieldElementParameter, LocalFieldSlotMax> LocalForceFields;

	bool HasValue = false;

	bool IsGlobalEnabled = false;

	bool Load(uint8_t*& pos, int32_t version);

	void MaintainGravityCompatibility(const Vec3f& gravity);

	void MaintainAttractiveForceCompatibility(const float force, const float control, const float minRange, const float maxRange);
};

struct LocalForceFieldInstance
{
	std::array<Vec3f, LocalFieldSlotMax> Velocities;

	Vec3f ExternalVelocity;
	Vec3f VelocitySum;
	Vec3f ModifyLocation;

	Vec3f GlobalVelocitySum;
	Vec3f GlobalModifyLocation;

	void Update(const LocalForceFieldParameter& parameter, const Vec3f& location, float magnification);

	void UpdateGlobal(const LocalForceFieldParameter& parameter, const Vec3f& location, float magnification, const Vec3f& targetPosition, float deltaTime);

	void Reset();
};

} // namespace Effekseer
