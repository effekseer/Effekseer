#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct ParticleData
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint Color;
    uint Direction;
    uint2 Velocity;
    float4x3 Transform;
};

struct VS_Input
{
    float3 Pos;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float2 UV;
    float4 Color;
    uint InstanceID;
    uint VertexID;
};

struct VS_Output
{
    float4 Pos;
    float2 UV;
    float4 Color;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
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

struct cb1
{
    ParameterData paramData;
};

struct RenderConstants
{
    packed_float3 CameraPos;
    uint CoordinateReversed;
    packed_float3 CameraFront;
    float Reserved1;
    packed_float3 LightDir;
    float Reserved2;
    float4 LightColor;
    float4 LightAmbient;
    float4x4 ProjMat;
    float4x4 CameraMat;
    float3x4 BillboardMat;
    float3x4 YAxisFixedMat;
};

struct cb0
{
    RenderConstants constants;
};

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

struct cb2
{
    EmitterData emitter;
};

struct TrailData
{
    float3 Position;
    uint Direction;
};

struct TrailData_1
{
    packed_float3 Position;
    uint Direction;
};

struct Trails
{
    TrailData_1 _data[1];
};

struct ParticleData_1
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint Color;
    uint Direction;
    uint2 Velocity;
    float3x4 Transform;
};

struct Particles
{
    ParticleData_1 _data[1];
};

struct main0_out
{
    float2 _entryPointOutput_UV [[user(locn0)]];
    float4 _entryPointOutput_Color [[user(locn1)]];
    float3 _entryPointOutput_WorldN [[user(locn2)]];
    float3 _entryPointOutput_WorldB [[user(locn3)]];
    float3 _entryPointOutput_WorldT [[user(locn4)]];
    float4 gl_Position [[position]];
};

struct main0_in
{
    float3 input_Pos [[attribute(0)]];
    float3 input_Normal [[attribute(1)]];
    float3 input_Binormal [[attribute(2)]];
    float3 input_Tangent [[attribute(3)]];
    float2 input_UV [[attribute(4)]];
    float4 input_Color [[attribute(5)]];
};

static inline __attribute__((always_inline))
float3 UnpackNormalizedFloat3(thread const uint& bits)
{
    float3 v = float3(uint3(bits, bits >> uint(10), bits >> uint(20)) & uint3(1023u));
    return ((v / float3(1023.0)) * 2.0) - float3(1.0);
}

static inline __attribute__((always_inline))
void transformSprite(thread const ParticleData& particle, thread float3& position, constant cb1& _121, constant cb0& _136)
{
    position = particle.Transform * float4(position, 0.0);
    if (_121.paramData.ShapeData == 0u)
    {
        position = float4(position, 0.0) * _136.constants.BillboardMat;
    }
    else
    {
        if (_121.paramData.ShapeData == 1u)
        {
            uint param = particle.Direction;
            float3 U = fast::normalize(UnpackNormalizedFloat3(param));
            float3 F = float3(_136.constants.CameraFront);
            float3 R = fast::normalize(cross(U, F));
            F = fast::normalize(cross(R, U));
            position = float3x3(float3(R), float3(U), float3(F)) * position;
        }
        else
        {
            if (_121.paramData.ShapeData == 2u)
            {
                position = float4(position, 0.0) * _136.constants.YAxisFixedMat;
            }
        }
    }
    position += particle.Transform[3];
}

static inline __attribute__((always_inline))
void transformModel(thread const ParticleData& particle, thread float3& position, constant cb0& _136)
{
    if (_136.constants.CoordinateReversed != 0u)
    {
        position.z = -position.z;
    }
    position = particle.Transform * float4(position, 1.0);
}

static inline __attribute__((always_inline))
void transformTrail(thread const ParticleData& particle, thread float3& position, thread float2& uv, thread const uint& instanceID, thread const uint& vertexID, constant cb1& _121, constant cb0& _136, constant cb2& _265, const device Trails& Trails_1)
{
    uint updateCount = (particle.FlagBits >> uint(1)) & 255u;
    uint trailLength = min(_121.paramData.ShapeData, updateCount);
    uint segmentID = min((vertexID / 2u), trailLength);
    float3 trailPosition;
    float3 trailDirection;
    if (segmentID == 0u)
    {
        trailPosition = particle.Transform[3];
        uint param = particle.Direction;
        trailDirection = fast::normalize(UnpackNormalizedFloat3(param));
    }
    else
    {
        uint trailID = _265.emitter.TrailHead + (instanceID * _121.paramData.ShapeData);
        trailID += ((((_121.paramData.ShapeData + _265.emitter.TrailPhase) - segmentID) + 1u) % _121.paramData.ShapeData);
        TrailData trail;
        trail.Position = float3(Trails_1._data[trailID].Position);
        trail.Direction = Trails_1._data[trailID].Direction;
        trailPosition = trail.Position;
        uint param_1 = trail.Direction;
        trailDirection = fast::normalize(UnpackNormalizedFloat3(param_1));
        uv.y = float(segmentID) / float(trailLength);
    }
    float3 trailTangent = fast::normalize(cross(float3(_136.constants.CameraFront), trailDirection));
    position = (trailTangent * position.x) * _121.paramData.ShapeSize;
    position += trailPosition;
}

static inline __attribute__((always_inline))
float4 UnpackColor(thread const uint& color32)
{
    return float4(float(color32 & 255u), float((color32 >> uint(8)) & 255u), float((color32 >> uint(16)) & 255u), float((color32 >> uint(24)) & 255u)) / float4(255.0);
}

static inline __attribute__((always_inline))
VS_Output _main(VS_Input _input, constant cb1& _121, constant cb0& _136, constant cb2& _265, const device Trails& Trails_1, const device Particles& Particles_1)
{
    uint index = _265.emitter.ParticleHead + _input.InstanceID;
    ParticleData particle;
    particle.FlagBits = Particles_1._data[index].FlagBits;
    particle.Seed = Particles_1._data[index].Seed;
    particle.LifeAge = Particles_1._data[index].LifeAge;
    particle.InheritColor = Particles_1._data[index].InheritColor;
    particle.Color = Particles_1._data[index].Color;
    particle.Direction = Particles_1._data[index].Direction;
    particle.Velocity = Particles_1._data[index].Velocity;
    particle.Transform = transpose(Particles_1._data[index].Transform);
    VS_Output _output;
    if ((particle.FlagBits & 1u) != 0u)
    {
        float3 position = _input.Pos;
        float2 uv = _input.UV;
        float4 color = _input.Color;
        if (_121.paramData.ShapeType == 0u)
        {
            ParticleData param = particle;
            float3 param_1 = position;
            transformSprite(param, param_1, _121, _136);
            position = param_1;
        }
        else
        {
            if (_121.paramData.ShapeType == 1u)
            {
                ParticleData param_2 = particle;
                float3 param_3 = position;
                transformModel(param_2, param_3, _136);
                position = param_3;
            }
            else
            {
                if (_121.paramData.ShapeType == 2u)
                {
                    ParticleData param_4 = particle;
                    float3 param_5 = position;
                    float2 param_6 = uv;
                    uint param_7 = _input.InstanceID;
                    uint param_8 = _input.VertexID;
                    transformTrail(param_4, param_5, param_6, param_7, param_8, _121, _136, _265, Trails_1);
                    position = param_5;
                    uv = param_6;
                }
            }
        }
        uint param_9 = particle.Color;
        color *= UnpackColor(param_9);
        float4 _432 = color;
        float3 _434 = _432.xyz * _121.paramData.Emissive;
        color.x = _434.x;
        color.y = _434.y;
        color.z = _434.z;
        _output.Pos = _136.constants.ProjMat * (_136.constants.CameraMat * float4(position, 1.0));
        _output.UV = uv;
        _output.Color = color;
        if (_121.paramData.MaterialType == 1u)
        {
            _output.WorldN = particle.Transform * float4(_input.Normal, 0.0);
            _output.WorldB = particle.Transform * float4(_input.Binormal, 0.0);
            _output.WorldT = particle.Transform * float4(_input.Tangent, 0.0);
        }
    }
    else
    {
        _output.Pos = float4(0.0);
        _output.UV = float2(0.0);
        _output.Color = float4(0.0);
        if (_121.paramData.MaterialType == 1u)
        {
            _output.WorldN = float3(0.0);
            _output.WorldB = float3(0.0);
            _output.WorldT = float3(0.0);
        }
    }
    return _output;
}

vertex main0_out main0(main0_in in [[stage_in]], constant cb0& _136 [[buffer(0)]], constant cb1& _121 [[buffer(1)]], constant cb2& _265 [[buffer(2)]], const device Particles& Particles_1 [[buffer(10)]], const device Trails& Trails_1 [[buffer(11)]], uint gl_InstanceIndex [[instance_id]], uint gl_VertexIndex [[vertex_id]])
{
    main0_out out = {};
    VS_Input _input;
    _input.Pos = in.input_Pos;
    _input.Normal = in.input_Normal;
    _input.Binormal = in.input_Binormal;
    _input.Tangent = in.input_Tangent;
    _input.UV = in.input_UV;
    _input.Color = in.input_Color;
    _input.InstanceID = gl_InstanceIndex;
    _input.VertexID = gl_VertexIndex;
    VS_Output flattenTemp = _main(_input, _121, _136, _265, Trails_1, Particles_1);
    out.gl_Position = flattenTemp.Pos;
    out._entryPointOutput_UV = flattenTemp.UV;
    out._entryPointOutput_Color = flattenTemp.Color;
    out._entryPointOutput_WorldN = flattenTemp.WorldN;
    out._entryPointOutput_WorldB = flattenTemp.WorldB;
    out._entryPointOutput_WorldT = flattenTemp.WorldT;
    return out;
}

