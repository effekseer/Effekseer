static const char metal_gpu_particles_spawn_cs[] = R"(mtlcode
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

struct ParameterData
{
    int EmitCount;
    int EmitPerFrame;
    float EmitOffset;
    uint Padding0;
    float2 LifeTime;
    uint EmitShapeType;
    uint Padding1;
    float4 EmitShapeData[2];
    packed_float3 Direction;
    float Spread;
    float2 InitialSpeed;
    float2 Damping;
    float4 AngularOffset[2];
    float4 AngularVelocity[2];
    float4 ScaleData1[2];
    float4 ScaleData2[2];
    packed_float3 ScaleEasing;
    uint ScaleFlags;
    packed_float3 Gravity;
    uint Padding2;
    packed_float3 VortexCenter;
    float VortexRotation;
    packed_float3 VortexAxis;
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
    packed_float3 ColorEasing;
    uint ColorFlags;
};

struct cb0
{
    ParameterData paramData;
};

struct EmitPoint
{
    float3 Position;
    uint Normal;
    uint Binormal;
    uint Tangent;
    uint UV;
    uint VColor;
};

struct EmitPoint_1
{
    packed_float3 Position;
    uint Normal;
    uint Binormal;
    uint Tangent;
    uint UV;
    uint VColor;
};

struct EmitPoints
{
    EmitPoint_1 _data[1];
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

static inline __attribute__((always_inline))
uint RandomUint(thread uint& seed)
{
    uint state = seed;
    seed = (seed * 747796405u) + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

static inline __attribute__((always_inline))
float RandomFloat(thread uint& seed)
{
    uint param = seed;
    uint _302 = RandomUint(param);
    seed = param;
    return float(_302) / 4294967296.0;
}

static inline __attribute__((always_inline))
float3 RandomSpread(thread uint& seed, thread float3& baseDir, thread const float& angle)
{
    uint param = seed;
    float _415 = RandomFloat(param);
    seed = param;
    float theta = 6.28318023681640625 * _415;
    uint param_1 = seed;
    float _422 = RandomFloat(param_1);
    seed = param_1;
    float phi = angle * _422;
    float3 randDir = float3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    baseDir = fast::normalize(baseDir);
    if (abs(baseDir.z) != 1.0)
    {
        float3 front = float3(0.0, 0.0, 1.0);
        float3 right = fast::normalize(cross(front, baseDir));
        float3 up = cross(baseDir, right);
        return float3x3(float3(right), float3(up), float3(baseDir)) * randDir;
    }
    else
    {
        return randDir * sign(baseDir.z);
    }
}

static inline __attribute__((always_inline))
float RandomFloatRange(thread uint& seed, thread const float2& maxmin)
{
    uint param = seed;
    float _315 = RandomFloat(param);
    seed = param;
    return mix(maxmin.y, maxmin.x, _315);
}

static inline __attribute__((always_inline))
float3 RandomDirection(thread uint& seed)
{
    uint param = seed;
    float _324 = RandomFloat(param);
    seed = param;
    float theta = 6.28318023681640625 * _324;
    uint param_1 = seed;
    float _330 = RandomFloat(param_1);
    seed = param_1;
    float phi = 6.28318023681640625 * _330;
    float3 randDir = float3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    return randDir;
}

static inline __attribute__((always_inline))
float3 RandomCircle(thread uint& seed, thread float3& axis)
{
    uint param = seed;
    float _353 = RandomFloat(param);
    seed = param;
    float theta = 6.28318023681640625 * _353;
    float3 randDir = float3(cos(theta), 0.0, sin(theta));
    axis = fast::normalize(axis);
    if (abs(axis.y) != 1.0)
    {
        float3 up = float3(0.0, 1.0, 0.0);
        float3 right = fast::normalize(cross(up, axis));
        float3 front = cross(axis, right);
        return float3x3(float3(right), float3(axis), float3(front)) * randDir;
    }
    else
    {
        return randDir * sign(axis.y);
    }
}

static inline __attribute__((always_inline))
float3 UnpackNormalizedFloat3(thread const uint& bits)
{
    float3 v = float3(uint3(bits, bits >> uint(10), bits >> uint(20)) & uint3(1023u));
    return ((v / float3(1023.0)) * 2.0) - float3(1.0);
}

static inline __attribute__((always_inline))
float4x3 TRSMatrix(thread const float3& translation, thread const float3& rotation, thread const float3& scale)
{
    float3 s = sin(rotation);
    float3 c = cos(rotation);
    float4x3 m;
    m[0].x = scale.x * ((c.z * c.y) + ((s.z * s.x) * s.y));
    m[1].x = scale.y * (s.z * c.x);
    m[2].x = scale.z * ((c.z * (-s.y)) + ((s.z * s.x) * c.y));
    m[3].x = translation.x;
    m[0].y = scale.x * (((-s.z) * c.y) + ((c.z * s.x) * s.y));
    m[1].y = scale.y * (c.z * c.x);
    m[2].y = scale.z * (((-s.z) * (-s.y)) + ((c.z * s.x) * c.y));
    m[3].y = translation.y;
    m[0].z = scale.x * (c.x * s.y);
    m[1].z = scale.y * (-s.x);
    m[2].z = scale.z * (c.x * c.y);
    m[3].z = translation.z;
    return m;
}

static inline __attribute__((always_inline))
uint2 PackFloat4(thread const float4& v)
{
    uint4 v16 = uint4(as_type<uint>(half2(float2(v.x, 0.0))), as_type<uint>(half2(float2(v.y, 0.0))), as_type<uint>(half2(float2(v.z, 0.0))), as_type<uint>(half2(float2(v.w, 0.0))));
    return uint2(v16.x | (v16.y << uint(16)), v16.z | (v16.w << uint(16)));
}

static inline __attribute__((always_inline))
void _main(thread const uint3& dtid, constant cb1& _489, constant cb0& _513, const device EmitPoints& EmitPoints_1, device Particles& Particles_1)
{
    uint seed = _489.emitter.Seed ^ (_489.emitter.TotalEmitCount + dtid.x);
    float3 position = float3(_489.emitter.Transform[0][3], _489.emitter.Transform[1][3], _489.emitter.Transform[2][3]);
    uint param = seed;
    float3 param_1 = float3(_513.paramData.Direction);
    float param_2 = (_513.paramData.Spread * 3.1415920257568359375) / 180.0;
    float3 _528 = RandomSpread(param, param_1, param_2);
    seed = param;
    float3 direction = _528;
    uint param_3 = seed;
    float2 param_4 = _513.paramData.InitialSpeed;
    float _537 = RandomFloatRange(param_3, param_4);
    seed = param_3;
    float speed = _537;
    if (_513.paramData.EmitShapeType == 1u)
    {
        float3 lineStart = float4(_513.paramData.EmitShapeData[0].xyz, 0.0) * _489.emitter.Transform;
        float3 lineEnd = float4(_513.paramData.EmitShapeData[1].xyz, 0.0) * _489.emitter.Transform;
        float lineWidth = _513.paramData.EmitShapeData[1].w;
        uint param_5 = seed;
        float _577 = RandomFloat(param_5);
        seed = param_5;
        position += mix(lineStart, lineEnd, float3(_577));
        uint param_6 = seed;
        float3 _585 = RandomDirection(param_6);
        seed = param_6;
        position += ((_585 * lineWidth) * 0.5);
    }
    else
    {
        if (_513.paramData.EmitShapeType == 2u)
        {
            float3 circleAxis = float4(_513.paramData.EmitShapeData[0].xyz, 0.0) * _489.emitter.Transform;
            float circleInner = _513.paramData.EmitShapeData[1].x;
            float circleOuter = _513.paramData.EmitShapeData[1].y;
            uint param_7 = seed;
            float _621 = RandomFloat(param_7);
            seed = param_7;
            float circleRadius = mix(circleInner, circleOuter, _621);
            uint param_8 = seed;
            float3 param_9 = circleAxis;
            float3 _628 = RandomCircle(param_8, param_9);
            seed = param_8;
            position += (_628 * circleRadius);
        }
        else
        {
            if (_513.paramData.EmitShapeType == 3u)
            {
                float sphereRadius = _513.paramData.EmitShapeData[0].x;
                uint param_10 = seed;
                float3 _645 = RandomDirection(param_10);
                seed = param_10;
                position += (_645 * sphereRadius);
            }
            else
            {
                if (_513.paramData.EmitShapeType == 4u)
                {
                    float modelSize = _513.paramData.EmitShapeData[0].y;
                    if (_489.emitter.EmitPointCount > 0u)
                    {
                        uint param_11 = seed;
                        uint _668 = RandomUint(param_11);
                        seed = param_11;
                        uint emitIndex = _668 % _489.emitter.EmitPointCount;
                        EmitPoint emitPoint;
                        emitPoint.Position = float3(EmitPoints_1._data[emitIndex].Position);
                        emitPoint.Normal = EmitPoints_1._data[emitIndex].Normal;
                        emitPoint.Binormal = EmitPoints_1._data[emitIndex].Binormal;
                        emitPoint.Tangent = EmitPoints_1._data[emitIndex].Tangent;
                        emitPoint.UV = EmitPoints_1._data[emitIndex].UV;
                        emitPoint.VColor = EmitPoints_1._data[emitIndex].VColor;
                        position += (float4(emitPoint.Position * modelSize, 0.0) * _489.emitter.Transform);
                        uint param_12 = emitPoint.Normal;
                        float3 emitNormal = UnpackNormalizedFloat3(param_12);
                        uint param_13 = emitPoint.Binormal;
                        float3 emitBinormal = UnpackNormalizedFloat3(param_13);
                        uint param_14 = emitPoint.Tangent;
                        float3 emitTangent = UnpackNormalizedFloat3(param_14);
                        direction = float3x3(float3(fast::normalize(emitTangent)), float3(fast::normalize(emitBinormal)), float3(fast::normalize(emitNormal))) * direction;
                    }
                }
            }
        }
    }
    direction = float4(direction, 0.0) * _489.emitter.Transform;
    uint particleID = _489.emitter.ParticleHead + ((_489.emitter.TotalEmitCount + dtid.x) % _489.emitter.ParticleSize);
    ParticleData particle;
    particle.FlagBits = 1u;
    particle.Seed = seed;
    particle.LifeAge = 0.0;
    if (_513.paramData.ColorFlags == 0u)
    {
        particle.InheritColor = 4294967295u;
    }
    else
    {
        particle.InheritColor = _489.emitter.Color;
    }
    particle.Color = 4294967295u;
    particle.Padding = 0u;
    float3 param_15 = position;
    float3 param_16 = float3(0.0);
    float3 param_17 = float3(1.0);
    particle.Transform = TRSMatrix(param_15, param_16, param_17);
    float4 param_18 = float4(direction, speed);
    particle.DirectionSpeed = PackFloat4(param_18);
    Particles_1._data[particleID].FlagBits = particle.FlagBits;
    Particles_1._data[particleID].Seed = particle.Seed;
    Particles_1._data[particleID].LifeAge = particle.LifeAge;
    Particles_1._data[particleID].InheritColor = particle.InheritColor;
    Particles_1._data[particleID].DirectionSpeed = particle.DirectionSpeed;
    Particles_1._data[particleID].Color = particle.Color;
    Particles_1._data[particleID].Padding = particle.Padding;
    Particles_1._data[particleID].Transform = transpose(particle.Transform);
}

kernel void main0(constant cb0& _513 [[buffer(0)]], constant cb1& _489 [[buffer(1)]], device Particles& Particles_1 [[buffer(10)]], const device EmitPoints& EmitPoints_1 [[buffer(11)]], uint3 gl_GlobalInvocationID [[thread_position_in_grid]])
{
    uint3 dtid = gl_GlobalInvocationID;
    uint3 param = dtid;
    _main(param, _489, _513, EmitPoints_1, Particles_1);
}

)";

