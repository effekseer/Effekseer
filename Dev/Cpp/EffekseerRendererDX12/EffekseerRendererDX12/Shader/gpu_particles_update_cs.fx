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

struct ParticleData
{
    uint FlagBits;
    uint Seed;
    float LifeAge;
    uint InheritColor;
    uint Color;
    uint Direction;
    uint2 Velocity;
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

struct TrailData
{
    float3 Position;
    uint Direction;
};

struct ComputeConstants
{
    uint CoordinateReversed;
    float Reserved0;
    float Reserved1;
    float Reserved2;
};

cbuffer cb2 : register(b2)
{
    EmitterData _620_emitter : packoffset(c0);
};

RWByteAddressBuffer Particles : register(u0);
cbuffer cb1 : register(b1)
{
    ParameterData _687_paramData : packoffset(c0);
};

RWByteAddressBuffer Trails : register(u1);
cbuffer cb0 : register(b0)
{
    ComputeConstants _1224_constants : packoffset(c0);
};

Texture3D<float4> NoiseTex : register(t2);
SamplerState NoiseSamp : register(s2);
Texture2D<float4> GradientTex : register(t4);
SamplerState GradientSamp : register(s4);
Texture3D<float4> FieldTex : register(t3);
SamplerState FieldSamp : register(s3);

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
    uint _461 = RandomUint(param);
    seed = param;
    return float(_461) / 4294967296.0f;
}

float RandomFloatRange(inout uint seed, float2 maxmin)
{
    uint param = seed;
    float _474 = RandomFloat(param);
    seed = param;
    return lerp(maxmin.y, maxmin.x, _474);
}

float4 RandomFloat4Range(inout uint seed, float4 maxmin[2])
{
    uint param = seed;
    float _485 = RandomFloat(param);
    seed = param;
    return lerp(maxmin[1], maxmin[0], _485.xxxx);
}

float3 UnpackNormalizedFloat3(uint bits)
{
    float3 v = float3(uint3(bits, bits >> uint(10), bits >> uint(20)) & uint3(1023u, 1023u, 1023u));
    return ((v / 1023.0f.xxx) * 2.0f) - 1.0f.xxx;
}

float4 UnpackFloat4(uint2 bits)
{
    return float4(spvUnpackHalf2x16(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).x).x, spvUnpackHalf2x16(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).y).x, spvUnpackHalf2x16(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).z).x, spvUnpackHalf2x16(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).w).x);
}

uint PackNormalizedFloat3(float3 v)
{
    uint3 i = uint3(((v + 1.0f.xxx) * 0.5f) * 1023.0f);
    return (i.x | (i.y << uint(10))) | (i.z << uint(20));
}

float3 Vortex(float rotation, float attraction, inout float3 center, inout float3 axis, float3 position, float4x3 transform)
{
    center = transform[3] + center;
    axis = normalize(mul(float4(axis, 0.0f), transform));
    float3 localPos = position - center;
    float3 axisToPos = localPos - (axis * dot(axis, localPos));
    float _distance = length(axisToPos);
    if (_distance < 9.9999997473787516355514526367188e-05f)
    {
        return 0.0f.xxx;
    }
    float3 radial = normalize(axisToPos);
    float3 tangent = cross(axis, radial);
    return (tangent * rotation) - (radial * attraction);
}

float EasingSpeed(float t, float3 params)
{
    return ((((params.x * t) * t) * t) + ((params.y * t) * t)) + (params.z * t);
}

float4 UnpackColor(uint color32)
{
    return float4(float(color32 & 255u), float((color32 >> uint(8)) & 255u), float((color32 >> uint(16)) & 255u), float((color32 >> uint(24)) & 255u)) / 255.0f.xxxx;
}

float4 RandomColorRange(inout uint seed, uint2 maxmin)
{
    uint param = maxmin.y;
    uint param_1 = maxmin.x;
    uint param_2 = seed;
    float _501 = RandomFloat(param_2);
    seed = param_2;
    return lerp(UnpackColor(param), UnpackColor(param_1), _501.xxxx);
}

float3 HSV2RGB(float3 c)
{
    float4 k = float4(1.0f, 0.666666686534881591796875f, 0.3333333432674407958984375f, 3.0f);
    float3 p = abs((frac(c.xxx + k.xyz) * 6.0f) - k.www);
    return lerp(k.xxx, clamp(p - k.xxx, 0.0f.xxx, 1.0f.xxx), c.y.xxx) * c.z;
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

uint PackColor(float4 color)
{
    uint4 colori = uint4(clamp(color * 255.0f, 0.0f.xxxx, 255.0f.xxxx));
    return ((colori.x | (colori.y << uint(8))) | (colori.z << uint(16))) | (colori.w << uint(24));
}

void _main(uint3 dtid)
{
    uint particleID = _620_emitter.ParticleHead + dtid.x;
    ParticleData _638;
    _638.FlagBits = Particles.Load(particleID * 80 + 0);
    _638.Seed = Particles.Load(particleID * 80 + 4);
    _638.LifeAge = asfloat(Particles.Load(particleID * 80 + 8));
    _638.InheritColor = Particles.Load(particleID * 80 + 12);
    _638.Color = Particles.Load(particleID * 80 + 16);
    _638.Direction = Particles.Load(particleID * 80 + 20);
    _638.Velocity = Particles.Load2(particleID * 80 + 24);
    _638.Transform = asfloat(uint4x3(Particles.Load(particleID * 80 + 32), Particles.Load(particleID * 80 + 48), Particles.Load(particleID * 80 + 64), Particles.Load(particleID * 80 + 36), Particles.Load(particleID * 80 + 52), Particles.Load(particleID * 80 + 68), Particles.Load(particleID * 80 + 40), Particles.Load(particleID * 80 + 56), Particles.Load(particleID * 80 + 72), Particles.Load(particleID * 80 + 44), Particles.Load(particleID * 80 + 60), Particles.Load(particleID * 80 + 76)));
    ParticleData particle;
    particle.FlagBits = _638.FlagBits;
    particle.Seed = _638.Seed;
    particle.LifeAge = _638.LifeAge;
    particle.InheritColor = _638.InheritColor;
    particle.Color = _638.Color;
    particle.Direction = _638.Direction;
    particle.Velocity = _638.Velocity;
    particle.Transform = _638.Transform;
    if ((particle.FlagBits & 1u) != 0u)
    {
        uint updateCount = (particle.FlagBits >> uint(1)) & 255u;
        float deltaTime = _620_emitter.DeltaTime;
        uint seed = particle.Seed;
        uint param = seed;
        float2 param_1 = _687_paramData.LifeTime;
        float _694 = RandomFloatRange(param, param_1);
        seed = param;
        float lifeTime = _694;
        float lifeRatio = particle.LifeAge / lifeTime;
        uint param_2 = seed;
        float2 param_3 = _687_paramData.Damping;
        float _708 = RandomFloatRange(param_2, param_3);
        seed = param_2;
        float damping = _708 * 0.00999999977648258209228515625f;
        uint param_4 = seed;
        float4 param_5[2];
        param_5[0] = _687_paramData.AngularOffset[0];
        param_5[1] = _687_paramData.AngularOffset[1];
        float4 _724 = RandomFloat4Range(param_4, param_5);
        seed = param_4;
        float4 angularOffset = _724;
        uint param_6 = seed;
        float4 param_7[2];
        param_7[0] = _687_paramData.AngularVelocity[0];
        param_7[1] = _687_paramData.AngularVelocity[1];
        float4 _738 = RandomFloat4Range(param_6, param_7);
        seed = param_6;
        float4 angularVelocity = _738;
        float3 position = particle.Transform[3];
        float3 lastPosition = position;
        uint param_8 = particle.Direction;
        float3 direction = normalize(UnpackNormalizedFloat3(param_8));
        uint2 param_9 = particle.Velocity;
        float3 velocity = UnpackFloat4(param_9).xyz;
        if (_620_emitter.TrailSize > 0u)
        {
            uint trailID = (_620_emitter.TrailHead + (dtid.x * _687_paramData.ShapeData)) + _620_emitter.TrailPhase;
            TrailData trail;
            trail.Position = position;
            float3 param_10 = direction;
            trail.Direction = PackNormalizedFloat3(param_10);
            Trails.Store3(trailID * 16 + 0, asuint(trail.Position));
            Trails.Store(trailID * 16 + 12, trail.Direction);
        }
        particle.FlagBits &= 4294966785u;
        particle.FlagBits |= (clamp(updateCount + 1u, 0u, 255u) << uint(1));
        particle.LifeAge += deltaTime;
        if (particle.LifeAge >= lifeTime)
        {
            particle.FlagBits &= 4294967294u;
        }
        velocity += (_687_paramData.Gravity * deltaTime);
        float speed = length(velocity);
        if (speed > 0.0f)
        {
            float newSpeed = max(0.0f, speed - (damping * deltaTime));
            velocity *= (newSpeed / speed);
        }
        position += (velocity * deltaTime);
        if ((_687_paramData.VortexRotation != 0.0f) || (_687_paramData.VortexAttraction != 0.0f))
        {
            float param_11 = _687_paramData.VortexRotation;
            float param_12 = _687_paramData.VortexAttraction;
            float3 param_13 = _687_paramData.VortexCenter;
            float3 param_14 = _687_paramData.VortexAxis;
            float3 param_15 = position;
            float4x3 param_16 = _620_emitter.Transform;
            float3 _888 = Vortex(param_11, param_12, param_13, param_14, param_15, param_16);
            position += (_888 * deltaTime);
        }
        if (_687_paramData.TurbulencePower != 0.0f)
        {
            float4 vfTexel = NoiseTex.SampleLevel(NoiseSamp, (position * _687_paramData.TurbulenceScale) + 0.5f.xxx, 0.0f);
            position += ((((vfTexel.xyz * 2.0f) - 1.0f.xxx) * _687_paramData.TurbulencePower) * deltaTime);
        }
        float3 diff = position - lastPosition;
        if (length(diff) > 9.9999997473787516355514526367188e-05f)
        {
            direction = normalize(diff);
        }
        float3 rotation = angularOffset.xyz + (angularVelocity.xyz * particle.LifeAge);
        float4 scale = 1.0f.xxxx;
        uint scaleMode = _687_paramData.ScaleFlags & 7u;
        if (scaleMode == 0u)
        {
            uint param_17 = seed;
            float4 param_18[2];
            param_18[0] = _687_paramData.ScaleData1[0];
            param_18[1] = _687_paramData.ScaleData1[1];
            float4 _971 = RandomFloat4Range(param_17, param_18);
            seed = param_17;
            scale = _971;
        }
        else
        {
            if (scaleMode == 2u)
            {
                uint param_19 = seed;
                float4 param_20[2];
                param_20[0] = _687_paramData.ScaleData1[0];
                param_20[1] = _687_paramData.ScaleData1[1];
                float4 _988 = RandomFloat4Range(param_19, param_20);
                seed = param_19;
                float4 scale1 = _988;
                uint param_21 = seed;
                float4 param_22[2];
                param_22[0] = _687_paramData.ScaleData2[0];
                param_22[1] = _687_paramData.ScaleData2[1];
                float4 _1002 = RandomFloat4Range(param_21, param_22);
                seed = param_21;
                float4 scale2 = _1002;
                float param_23 = lifeRatio;
                float3 param_24 = _687_paramData.ScaleEasing;
                scale = lerp(scale1, scale2, EasingSpeed(param_23, param_24).xxxx);
            }
        }
        uint colorMode = _687_paramData.ColorFlags & 7u;
        float4 color = 1.0f.xxxx;
        if (colorMode == 0u)
        {
            uint param_25 = _687_paramData.ColorData.x;
            color = UnpackColor(param_25);
        }
        else
        {
            if (colorMode == 1u)
            {
                uint param_26 = seed;
                uint2 param_27 = _687_paramData.ColorData.xy;
                float4 _1041 = RandomColorRange(param_26, param_27);
                seed = param_26;
                color = _1041;
            }
            else
            {
                if (colorMode == 2u)
                {
                    uint param_28 = seed;
                    uint2 param_29 = _687_paramData.ColorData.xy;
                    float4 _1055 = RandomColorRange(param_28, param_29);
                    seed = param_28;
                    float4 colorStart = _1055;
                    uint param_30 = seed;
                    uint2 param_31 = _687_paramData.ColorData.zw;
                    float4 _1064 = RandomColorRange(param_30, param_31);
                    seed = param_30;
                    float4 colorEnd = _1064;
                    float param_32 = lifeRatio;
                    float3 param_33 = _687_paramData.ColorEasing;
                    color = lerp(colorStart, colorEnd, EasingSpeed(param_32, param_33).xxxx);
                }
                else
                {
                    if ((colorMode == 3u) || (colorMode == 4u))
                    {
                        color = GradientTex.SampleLevel(GradientSamp, float2(lifeRatio, 0.0f), 0.0f);
                    }
                }
            }
        }
        if (((_687_paramData.ColorFlags >> uint(5)) & 1u) != 0u)
        {
            float3 param_34 = color.xyz;
            float3 _1106 = HSV2RGB(param_34);
            color.x = _1106.x;
            color.y = _1106.y;
            color.z = _1106.z;
        }
        uint colorInherit = (_687_paramData.ColorFlags >> uint(3)) & 3u;
        if ((colorInherit == 2u) || (colorInherit == 3u))
        {
            uint param_35 = _620_emitter.Color;
            color *= UnpackColor(param_35);
        }
        else
        {
            uint param_36 = particle.InheritColor;
            color *= UnpackColor(param_36);
        }
        color.w *= clamp(particle.LifeAge / _687_paramData.FadeIn, 0.0f, 1.0f);
        color.w *= clamp((lifeTime - particle.LifeAge) / _687_paramData.FadeOut, 0.0f, 1.0f);
        float3 param_37 = position;
        float3 param_38 = rotation;
        float3 param_39 = (scale.xyz * scale.w) * _687_paramData.ShapeSize;
        particle.Transform = TRSMatrix(param_37, param_38, param_39);
        float4 param_40 = float4(velocity, 0.0f);
        particle.Velocity = PackFloat4(param_40);
        float3 param_41 = direction;
        particle.Direction = PackNormalizedFloat3(param_41);
        float4 param_42 = color;
        particle.Color = PackColor(param_42);
        Particles.Store(particleID * 80 + 0, particle.FlagBits);
        Particles.Store(particleID * 80 + 4, particle.Seed);
        Particles.Store(particleID * 80 + 8, asuint(particle.LifeAge));
        Particles.Store(particleID * 80 + 12, particle.InheritColor);
        Particles.Store(particleID * 80 + 16, particle.Color);
        Particles.Store(particleID * 80 + 20, particle.Direction);
        Particles.Store2(particleID * 80 + 24, particle.Velocity);
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
}

void comp_main()
{
    uint3 dtid = gl_GlobalInvocationID;
    uint3 param = dtid;
    _main(param);
}

[numthreads(256, 1, 1)]
void main(SPIRV_Cross_Input stage_input)
{
    gl_GlobalInvocationID = stage_input.gl_GlobalInvocationID;
    comp_main();
}
