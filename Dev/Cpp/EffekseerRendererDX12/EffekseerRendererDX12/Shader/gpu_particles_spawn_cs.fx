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
    column_major float4x3 Transform;
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

struct EmitPoint
{
    float3 Position;
    uint Normal;
    uint Binormal;
    uint Tangent;
    uint UV;
    uint VColor;
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
    column_major float4x3 Transform;
};

cbuffer cb1 : register(b1)
{
    EmitterData _489_emitter : packoffset(c0);
};

cbuffer cb0 : register(b0)
{
    ParameterData _513_paramData : packoffset(c0);
};

ByteAddressBuffer EmitPoints : register(t1);
RWByteAddressBuffer Particles : register(u0);

static uint3 gl_GlobalInvocationID;
struct SPIRV_Cross_Input
{
    uint3 gl_GlobalInvocationID : SV_DispatchThreadID;
};

uint spvPackHalf2x16(float2 value)
{
    uint2 Packed = f32tof16(value);
    return Packed.x | (Packed.y << 16);
}

float2 spvUnpackHalf2x16(uint value)
{
    return f16tof32(uint2(value & 0xffff, value >> 16));
}

uint RandomUint(inout uint seed)
{
    uint state = seed;
    seed = (seed * 747796405u) + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float RandomFloat(inout uint seed)
{
    uint param = seed;
    uint _302 = RandomUint(param);
    seed = param;
    return float(_302) / 4294967296.0f;
}

float3 RandomSpread(inout uint seed, inout float3 baseDir, float angle)
{
    uint param = seed;
    float _415 = RandomFloat(param);
    seed = param;
    float theta = 6.28318023681640625f * _415;
    uint param_1 = seed;
    float _422 = RandomFloat(param_1);
    seed = param_1;
    float phi = angle * _422;
    float3 randDir = float3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    baseDir = normalize(baseDir);
    if (abs(baseDir.z) != 1.0f)
    {
        float3 front = float3(0.0f, 0.0f, 1.0f);
        float3 right = normalize(cross(front, baseDir));
        float3 up = cross(baseDir, right);
        return mul(randDir, float3x3(float3(right), float3(up), float3(baseDir)));
    }
    else
    {
        return randDir * sign(baseDir.z);
    }
}

float RandomFloatRange(inout uint seed, float2 maxmin)
{
    uint param = seed;
    float _315 = RandomFloat(param);
    seed = param;
    return lerp(maxmin.y, maxmin.x, _315);
}

float3 RandomDirection(inout uint seed)
{
    uint param = seed;
    float _324 = RandomFloat(param);
    seed = param;
    float theta = 6.28318023681640625f * _324;
    uint param_1 = seed;
    float _330 = RandomFloat(param_1);
    seed = param_1;
    float phi = 6.28318023681640625f * _330;
    float3 randDir = float3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi));
    return randDir;
}

float3 RandomCircle(inout uint seed, inout float3 axis)
{
    uint param = seed;
    float _353 = RandomFloat(param);
    seed = param;
    float theta = 6.28318023681640625f * _353;
    float3 randDir = float3(cos(theta), 0.0f, sin(theta));
    axis = normalize(axis);
    if (abs(axis.y) != 1.0f)
    {
        float3 up = float3(0.0f, 1.0f, 0.0f);
        float3 right = normalize(cross(up, axis));
        float3 front = cross(axis, right);
        return mul(randDir, float3x3(float3(right), float3(axis), float3(front)));
    }
    else
    {
        return randDir * sign(axis.y);
    }
}

float3 UnpackNormalizedFloat3(uint bits)
{
    float3 v = float3(uint3(bits, bits >> uint(10), bits >> uint(20)) & uint3(1023u, 1023u, 1023u));
    return ((v / 1023.0f.xxx) * 2.0f) - 1.0f.xxx;
}

float4x3 TRSMatrix(float3 translation, float3 rotation, float3 scale)
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

uint2 PackFloat4(float4 v)
{
    uint4 v16 = uint4(spvPackHalf2x16(float2(v.x, 0.0f)), spvPackHalf2x16(float2(v.y, 0.0f)), spvPackHalf2x16(float2(v.z, 0.0f)), spvPackHalf2x16(float2(v.w, 0.0f)));
    return uint2(v16.x | (v16.y << uint(16)), v16.z | (v16.w << uint(16)));
}

void _main(uint3 dtid)
{
    uint seed = _489_emitter.Seed ^ (_489_emitter.TotalEmitCount + dtid.x);
    float3 position = _489_emitter.Transform[3];
    uint param = seed;
    float3 param_1 = _513_paramData.Direction;
    float param_2 = (_513_paramData.Spread * 3.1415920257568359375f) / 180.0f;
    float3 _528 = RandomSpread(param, param_1, param_2);
    seed = param;
    float3 direction = _528;
    uint param_3 = seed;
    float2 param_4 = _513_paramData.InitialSpeed;
    float _537 = RandomFloatRange(param_3, param_4);
    seed = param_3;
    float speed = _537;
    if (_513_paramData.EmitShapeType == 1u)
    {
        float3 lineStart = mul(float4(_513_paramData.EmitShapeData[0].xyz, 0.0f), _489_emitter.Transform);
        float3 lineEnd = mul(float4(_513_paramData.EmitShapeData[1].xyz, 0.0f), _489_emitter.Transform);
        float lineWidth = _513_paramData.EmitShapeData[1].w;
        uint param_5 = seed;
        float _577 = RandomFloat(param_5);
        seed = param_5;
        position += lerp(lineStart, lineEnd, _577.xxx);
        uint param_6 = seed;
        float3 _585 = RandomDirection(param_6);
        seed = param_6;
        position += ((_585 * lineWidth) * 0.5f);
    }
    else
    {
        if (_513_paramData.EmitShapeType == 2u)
        {
            float3 circleAxis = mul(float4(_513_paramData.EmitShapeData[0].xyz, 0.0f), _489_emitter.Transform);
            float circleInner = _513_paramData.EmitShapeData[1].x;
            float circleOuter = _513_paramData.EmitShapeData[1].y;
            uint param_7 = seed;
            float _621 = RandomFloat(param_7);
            seed = param_7;
            float circleRadius = lerp(circleInner, circleOuter, _621);
            uint param_8 = seed;
            float3 param_9 = circleAxis;
            float3 _628 = RandomCircle(param_8, param_9);
            seed = param_8;
            position += (_628 * circleRadius);
        }
        else
        {
            if (_513_paramData.EmitShapeType == 3u)
            {
                float sphereRadius = _513_paramData.EmitShapeData[0].x;
                uint param_10 = seed;
                float3 _645 = RandomDirection(param_10);
                seed = param_10;
                position += (_645 * sphereRadius);
            }
            else
            {
                if (_513_paramData.EmitShapeType == 4u)
                {
                    float modelSize = _513_paramData.EmitShapeData[0].y;
                    if (_489_emitter.EmitPointCount > 0u)
                    {
                        uint param_11 = seed;
                        uint _668 = RandomUint(param_11);
                        seed = param_11;
                        uint emitIndex = _668 % _489_emitter.EmitPointCount;
                        EmitPoint _684;
                        _684.Position = asfloat(EmitPoints.Load3(emitIndex * 32 + 0));
                        _684.Normal = EmitPoints.Load(emitIndex * 32 + 12);
                        _684.Binormal = EmitPoints.Load(emitIndex * 32 + 16);
                        _684.Tangent = EmitPoints.Load(emitIndex * 32 + 20);
                        _684.UV = EmitPoints.Load(emitIndex * 32 + 24);
                        _684.VColor = EmitPoints.Load(emitIndex * 32 + 28);
                        EmitPoint emitPoint;
                        emitPoint.Position = _684.Position;
                        emitPoint.Normal = _684.Normal;
                        emitPoint.Binormal = _684.Binormal;
                        emitPoint.Tangent = _684.Tangent;
                        emitPoint.UV = _684.UV;
                        emitPoint.VColor = _684.VColor;
                        position += mul(float4(emitPoint.Position * modelSize, 0.0f), _489_emitter.Transform);
                        uint param_12 = emitPoint.Normal;
                        float3 emitNormal = UnpackNormalizedFloat3(param_12);
                        uint param_13 = emitPoint.Binormal;
                        float3 emitBinormal = UnpackNormalizedFloat3(param_13);
                        uint param_14 = emitPoint.Tangent;
                        float3 emitTangent = UnpackNormalizedFloat3(param_14);
                        direction = mul(direction, float3x3(float3(normalize(emitTangent)), float3(normalize(emitBinormal)), float3(normalize(emitNormal))));
                    }
                }
            }
        }
    }
    direction = mul(float4(direction, 0.0f), _489_emitter.Transform);
    uint particleID = _489_emitter.ParticleHead + ((_489_emitter.TotalEmitCount + dtid.x) % _489_emitter.ParticleSize);
    ParticleData particle;
    particle.FlagBits = 1u;
    particle.Seed = seed;
    particle.LifeAge = 0.0f;
    if (_513_paramData.ColorFlags == 0u)
    {
        particle.InheritColor = 4294967295u;
    }
    else
    {
        particle.InheritColor = _489_emitter.Color;
    }
    particle.Color = 4294967295u;
    particle.Padding = 0u;
    float3 param_15 = position;
    float3 param_16 = 0.0f.xxx;
    float3 param_17 = 1.0f.xxx;
    particle.Transform = TRSMatrix(param_15, param_16, param_17);
    float4 param_18 = float4(direction, speed);
    particle.DirectionSpeed = PackFloat4(param_18);
    Particles.Store(particleID * 80 + 0, particle.FlagBits);
    Particles.Store(particleID * 80 + 4, particle.Seed);
    Particles.Store(particleID * 80 + 8, asuint(particle.LifeAge));
    Particles.Store(particleID * 80 + 12, particle.InheritColor);
    Particles.Store2(particleID * 80 + 16, particle.DirectionSpeed);
    Particles.Store(particleID * 80 + 24, particle.Color);
    Particles.Store(particleID * 80 + 28, particle.Padding);
    Particles.Store(particleID * 80 + 32, asuint(particle.Transform[0].x));
    Particles.Store(particleID * 80 + 36, asuint(particle.Transform[1].x));
    Particles.Store(particleID * 80 + 40, asuint(particle.Transform[2].x));
    Particles.Store(particleID * 80 + 44, asuint(particle.Transform[3].x));
    Particles.Store(particleID * 80 + 48, asuint(particle.Transform[0].y));
    Particles.Store(particleID * 80 + 52, asuint(particle.Transform[1].y));
    Particles.Store(particleID * 80 + 56, asuint(particle.Transform[2].y));
    Particles.Store(particleID * 80 + 60, asuint(particle.Transform[3].y));
    Particles.Store(particleID * 80 + 64, asuint(particle.Transform[0].z));
    Particles.Store(particleID * 80 + 68, asuint(particle.Transform[1].z));
    Particles.Store(particleID * 80 + 72, asuint(particle.Transform[2].z));
    Particles.Store(particleID * 80 + 76, asuint(particle.Transform[3].z));
}

void comp_main()
{
    uint3 dtid = gl_GlobalInvocationID;
    uint3 param = dtid;
    _main(param);
}

[numthreads(1, 1, 1)]
void main(SPIRV_Cross_Input stage_input)
{
    gl_GlobalInvocationID = stage_input.gl_GlobalInvocationID;
    comp_main();
}
