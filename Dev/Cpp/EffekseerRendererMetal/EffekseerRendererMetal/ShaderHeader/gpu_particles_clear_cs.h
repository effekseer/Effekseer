static const char metal_gpu_particles_clear_cs[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct EmitterData
{
    uint FlagBits;
    uint Seed;
    uint ParticleHead;
    uint ParticleSize;
    uint TrailHead;
    uint TrailSize;
    uint TrailPhase;
    uint NextEmitCount;
    uint TotalEmitCount;
    uint EmitPointCount;
    float TimeCount;
    float TimeStopped;
    uint Reserved0;
    uint Reserved1;
    float DeltaTime;
    uint Color;
    float3x4 Transform;
};

struct cb1
{
    EmitterData emitter;
};

struct ParticleData
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint2 DirectionSpeed;
    uint Color;
    uint Padding;
    float4x3 Transform;
};

struct ParticleData_1
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint2 DirectionSpeed;
    uint Color;
    uint Padding;
    float3x4 Transform;
};

struct Particles
{
    ParticleData_1 _data[1];
};

struct ParameterData
{
    int EmitCount;
    int EmitPerFrame;
    float EmitOffset;
    uint Padding0;
    float2 LifeTime;
    uint EmitShapeType;
    uint EmitRotationApplied;
    float4 EmitShapeData[2];
    float3 Direction;
    float Spread;
    float2 InitialSpeed;
    float2 Damping;
    float4 AngularOffset[2];
    float4 AngularVelocity[2];
    float4 ScaleData1[2];
    float4 ScaleData2[2];
    float3 ScaleEasing;
    uint ScaleFlags;
    float3 Gravity;
    uint Padding2;
    float3 VortexCenter;
    float VortexRotation;
    float3 VortexAxis;
    float VortexAttraction;
    float TurbulencePower;
    uint TurbulenceSeed;
    float TurbulenceScale;
    float TurbulenceOctave;
    uint RenderState;
    uint ShapeType;
    uint ShapeData;
    float ShapeSize;
    float Emissive;
    float FadeIn;
    float FadeOut;
    uint MaterialType;
    uint4 ColorData;
    float3 ColorEasing;
    uint ColorFlags;
};

struct cb0
{
    ParameterData paramData;
};

static inline __attribute__((always_inline))
uint2 PackFloat4(thread const float4& v)
{
    uint4 v16 = uint4(as_type<uint>(half2(float2(v.x, 0.0))), as_type<uint>(half2(float2(v.y, 0.0))), as_type<uint>(half2(float2(v.z, 0.0))), as_type<uint>(half2(float2(v.w, 0.0))));
    return uint2(v16.x | (v16.y << uint(16)), v16.z | (v16.w << uint(16)));
}

static inline __attribute__((always_inline))
void _main(thread const uint3& dtid, constant cb1& _72, device Particles& Particles_1)
{
    uint particleID = _72.emitter.ParticleHead + dtid.x;
    ParticleData particle;
    particle.FlagBits = 0u;
    particle.Seed = 0u;
    particle.LifeAge = 0.0;
    particle.InheritColor = 0u;
    particle.Color = 0u;
    particle.Padding = 0u;
    float4 param = float4(0.0);
    particle.DirectionSpeed = PackFloat4(param);
    particle.Transform = float4x3(float3(0.0), float3(0.0), float3(0.0), float3(0.0));
    Particles_1._data[particleID].FlagBits = particle.FlagBits;
    Particles_1._data[particleID].Seed = particle.Seed;
    Particles_1._data[particleID].LifeAge = particle.LifeAge;
    Particles_1._data[particleID].InheritColor = particle.InheritColor;
    Particles_1._data[particleID].DirectionSpeed = particle.DirectionSpeed;
    Particles_1._data[particleID].Color = particle.Color;
    Particles_1._data[particleID].Padding = particle.Padding;
    Particles_1._data[particleID].Transform = transpose(particle.Transform);
}

kernel void main0(constant cb1& _72 [[buffer(1)]], device Particles& Particles_1 [[buffer(10)]], uint3 gl_GlobalInvocationID [[thread_position_in_grid]])
{
    uint3 dtid = gl_GlobalInvocationID;
    uint3 param = dtid;
    _main(param, _72, Particles_1);
}

)";

