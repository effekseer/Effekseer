#include "ForceFields.h"
#include "../Effekseer.Matrix44.h"
#include "../SIMD/Effekseer.SIMDUtils.h"
#include "../Utils/Effekseer.BinaryReader.h"

namespace Effekseer
{

ForceFieldTurbulenceParameter::ForceFieldTurbulenceParameter(int32_t seed, float scale, float strength, int octave)
	: Noise(seed)
{
	Noise.Octave = octave;
	Noise.Scale = scale;
	Power = strength;
}

LocalForceFieldTurbulenceParameterOld::LocalForceFieldTurbulenceParameterOld(int32_t seed, float scale, float strength, int octave)
	: Noise(seed)
{
	Noise.Octave = octave;
	Noise.Scale = scale;
	Strength = strength;
}

bool LocalForceFieldParameterOld::Load(uint8_t*& pos, int32_t version)
{
	auto br = BinaryReader<false>(pos, std::numeric_limits<int>::max());

	LocalForceFieldType type{};
	br.Read(type);

	if (type == LocalForceFieldType::Turbulence)
	{
		int32_t seed{};
		float scale{};
		float strength{};
		int octave{};

		br.Read(seed);
		br.Read(scale);
		br.Read(strength);
		br.Read(octave);

		scale = 1.0f / scale;

		Turbulence = std::unique_ptr<LocalForceFieldTurbulenceParameterOld>(
			new LocalForceFieldTurbulenceParameterOld(seed, scale, strength, octave));
	}

	pos += br.GetOffset();

	return true;
}

bool LocalForceFieldElementParameter::Load(uint8_t*& pos, int32_t version)
{
	auto br = BinaryReader<false>(pos, std::numeric_limits<int>::max());

	LocalForceFieldType type{};
	br.Read(type);

	HasValue = true;
	float power = 1.0f;

	if (version >= 1600)
	{
		br.Read(power);

		br.Read(Position.X);
		br.Read(Position.Y);
		br.Read(Position.Z);

		Vector3D rotation;
		br.Read(rotation.X);
		br.Read(rotation.Y);
		br.Read(rotation.Z);

		IsRotated = rotation.X != 0.0f || rotation.Y != 0.0f || rotation.Z != 0.0f;

		if (IsRotated)
		{
			Rotation = Mat44f::RotationZXY(rotation.Z, rotation.X, rotation.Y);
			Matrix44 invMat;
			InvRotation = Mat44f(Matrix44::Inverse(invMat, ToStruct(Rotation)));
		}
	}

	if (type == LocalForceFieldType::Force)
	{
		int gravitation = 0;
		br.Read(gravitation);

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
		// convert it by frames
		power /= 60.0f;

		auto ff = new ForceFieldVortexParameter();
		ff->Power = power;
		Vortex = std::unique_ptr<ForceFieldVortexParameter>(ff);
	}
	else if (type == LocalForceFieldType::Maginetic)
	{
		// convert it by frames
		power /= 60.0f;

		auto ff = new ForceFieldMagineticParameter();
		ff->Power = power;
		Maginetic = std::unique_ptr<ForceFieldMagineticParameter>(ff);
	}
	else if (type == LocalForceFieldType::Turbulence)
	{
		int32_t seed{};
		float scale{};
		float strength{};
		int octave{};

		br.Read(seed);
		br.Read(scale);
		br.Read(strength);
		br.Read(octave);

		scale = 1.0f / scale;

		Turbulence = std::unique_ptr<ForceFieldTurbulenceParameter>(new ForceFieldTurbulenceParameter(seed, scale, strength, octave));
	}
	else if (type == LocalForceFieldType::Drag)
	{
		// convert it by frames
		power /= 60.0f;

		auto ff = new ForceFieldDragParameter();
		ff->Power = power;
		Drag = std::unique_ptr<ForceFieldDragParameter>(ff);
	}
	else
	{
		HasValue = false;
	}

	if (version >= 1600)
	{
		LocalForceFieldFalloffType ffType{};
		br.Read(ffType);

		if (ffType != LocalForceFieldFalloffType::None)
		{
			FalloffCommon = std::make_unique<ForceFieldFalloffCommonParameter>();
			br.Read(FalloffCommon->Power);
			br.Read(FalloffCommon->MaxDistance);
			br.Read(FalloffCommon->MinDistance);
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
			br.Read(FalloffTube->RadiusPower);
			br.Read(FalloffTube->MaxRadius);
			br.Read(FalloffTube->MinRadius);
		}
		else if (ffType == LocalForceFieldFalloffType::Cone)
		{
			FalloffCone = std::make_unique<ForceFieldFalloffConeParameter>();
			br.Read(FalloffCone->AnglePower);
			br.Read(FalloffCone->MaxAngle);
			br.Read(FalloffCone->MinAngle);
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

	pos += br.GetOffset();

	return true;
}

bool LocalForceFieldParameter::Load(uint8_t*& pos, int32_t version)
{
	int32_t count = 0;
	memcpy(&count, pos, sizeof(int));
	pos += sizeof(int);

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
		}
	}

	return true;
}

void LocalForceFieldInstance::Update(const LocalForceFieldParameter& parameter, const Vec3f& location, float magnification)
{
	for (size_t i = 0; i < parameter.LocalForceFields.size(); i++)
	{
		auto& field = parameter.LocalForceFields[i];
		if (!field.HasValue)
		{
			continue;
		}

		ForceFieldCommonParameter ffcp;
		ffcp.FieldCenter = parameter.LocalForceFields[i].Position;
		ffcp.Position = location / magnification;
		ffcp.PreviousSumVelocity = VelocitySum;
		ffcp.PreviousVelocity = Velocities[i];
		ffcp.IsFieldRotated = field.IsRotated;

		if (field.IsRotated)
		{
			ffcp.PreviousSumVelocity = Vec3f::Transform(VelocitySum, field.InvRotation);
			ffcp.PreviousVelocity = Vec3f::Transform(Velocities[i], field.InvRotation);
			ffcp.Position = Vec3f::Transform(ffcp.Position, field.InvRotation);
		}

		ForceField ff;

		Vec3f acc = Vec3f(0, 0, 0);
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

		if (field.Maginetic != nullptr)
		{
			acc = ff.GetAcceleration(ffcp, *field.Maginetic) * magnification;
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
			acc = Vec3f::Transform(acc, field.Rotation);
		}

		Velocities[i] += acc;
	}

	VelocitySum = Vec3f(0, 0, 0);

	for (size_t i = 0; i < parameter.LocalForceFields.size(); i++)
	{
		VelocitySum += Velocities[i];
	}

	ModifyLocation += VelocitySum;
}

#ifdef __EFFEKSEER_BUILD_VERSION16__
void LocalForceFieldInstance::DraggedVelocity(Vec3f& outAcceleration, const LocalForceFieldParameter& parameter, float deltaFrame)
{
	for (auto& field : parameter.LocalForceFields)
	{
		if (!field.HasValue)
		{
			continue;
		}

		if (field.Drag != nullptr)
		{
			ForceField ff;
			outAcceleration += ff.GetDragedVelocity(outAcceleration, *field.Drag) * deltaFrame;
		}
	}
}
#endif

void LocalForceFieldInstance::Reset()
{
	Velocities.fill(Vec3f(0, 0, 0));
	VelocitySum = Vec3f(0, 0, 0);
	ModifyLocation = Vec3f(0, 0, 0);
}

} // namespace Effekseer