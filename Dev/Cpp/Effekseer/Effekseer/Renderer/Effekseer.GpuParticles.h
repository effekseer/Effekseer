
#ifndef __EFFEKSEER_GPU_PARTICLES_H__
#define __EFFEKSEER_GPU_PARTICLES_H__

#include "../Effekseer.Base.h"
#include "../Effekseer.Vector2D.h"
#include "../Effekseer.Vector3D.h"
#include "../Effekseer.Color.h"

namespace Effekseer
{

namespace GpuParticles
{
template <class T>
struct vector2 {
	T x, y;

	vector2() = default;
	vector2(T x, T y): x(x), y(y) {}
	vector2(const std::array<T, 2>& rhs): x(rhs[0]), y(rhs[1]) {}
	vector2(const Effekseer::Vector2D& rhs): x((T)rhs.X), y((T)rhs.Y) {}
	vector2& operator=(const std::array<T, 2>& rhs) { return *this = vector2(rhs); }
	vector2& operator=(const Effekseer::Vector2D& rhs) { return *this = vector2(rhs); }
};
template <class T>
struct vector3
{
	float x, y, z;

	vector3() = default;
	vector3(T x, T y, T z): x(x), y(y), z(z) {}
	vector3(const std::array<T, 3>& rhs): x(rhs[0]), y(rhs[1]), z(rhs[2]) {}
	vector3(const Effekseer::Vector3D& rhs): x((T)rhs.X), y((T)rhs.Y), z((T)rhs.Z) {}
	vector3& operator=(const std::array<T, 3>& rhs) { return *this = vector3(rhs); }
	vector3& operator=(const Effekseer::Vector3D& rhs) { return *this = vector3(rhs); }
};
template <class T>
struct vector4
{
	T x, y, z, w;

	vector4() = default;
	vector4(T x, T y, T z, T w): x(x), y(y), z(z), w(w) {}
	vector4(vector3<T> v3, T w): x(v3.x), y(v3.y), z(v3.z), w(w) {}
	vector4(const std::array<T, 4>& rhs): x(rhs[0]), y(rhs[1]), z(rhs[2]), w(rhs[3]) {}
	vector4& operator=(const std::array<T, 4>& rhs) { return *this = vector4(rhs); }
};

using float2 = GpuParticles::vector2<float>;
using float3 = GpuParticles::vector3<float>;
using float4 = GpuParticles::vector4<float>;
using float4x3 = std::array<float4, 3>;
using float4x4 = Effekseer::Matrix44;

using int2 = GpuParticles::vector2<int32_t>;
using int3 = GpuParticles::vector3<int32_t>;
using int4 = GpuParticles::vector4<int32_t>;

using uint = uint32_t;
using uint2 = GpuParticles::vector2<uint32_t>;
using uint3 = GpuParticles::vector3<uint32_t>;
using uint4 = GpuParticles::vector4<uint32_t>;


template <class T>
inline vector2<T> operator+(vector2<T> a, vector2<T> b)
{
	return vector2<T>{ a.x + b.x, a.y + b.y };
}

template <class T>
inline vector2<T> operator*(vector2<T> a, T b)
{
	return vector2<T>{ a.x * b, a.y * b };
}

template <class T>
inline vector3<T> operator+(vector3<T> a, vector3<T> b)
{
	return vector3<T>{ a.x + b.x, a.y + b.y, a.z + b.z };
}

template <class T>
inline vector3<T> operator*(vector3<T> a, T b)
{
	return vector3<T>{ a.x * b, a.y * b, a.z * b };
}

template <class T>
inline vector4<T> operator+(vector4<T> a, vector4<T> b)
{
	return vector4<T>{ a.x + b.x, a.y + b.y, a.z + b.z , a.w + b.w };
}

template <class T>
inline vector4<T> operator*(vector4<T> a, T b)
{
	return vector4<T>{ a.x * b, a.y * b, a.z * b, a.w * b };
}


enum class EmitShapeT : uint32_t
{
	Point, Line, Circle, Sphere, Model,
};
enum class ScaleType : uint32_t
{
	Fixed, PVA, Easing,
};
enum class ColorParamType : uint32_t
{
	Fixed, Random, Easing, FCurve, Gradient,
};
enum class ColorSpaceType : uint32_t
{
	RGBA, HSVA,
};
enum class RenderShapeT : uint32_t
{
	Sprite, Model, Trail,
};
enum class MaterialType : uint32_t
{
	Unlit, Lighting,
};


struct ParamSet
{
	struct BasicParams {
		int32_t EmitCount;
		int32_t EmitPerFrame;
		float EmitOffset;
		std::array<float, 2> LifeTime;
	};
	BasicParams Basic;

	struct EmitShapeParams
	{
		struct LineParams
		{
			float3 Start;
			float Padding;
			float3 End;
			float Width;
		};
		struct CircleParams
		{
			float3 Axis;
			float Padding;
			float Inner;
			float Outer;
		};
		struct SphereParams
		{
			float Radius;
		};
		struct ModelParams
		{
			int32_t Index;
			float Size;
		};

		EmitShapeT Type;
		bool RotationApplied;
		union
		{
			std::array<float4, 2> Data;
			LineParams Line;
			CircleParams Circle;
			SphereParams Sphere;
			ModelParams Model;
		};
	};
	EmitShapeParams EmitShape;

	struct VelocityParams
	{
		float3 Direction;
		float Spread;
		std::array<float, 2> InitialSpeed;
		std::array<float, 2> Damping;
	};
	VelocityParams Velocity;

	struct RotationParams
	{
		std::array<float3, 2> Offset;
		std::array<float3, 2> Velocity;
	};
	RotationParams Rotation;

	struct ScaleParams
	{
		struct FixedParams
		{
			std::array<float4, 2> Scale;
		};

		struct EasingParams
		{
			std::array<float4, 2> Start;
			std::array<float4, 2> End;
			float3 Speed;
		};

		ScaleType Type;
		union
		{
			std::array<float4, 4> Data;
			FixedParams Fixed;
			EasingParams Easing;
		};
	};
	ScaleParams Scale;

	struct ForceParams
	{
		float3 Gravity;

		float3 VortexCenter;
		float VortexRotation;
		float3 VortexAxis;
		float VortexAttraction;

		float TurbulencePower;
		int32_t TurbulenceSeed;
		float TurbulenceScale;
		int32_t TurbulenceOctave;
	};
	ForceParams Force;

	struct RenderBasicParams
	{
		AlphaBlendType BlendType;
		uint8_t ZWrite;
		uint8_t ZTest;
	};
	RenderBasicParams RenderBasic;

	struct RenderShapeParams
	{
		RenderShapeT Type;
		uint32_t Data;
		float Size;
	};
	RenderShapeParams RenderShape;

	struct RenderColorParams
	{
		struct FixedParams
		{
			Effekseer::Color Color;
		};
		struct RandomParams
		{
			std::array<Effekseer::Color, 2> Color;
		};
		struct EasingParams
		{
			std::array<Effekseer::Color, 2> Start;
			std::array<Effekseer::Color, 2> End;
			float3 Speed;
		};
		struct GradientParams
		{
			std::array<Effekseer::Color, 32> Pixels;
		};

		BindType ColorInherit;
		ColorSpaceType ColorSpace;
		ColorParamType ColorAllType;
		union ColorParams
		{
			uint4 Data;
			FixedParams Fixed;
			RandomParams Random;
			EasingParams Easing;
			GradientParams Gradient;
		};
		ColorParams ColorAll;

		float Emissive;
		float FadeIn;
		float FadeOut;
	};
	RenderColorParams RenderColor;

	struct RenderMaterialParams
	{
		MaterialType Material;
		std::array<uint32_t, 4> TextureIndexes;
		std::array<uint8_t, 4> TextureFilters;
		std::array<uint8_t, 4> TextureWraps;
	};
	RenderMaterialParams RenderMaterial;
};

class Resource : public ReferenceObject
{
public:
	Resource() = default;
	virtual ~Resource() = default;

	virtual const ParamSet& GetParamSet() const = 0;
};
using ResourceRef = Effekseer::RefPtr<Resource>;

}

class GpuParticleFactory : public ReferenceObject
{
public:
	virtual GpuParticles::ResourceRef CreateResource(const GpuParticles::ParamSet& paramSet, const Effect* effect) { return nullptr; }
};
using GpuParticleFactoryRef = RefPtr<GpuParticleFactory>;


class GpuParticleSystem : public ReferenceObject
{
public:
	struct Settings
	{
		uint32_t EmitterMaxCount = 256;
		uint32_t ParticleMaxCount = 1 * 1024 * 1024;
		uint32_t TrailMaxCount = 4 * 1024 * 1024;
	};
	struct Context
	{
		bool CoordinateReversed;
	};

	using EmitterID = int32_t;
	static constexpr int32_t InvalidID = -1;

public:
	GpuParticleSystem() = default;

	virtual ~GpuParticleSystem() = default;
	
	virtual bool InitSystem(const Settings& settings) { return true; }

	virtual void ComputeFrame(const Context& context) {}

	virtual void RenderFrame(const Context& context) {}

	virtual EmitterID NewEmitter(GpuParticles::ResourceRef paramRes, Effekseer::InstanceGlobal* instanceGlobal) { return InvalidID; }

	virtual void StartEmit(EmitterID emitterID) {}

	virtual void StopEmit(EmitterID emitterID) {}

	virtual void SetRandomSeed(EmitterID emitterID, uint32_t seed) {}

	virtual void SetTransform(EmitterID emitterID, const Effekseer::Matrix43& transform) {}

	virtual void SetColor(EmitterID emitterID, Effekseer::Color color) {}

	virtual void SetDeltaTime(Effekseer::InstanceGlobal* instanceGlobal, float deltaTime) {}

	virtual void KillParticles(Effekseer::InstanceGlobal* instanceGlobal) {}

	virtual int32_t GetParticleCount(Effekseer::InstanceGlobal* instanceGlobal) { return 0; }

protected:
	Settings m_settings;
};
using GpuParticleSystemRef = RefPtr<GpuParticleSystem>;

} // namespace Effekseer

#endif // __EFFEKSEER_GPU_PARTICLES_H__
