#include "GpuParticlesParameter.h"

namespace Effekseer
{

template <class T>
inline T Read(uint8_t*& pos) {
	T data;
	memcpy(&data, pos, sizeof(T));
	pos += sizeof(T);
	return data;
}

static inline void SwapCoordinateSystemTranslation(GpuParticles::float3& t)
{
	t.z = -t.z;
}

static inline void SwapCoordinateSystemRotation(GpuParticles::float3& r)
{
	r.x = -r.x;
	r.y = -r.y;
}

static inline void SwapCoordinateSystemRotation(float& r)
{
	r = -r;
}

GpuParticles::ParamSet LoadGpuParticlesParameter(uint8_t*& pos, int32_t version, float magnification, CoordinateSystem coordinateSystem)
{
	using namespace Effekseer::GpuParticles;

	GpuParticles::ParamSet paramSet{};

	paramSet.Basic.EmitCount = Read<int32_t>(pos);
	paramSet.Basic.EmitPerFrame = Read<int32_t>(pos);
	paramSet.Basic.EmitOffset = Read<float>(pos);
	paramSet.Basic.LifeTime = Read<std::array<float, 2>>(pos);

	paramSet.EmitShape.Type = (EmitShapeT)Read<uint8_t>(pos);
	paramSet.EmitShape.RotationApplied = Read<uint8_t>(pos) != 0;
	switch (paramSet.EmitShape.Type)
	{
	case EmitShapeT::Point:
		break;
	case EmitShapeT::Line:
		paramSet.EmitShape.Line.Start = Read<float3>(pos) * magnification;
		paramSet.EmitShape.Line.End = Read<float3>(pos) * magnification;
		paramSet.EmitShape.Line.Width = Read<float>(pos) * magnification;
		break;
	case EmitShapeT::Circle:
		paramSet.EmitShape.Circle.Axis = Read<float3>(pos);
		paramSet.EmitShape.Circle.Inner = Read<float>(pos) * magnification;
		paramSet.EmitShape.Circle.Outer = Read<float>(pos) * magnification;
		break;
	case EmitShapeT::Sphere:
		paramSet.EmitShape.Sphere.Radius = Read<float>(pos) * magnification;
		break;
	case EmitShapeT::Model:
		paramSet.EmitShape.Model.Index = Read<int32_t>(pos);
		paramSet.EmitShape.Model.Size = Read<float>(pos) * magnification;
		break;
	}

	paramSet.Velocity.Direction = Read<float3>(pos);
	paramSet.Velocity.Spread = Read<float>(pos);
	paramSet.Velocity.InitialSpeed = Read<std::array<float, 2>>(pos);
	paramSet.Velocity.Damping = Read<std::array<float, 2>>(pos);

	paramSet.Velocity.InitialSpeed[0] *= magnification;
	paramSet.Velocity.InitialSpeed[1] *= magnification;

	paramSet.Rotation.Offset = Read<std::array<float3, 2>>(pos);
	paramSet.Rotation.Velocity = Read<std::array<float3, 2>>(pos);

	for (size_t i = 0; i < 2; i++)
	{
		// Degrees to Radians
		paramSet.Rotation.Offset[i].x *= EFK_PI / 180.0f;
		paramSet.Rotation.Offset[i].y *= EFK_PI / 180.0f;
		paramSet.Rotation.Offset[i].z *= EFK_PI / 180.0f;
		paramSet.Rotation.Velocity[i].x *= EFK_PI / 180.0f;
		paramSet.Rotation.Velocity[i].y *= EFK_PI / 180.0f;
		paramSet.Rotation.Velocity[i].z *= EFK_PI / 180.0f;
	}

	paramSet.Scale.Type = (ScaleType)Read<uint8_t>(pos);

	auto ReadScale4 = [](uint8_t*& pos)
	{
		float s0 = Read<float>(pos);
		float s1 = Read<float>(pos);
		float3 xyz0 = Read<float3>(pos);
		float3 xyz1 = Read<float3>(pos);
		return std::array<float4, 2>{ float4(xyz0.x, xyz0.y, xyz0.z, s0), float4(xyz1.x, xyz1.y, xyz1.z, s1) };
	};
	switch (paramSet.Scale.Type)
	{
	case ScaleType::Fixed:
		paramSet.Scale.Fixed.Scale = ReadScale4(pos);
		break;
	case ScaleType::Easing:
		paramSet.Scale.Easing.Start = ReadScale4(pos);
		paramSet.Scale.Easing.End = ReadScale4(pos);
		paramSet.Scale.Easing.Speed = Read<float3>(pos);
		break;
	default:
		assert(false);
		break;
	}

	paramSet.Force.Gravity = Read<float3>(pos) * magnification;
	paramSet.Force.VortexRotation = Read<float>(pos) * magnification;
	paramSet.Force.VortexAttraction = Read<float>(pos) * magnification;
	paramSet.Force.VortexCenter = Read<float3>(pos) * magnification;
	paramSet.Force.VortexAxis = Read<float3>(pos);
	paramSet.Force.TurbulencePower = Read<float>(pos) * magnification;
	paramSet.Force.TurbulenceSeed = Read<int32_t>(pos);
	paramSet.Force.TurbulenceScale = Read<float>(pos) * 0.125f / magnification;
	paramSet.Force.TurbulenceOctave = Read<int32_t>(pos);

	paramSet.RenderBasic.BlendType = (AlphaBlendType)Read<uint8_t>(pos);
	paramSet.RenderBasic.ZWrite = Read<uint8_t>(pos);
	paramSet.RenderBasic.ZTest = Read<uint8_t>(pos);

	paramSet.RenderShape.Type = (RenderShapeT)Read<uint8_t>(pos);
	paramSet.RenderShape.Data = Read<uint32_t>(pos);
	paramSet.RenderShape.Size = Read<float>(pos) * magnification;

	paramSet.RenderColor.ColorInherit = (BindType)Read<uint8_t>(pos);
	paramSet.RenderColor.ColorAllType = (ColorParamType)Read<uint8_t>(pos);
	paramSet.RenderColor.ColorSpace = (GpuParticles::ColorSpaceType)Read<uint8_t>(pos);

	switch (paramSet.RenderColor.ColorAllType)
	{
	case ColorParamType::Fixed:
		paramSet.RenderColor.ColorAll.Fixed.Color = Read<Color>(pos);
		break;
	case ColorParamType::Random:
		paramSet.RenderColor.ColorAll.Random.Color = Read<std::array<Color, 2>>(pos);
		break;
	case ColorParamType::Easing:
		paramSet.RenderColor.ColorAll.Easing.Start = Read<std::array<Color, 2>>(pos);
		paramSet.RenderColor.ColorAll.Easing.End = Read<std::array<Color, 2>>(pos);
		paramSet.RenderColor.ColorAll.Easing.Speed = Read<float3>(pos);
		break;
	case ColorParamType::FCurve:
	case ColorParamType::Gradient:
		paramSet.RenderColor.ColorAll.Gradient.Pixels = Read<std::array<Color, 32>>(pos);
		break;
	default:
		assert(false);
	}

	paramSet.RenderColor.Emissive = Read<float>(pos);
	paramSet.RenderColor.FadeIn = Read<float>(pos);
	paramSet.RenderColor.FadeOut = Read<float>(pos);

	paramSet.RenderMaterial.Material = (MaterialType)Read<uint8_t>(pos);
	paramSet.RenderMaterial.TextureIndexes = Read<std::array<uint32_t, 4>>(pos);
	paramSet.RenderMaterial.TextureFilters = Read<std::array<uint8_t, 4>>(pos);
	paramSet.RenderMaterial.TextureWraps = Read<std::array<uint8_t, 4>>(pos);

	if (coordinateSystem == CoordinateSystem::LH)
	{
		SwapCoordinateSystemTranslation(paramSet.EmitShape.Line.Start);
		SwapCoordinateSystemTranslation(paramSet.EmitShape.Line.End);

		SwapCoordinateSystemRotation(paramSet.Rotation.Offset[0]);
		SwapCoordinateSystemRotation(paramSet.Rotation.Offset[1]);
		SwapCoordinateSystemRotation(paramSet.Rotation.Velocity[0]);
		SwapCoordinateSystemRotation(paramSet.Rotation.Velocity[1]);

		SwapCoordinateSystemTranslation(paramSet.Force.Gravity);
		SwapCoordinateSystemTranslation(paramSet.Force.VortexAxis);
		SwapCoordinateSystemTranslation(paramSet.Force.VortexCenter);
		SwapCoordinateSystemRotation(paramSet.Force.VortexRotation);
	}

	return paramSet;
}

} // namespace Effekseer
