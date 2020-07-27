#pragma once

#include <array>
#include <memory>
#include <random>

#include "../Effekseer.Vector3D.h"
#include "../Noise/CurlNoise.h"
#include "../Noise/PerlinNoise.h"
#include "../SIMD/Effekseer.Mat44f.h"
#include "../SIMD/Effekseer.Vec3f.h"

namespace Effekseer
{

/**
	TODO
	Implement rotation
	Check falloff specification
*/

struct ForceFieldCommonParameter
{
	Vec3f Position;
	Vec3f PreviousVelocity;
	Vec3f PreviousSumVelocity;
	Vec3f FieldCenter;
	Mat44f FieldRotation;
	bool IsFieldRotated = false;
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
	// Shape
	float Power;
	bool Gravitation;
};

struct ForceFieldWindParameter
{
	// Shape
	float Power;
};

struct ForceFieldVortexParameter
{
	// Shape
	float Power;
};

struct ForceFieldMagineticParameter
{
	// Shape
	float Power;
};

struct ForceFieldTurbulenceParameter
{
	float Power;
	CurlNoise Noise;

	ForceFieldTurbulenceParameter(int32_t seed, float scale, float strength, int octave);
};

struct ForceFieldDragParameter
{
	float Power;
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

		auto xlen = ffp.Power / distance * (ffp.Power / 2.0f);
		auto flen = sqrt(ffp.Power * ffp.Power - xlen * xlen);
		return (front * flen - localPos * xlen) * direction - ffc.PreviousVelocity;
	}

	/**
		@brief	Maginetic
	*/
	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldMagineticParameter& ffp)
	{
		float eps = 0.0000001f;
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto distance = localPos.GetLength() + eps;
		auto dir = localPos / distance;

		auto vecLenSq = ffc.PreviousSumVelocity.GetSquaredLength();
		if (vecLenSq < eps)
			return Vec3f(0.0f, 0.0f, 0.0f);

		auto forceDir = Vec3f::Cross(ffc.PreviousSumVelocity / sqrtf(vecLenSq), dir);

		auto forceDirScaleSq = forceDir.GetSquaredLength();
		if (forceDirScaleSq < 0.01f)
			return Vec3f(0.0f, 0.0f, 0.0f);

		forceDir /= (sqrtf(forceDirScaleSq));

		return forceDir * ffp.Power;
	}

	/**
		@brief	Turbulence
	*/
	Vec3f GetAcceleration(const ForceFieldCommonParameter& ffc, const ForceFieldTurbulenceParameter& ffp)
	{
		auto localPos = ffc.Position - ffc.FieldCenter;
		auto vel = ffp.Noise.Get(localPos) * ffp.Power;
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
	Maginetic = 5,
	Turbulence = 1,
	Drag = 7,
};

struct LocalForceFieldTurbulenceParameterOld
{
	float Strength = 0.1f;
	CurlNoise Noise;

	LocalForceFieldTurbulenceParameterOld(int32_t seed, float scale, float strength, int octave);
};

//! TODO Replace
struct LocalForceFieldParameterOld
{
	std::unique_ptr<LocalForceFieldTurbulenceParameterOld> Turbulence;

	bool Load(uint8_t*& pos, int32_t version);
};

struct LocalForceFieldElementParameter
{
	Vector3D Position;
	Mat44f Rotation;
	Mat44f InvRotation;
	bool IsRotated;

	std::unique_ptr<ForceFieldForceParameter> Force;
	std::unique_ptr<ForceFieldWindParameter> Wind;
	std::unique_ptr<ForceFieldVortexParameter> Vortex;
	std::unique_ptr<ForceFieldMagineticParameter> Maginetic;
	std::unique_ptr<ForceFieldTurbulenceParameter> Turbulence;
	std::unique_ptr<ForceFieldDragParameter> Drag;

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

	bool Load(uint8_t*& pos, int32_t version);
};

struct LocalForceFieldInstance
{
	std::array<Vec3f, LocalFieldSlotMax> Velocities;

	Vec3f ExternalVelocity;
	Vec3f VelocitySum;
	Vec3f ModifyLocation;

	void Update(const LocalForceFieldParameter& parameter, const Vec3f& location, float magnification);

	void Reset();
};

} // namespace Effekseer
