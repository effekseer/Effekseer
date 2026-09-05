#include "Effekseer.ForceFields.h"
#include "../Effekseer.InternalStruct.h"
#include "../Effekseer.Matrix44.h"
#include "../SIMD/Utils.h"
#include "../Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

enum class LocationAbsType : int32_t
{
	None = 0,
	Gravity = 1,
	AttractiveForce = 2,
};

struct LocationAbsParameter
{
	LocationAbsType type = LocationAbsType::None;

	union
	{
		struct
		{

		} none;

		SIMD::Vec3f gravity;

		struct
		{
			float force;
			float control;
			float minRange;
			float maxRange;
		} attractiveForce;
	};
};

ForceFieldTurbulenceParameter::ForceFieldTurbulenceParameter(ForceFieldTurbulenceType type, int32_t seed, float scale, float strength, int octave)
{
	if (type == ForceFieldTurbulenceType::Simple)
	{
		LightNoise = std::make_unique<LightCurlNoise>(seed, scale, octave);
	}
	else if (type == ForceFieldTurbulenceType::Complicated)
	{
		Noise = std::make_unique<CurlNoise>(seed, scale, octave);
	}
	Power = strength;
}

bool LocalForceFieldElementParameter::Load(BinaryReader<true>& pos, int32_t version)
{
	auto& br = pos;

	LocalForceFieldType type{};
	if (!br.Read(type))
	{
		return false;
	}

	HasValue = true;
	float power = 1.0f;

	if (version >= 1600)
	{
		if (!br.Read(power))
		{
			return false;
		}

		if (!br.Read(Position.X))
		{
			return false;
		}
		if (!br.Read(Position.Y))
		{
			return false;
		}
		if (!br.Read(Position.Z))
		{
			return false;
		}

		Vector3D rotation;
		if (!br.Read(rotation.X))
		{
			return false;
		}
		if (!br.Read(rotation.Y))
		{
			return false;
		}
		if (!br.Read(rotation.Z))
		{
			return false;
		}

		IsRotated = rotation.X != 0.0f || rotation.Y != 0.0f || rotation.Z != 0.0f;

		if (IsRotated)
		{
			Rotation = SIMD::Mat44f::RotationZXY(rotation.Z, rotation.X, rotation.Y);
			Matrix44 invMat;
			InvRotation = SIMD::Mat44f(Matrix44::Inverse(invMat, ToStruct(Rotation)));
		}
	}

	if (type == LocalForceFieldType::Force)
	{
		int gravitation = 0;
		if (!br.Read(gravitation))
		{
			return false;
		}

		// convert it by frames
		power /= 60.0f;

		auto ff = new ForceFieldForceParameter();
		ff->Power = power;
		ff->Gravitation = gravitation > 0;
		Force = std::unique_ptr<ForceFieldForceParameter>(ff);
	}
	else if (type == LocalForceFieldType::Wind)
	{
		// convert it by frames
		power /= 60.0f;

		auto ff = new ForceFieldWindParameter();
		ff->Power = power;
		Wind = std::unique_ptr<ForceFieldWindParameter>(ff);
	}
	else if (type == LocalForceFieldType::Vortex)
	{
		if (version < Version16Alpha2)
		{
			power /= 5.0f;
		}

		// convert it by frames
		power /= 12.0f;

		ForceFieldVortexType ftype{};

		if (version < Version16Alpha2)
		{
			ftype = ForceFieldVortexType::ConstantSpeed;
		}
		else
		{
			if (!br.Read(ftype))
			{
				return false;
			}
		}

		auto ff = new ForceFieldVortexParameter();
		ff->Power = power;
		ff->Type = ftype;
		Vortex = std::unique_ptr<ForceFieldVortexParameter>(ff);
	}
	else if (type == LocalForceFieldType::Turbulence)
	{
		ForceFieldTurbulenceType ftype{};
		int32_t seed{};
		float scale{};
		float strength{};
		int octave{};

		if (version < Version16Alpha2)
		{
			ftype = ForceFieldTurbulenceType::Complicated;
		}
		else
		{
			if (!br.Read(ftype))
			{
				return false;
			}
		}

		if (!br.Read(seed))
		{
			return false;
		}
		if (!br.Read(scale))
		{
			return false;
		}

		if (version < Version16Alpha2)
		{
			if (!br.Read(strength))
			{
				return false;
			}
			strength *= 10.0f;
		}
		else
		{
			strength = power;
		}

		if (!br.Read(octave))
		{
			return false;
		}

		scale = 1.0f / scale;

		strength /= 10.0f;

		Turbulence = std::unique_ptr<ForceFieldTurbulenceParameter>(new ForceFieldTurbulenceParameter(ftype, seed, scale, strength, octave));
	}
	else if (type == LocalForceFieldType::Drag)
	{
		// convert it by frames
		power /= 60.0f;

		auto ff = new ForceFieldDragParameter();
		ff->Power = power;
		Drag = std::unique_ptr<ForceFieldDragParameter>(ff);
	}
	else if (type == LocalForceFieldType::Gravity)
	{
		std::array<float, 3> values;
		if (!br.Read(values))
		{
			return false;
		}
		SIMD::Vec3f gravity{values};
		Gravity = std::make_unique<ForceFieldGravityParameter>();
		Gravity->Gravity = gravity;
		IsGlobal = true;
	}
	else if (type == LocalForceFieldType::AttractiveForce)
	{
		AttractiveForce = std::make_unique<ForceFieldAttractiveForceParameter>();
		AttractiveForce->Force = power;
		if (!br.Read(AttractiveForce->Control))
		{
			return false;
		}
		if (!br.Read(AttractiveForce->MinRange))
		{
			return false;
		}
		if (!br.Read(AttractiveForce->MaxRange))
		{
			return false;
		}
		IsGlobal = true;
	}
	else
	{
		HasValue = false;
	}

	if (version >= 1600)
	{
		LocalForceFieldFalloffType ffType{};
		if (!br.Read(ffType))
		{
			return false;
		}

		if (ffType != LocalForceFieldFalloffType::None)
		{
			FalloffCommon = std::make_unique<ForceFieldFalloffCommonParameter>();
			if (!br.Read(FalloffCommon->Power))
			{
				return false;
			}
			if (!br.Read(FalloffCommon->MaxDistance))
			{
				return false;
			}
			if (!br.Read(FalloffCommon->MinDistance))
			{
				return false;
			}
		}

		if (ffType == LocalForceFieldFalloffType::None)
		{
		}
		else if (ffType == LocalForceFieldFalloffType::Sphere)
		{
			FalloffSphere = std::make_unique<ForceFieldFalloffSphereParameter>();
		}
		else if (ffType == LocalForceFieldFalloffType::Tube)
		{
			FalloffTube = std::make_unique<ForceFieldFalloffTubeParameter>();
			if (!br.Read(FalloffTube->RadiusPower))
			{
				return false;
			}
			if (!br.Read(FalloffTube->MaxRadius))
			{
				return false;
			}
			if (!br.Read(FalloffTube->MinRadius))
			{
				return false;
			}
		}
		else if (ffType == LocalForceFieldFalloffType::Cone)
		{
			FalloffCone = std::make_unique<ForceFieldFalloffConeParameter>();
			if (!br.Read(FalloffCone->AnglePower))
			{
				return false;
			}
			if (!br.Read(FalloffCone->MaxAngle))
			{
				return false;
			}
			if (!br.Read(FalloffCone->MinAngle))
			{
				return false;
			}
		}
		else
		{
			assert(0);
		}
	}
	else
	{
		IsRotated = false;
	}

	return true;
}

bool LocalForceFieldParameter::Load(BinaryReader<true>& pos, int32_t version)
{
	if (version >= 1500)
	{
		int32_t count = 0;
		if (!pos.Peek(&count, sizeof(int)))
		{
			return false;
		}
		pos.Skip(sizeof(int));
		if (count < 0 || count > static_cast<int32_t>(LocalForceFields.size()))
		{
			return false;
		}

		for (int32_t i = 0; i < count; i++)
		{
			if (!LocalForceFields[i].Load(pos, version))
			{
				return false;
			}
		}

		for (auto& ff : LocalForceFields)
		{
			if (ff.HasValue)
			{
				HasValue = true;

				if (ff.Gravity != nullptr || ff.AttractiveForce != nullptr)
				{
					IsGlobalEnabled = true;
				}
			}
		}
	}

	// for compatiblity of location abs
	if (version <= Version16Alpha1)
	{
		LocationAbsParameter LocationAbs;
		int32_t size = 0;

		if (!pos.Peek(&LocationAbs.type, sizeof(int)))
		{
			return false;
		}
		pos.Skip(sizeof(int));

		// Calc attraction forces
		if (LocationAbs.type == LocationAbsType::None)
		{
			if (!pos.Peek(&size, sizeof(int)))
			{
				return false;
			}
			pos.Skip(sizeof(int));
			if (size != 0)
				return false;
			if (!pos.Peek(&LocationAbs.none, size))
			{
				return false;
			}
			pos.Skip(size);
		}
		else if (LocationAbs.type == LocationAbsType::Gravity)
		{
			if (!pos.Peek(&size, sizeof(int)))
			{
				return false;
			}
			pos.Skip(sizeof(int));
			if (size != sizeof(vector3d))
				return false;
			if (!pos.Peek(&LocationAbs.gravity, size))
			{
				return false;
			}
			pos.Skip(size);
		}
		else if (LocationAbs.type == LocationAbsType::AttractiveForce)
		{
			if (!pos.Peek(&size, sizeof(int)))
			{
				return false;
			}
			pos.Skip(sizeof(int));
			if (size != sizeof(LocationAbs.attractiveForce))
				return false;
			if (!pos.Peek(&LocationAbs.attractiveForce, size))
			{
				return false;
			}
			pos.Skip(size);
		}

		if (LocationAbs.type == LocationAbsType::Gravity)
		{
			MaintainGravityCompatibility(LocationAbs.gravity);
		}
		else if (LocationAbs.type == LocationAbsType::AttractiveForce)
		{
			MaintainAttractiveForceCompatibility(
				LocationAbs.attractiveForce.force,
				LocationAbs.attractiveForce.control,
				LocationAbs.attractiveForce.minRange,
				LocationAbs.attractiveForce.maxRange);
		}
	}

	return true;
}

void LocalForceFieldParameter::MaintainGravityCompatibility(const SIMD::Vec3f& gravity)
{
	HasValue = true;
	IsGlobalEnabled = true;
	LocalForceFields[3].Gravity = std::make_unique<ForceFieldGravityParameter>();
	LocalForceFields[3].HasValue = true;
	LocalForceFields[3].Gravity->Gravity = gravity;
	LocalForceFields[3].IsGlobal = true;
}

void LocalForceFieldParameter::MaintainAttractiveForceCompatibility(const float force, const float control, const float minRange, const float maxRange)
{
	HasValue = true;
	IsGlobalEnabled = true;
	LocalForceFields[3].AttractiveForce = std::make_unique<ForceFieldAttractiveForceParameter>();
	LocalForceFields[3].HasValue = true;
	LocalForceFields[3].AttractiveForce->Force = force;
	LocalForceFields[3].AttractiveForce->Control = control;
	LocalForceFields[3].AttractiveForce->MinRange = minRange;
	LocalForceFields[3].AttractiveForce->MaxRange = maxRange;
	LocalForceFields[3].IsGlobal = true;
}

SIMD::Vec3f LocalForceFieldInstance::Update(const LocalForceFieldParameter& parameter, const SIMD::Vec3f& location, const SIMD::Vec3f& local_velocity_per_update, float magnification, float deltaFrame, CoordinateSystem coordinateSystem)
{
	if (deltaFrame == 0.0f)
	{
		return SIMD::Vec3f{0, 0, 0};
	}

	std::array<SIMD::Vec3f, LocalFieldSlotMax> accs;
	accs.fill(SIMD::Vec3f(0, 0, 0));

	for (size_t i = 0; i < parameter.LocalForceFields.size(); i++)
	{
		auto& field = parameter.LocalForceFields[i];
		if (!field.HasValue)
		{
			continue;
		}

		if (parameter.LocalForceFields[i].IsGlobal)
			continue;

		ForceFieldCommonParameter ffcp;
		ffcp.FieldCenter = parameter.LocalForceFields[i].Position;
		ffcp.Position = location / magnification;
		ffcp.PreviousSumVelocity = local_velocity_per_update / magnification;
		ffcp.PreviousVelocity = Velocities[i] / magnification;
		ffcp.DeltaFrame = deltaFrame;
		ffcp.IsFieldRotated = field.IsRotated;

		if (coordinateSystem == CoordinateSystem::LH)
		{
			ffcp.Position.SetZ(-ffcp.Position.GetZ());
			ffcp.PreviousVelocity.SetZ(-ffcp.PreviousVelocity.GetZ());
			ffcp.PreviousSumVelocity.SetZ(-ffcp.PreviousSumVelocity.GetZ());
		}

		if (field.IsRotated)
		{
			ffcp.PreviousSumVelocity = SIMD::Vec3f::Transform(ffcp.PreviousSumVelocity, field.InvRotation);
			ffcp.PreviousVelocity = SIMD::Vec3f::Transform(ffcp.PreviousVelocity, field.InvRotation);
			ffcp.Position = SIMD::Vec3f::Transform(ffcp.Position, field.InvRotation);
		}

		ForceField ff;

		SIMD::Vec3f acc = SIMD::Vec3f(0, 0, 0);
		if (field.Force != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.Force) * magnification;
		}

		if (field.Wind != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.Wind) * magnification;
		}

		if (field.Vortex != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.Vortex) * magnification;
		}

		if (field.Turbulence != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.Turbulence) * magnification;
		}

		if (field.Drag != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.Drag) * magnification;
		}

		float power = 1.0f;
		if (field.FalloffCommon != nullptr && field.FalloffCone != nullptr)
		{
			ForceFieldFalloff fff;
			power = fff.GetPower(power, ffcp, *field.FalloffCommon, *field.FalloffCone);
		}

		if (field.FalloffCommon != nullptr && field.FalloffSphere != nullptr)
		{
			ForceFieldFalloff fff;
			power = fff.GetPower(power, ffcp, *field.FalloffCommon, *field.FalloffSphere);
		}

		if (field.FalloffCommon != nullptr && field.FalloffTube != nullptr)
		{
			ForceFieldFalloff fff;
			power = fff.GetPower(power, ffcp, *field.FalloffCommon, *field.FalloffTube);
		}

		acc *= power;

		if (field.IsRotated)
		{
			acc = SIMD::Vec3f::Transform(acc, field.Rotation);
		}

		if (coordinateSystem == CoordinateSystem::LH)
		{
			acc.SetZ(-acc.GetZ());
		}

		Velocities[i] += acc;
		accs[i] = acc;
	}

	SIMD::Vec3f acc_sum = SIMD::Vec3f(0, 0, 0);

	for (size_t i = 0; i < parameter.LocalForceFields.size(); i++)
	{
		if (parameter.LocalForceFields[i].IsGlobal)
			continue;
		acc_sum += accs[i];
	}

	return acc_sum;
}

SIMD::Vec3f LocalForceFieldInstance::UpdateGlobal(const LocalForceFieldParameter& parameter, const SIMD::Vec3f& location, float magnification, const SIMD::Vec3f& targetPosition, float deltaFrame, CoordinateSystem coordinateSystem)
{
	if (deltaFrame == 0.0f)
	{
		return SIMD::Vec3f(0, 0, 0);
	}

	std::array<SIMD::Vec3f, LocalFieldSlotMax> accs;
	accs.fill(SIMD::Vec3f(0, 0, 0));

	SIMD::Vec3f velocity_sum_prev = SIMD::Vec3f(0, 0, 0);
	for (size_t i = 0; i < parameter.LocalForceFields.size(); i++)
	{
		if (!parameter.LocalForceFields[i].IsGlobal)
			continue;
		velocity_sum_prev += Velocities[i];
	}

	for (size_t i = 0; i < parameter.LocalForceFields.size(); i++)
	{
		auto& field = parameter.LocalForceFields[i];
		if (!field.HasValue)
		{
			continue;
		}

		if (!parameter.LocalForceFields[i].IsGlobal)
			continue;

		ForceFieldCommonParameter ffcp;
		ffcp.FieldCenter = parameter.LocalForceFields[i].Position;
		ffcp.Position = location / magnification;
		ffcp.PreviousSumVelocity = velocity_sum_prev / magnification;
		ffcp.PreviousVelocity = Velocities[i] / magnification;
		ffcp.TargetPosition = targetPosition / magnification;
		ffcp.DeltaFrame = deltaFrame;
		ffcp.IsFieldRotated = field.IsRotated;

		if (coordinateSystem == CoordinateSystem::LH)
		{
			ffcp.Position.SetZ(-ffcp.Position.GetZ());
			ffcp.PreviousVelocity.SetZ(-ffcp.PreviousVelocity.GetZ());
			ffcp.PreviousSumVelocity.SetZ(-ffcp.PreviousSumVelocity.GetZ());
			ffcp.TargetPosition.SetZ(-ffcp.TargetPosition.GetZ());
		}

		if (field.IsRotated)
		{
			ffcp.PreviousSumVelocity = SIMD::Vec3f::Transform(ffcp.PreviousSumVelocity, field.InvRotation);
			ffcp.PreviousVelocity = SIMD::Vec3f::Transform(ffcp.PreviousVelocity, field.InvRotation);
			ffcp.Position = SIMD::Vec3f::Transform(ffcp.Position, field.InvRotation);
			ffcp.TargetPosition = SIMD::Vec3f::Transform(ffcp.TargetPosition, field.InvRotation);
		}

		ForceField ff;

		SIMD::Vec3f acc = SIMD::Vec3f(0, 0, 0);
		if (field.Gravity != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.Gravity) * magnification;
		}

		if (field.AttractiveForce != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.AttractiveForce) * magnification;
		}

		float power = 1.0f;
		if (field.FalloffCommon != nullptr && field.FalloffCone != nullptr)
		{
			ForceFieldFalloff fff;
			power = fff.GetPower(power, ffcp, *field.FalloffCommon, *field.FalloffCone);
		}

		if (field.FalloffCommon != nullptr && field.FalloffSphere != nullptr)
		{
			ForceFieldFalloff fff;
			power = fff.GetPower(power, ffcp, *field.FalloffCommon, *field.FalloffSphere);
		}

		if (field.FalloffCommon != nullptr && field.FalloffTube != nullptr)
		{
			ForceFieldFalloff fff;
			power = fff.GetPower(power, ffcp, *field.FalloffCommon, *field.FalloffTube);
		}

		acc *= power;

		if (field.IsRotated)
		{
			acc = SIMD::Vec3f::Transform(acc, field.Rotation);
		}

		if (coordinateSystem == CoordinateSystem::LH)
		{
			acc.SetZ(-acc.GetZ());
		}

		Velocities[i] += acc;
		accs[i] = acc;
	}

	SIMD::Vec3f acc_sum = SIMD::Vec3f(0, 0, 0);
	for (size_t i = 0; i < parameter.LocalForceFields.size(); i++)
	{
		if (!parameter.LocalForceFields[i].IsGlobal)
			continue;
		acc_sum += accs[i];
	}

	return acc_sum;
}

void LocalForceFieldInstance::Reset()
{
	Velocities.fill(SIMD::Vec3f(0, 0, 0));
}

} // namespace Effekseer
