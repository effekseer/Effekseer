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
    uint2 DirectionSpeed;
    uint Color;
    uint Padding;
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

struct TrailData
{
    float3 Position;
    uint Direction;
};

cbuffer cb1 : register(b1)
{
    EmitterData _592_emitter : packoffset(c0);
};

RWByteAddressBuffer Particles : register(u0);
cbuffer cb0 : register(b0)
{
    ParameterData _659_paramData : packoffset(c0);
};

RWByteAddressBuffer Trails : register(u1);
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
    uint _437 = RandomUint(param);
    seed = param;
    return float(_437) / 4294967296.0f;
}

float RandomFloatRange(inout uint seed, float2 maxmin)
{
    uint param = seed;
    float _450 = RandomFloat(param);
    seed = param;
    return lerp(maxmin.y, maxmin.x, _450);
}

float4 RandomFloat4Range(inout uint seed, float4 maxmin[2])
{
    uint param = seed;
    float _461 = RandomFloat(param);
    seed = param;
    return lerp(maxmin[1], maxmin[0], _461.xxxx);
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
    float3 diff = position - center;
    float _distance = length(diff);
    if (_distance == 0.0f)
    {
        return 0.0f.xxx;
    }
    float3 radial = diff / _distance.xxx;
    float3 tangent = cross(axis, radial);
    radial = cross(tangent, axis);
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
    float _477 = RandomFloat(param_2);
    seed = param_2;
    return lerp(UnpackColor(param), UnpackColor(param_1), _477.xxxx);
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
    uint particleID = _592_emitter.ParticleHead + dtid.x;
    ParticleData _610;
    _610.FlagBits = Particles.Load(particleID * 80 + 0);
    _610.Seed = Particles.Load(particleID * 80 + 4);
    _610.LifeAge = asfloat(Particles.Load(particleID * 80 + 8));
    _610.InheritColor = Particles.Load(particleID * 80 + 12);
    _610.DirectionSpeed = Particles.Load2(particleID * 80 + 16);
    _610.Color = Particles.Load(particleID * 80 + 24);
    _610.Padding = Particles.Load(particleID * 80 + 28);
    _610.Transform = asfloat(uint4x3(Particles.Load(particleID * 80 + 32), Particles.Load(particleID * 80 + 48), Particles.Load(particleID * 80 + 64), Particles.Load(particleID * 80 + 36), Particles.Load(particleID * 80 + 52), Particles.Load(particleID * 80 + 68), Particles.Load(particleID * 80 + 40), Particles.Load(particleID * 80 + 56), Particles.Load(particleID * 80 + 72), Particles.Load(particleID * 80 + 44), Particles.Load(particleID * 80 + 60), Particles.Load(particleID * 80 + 76)));
    ParticleData particle;
    particle.FlagBits = _610.FlagBits;
    particle.Seed = _610.Seed;
    particle.LifeAge = _610.LifeAge;
    particle.InheritColor = _610.InheritColor;
    particle.DirectionSpeed = _610.DirectionSpeed;
    particle.Color = _610.Color;
    particle.Padding = _610.Padding;
    particle.Transform = _610.Transform;
    if ((particle.FlagBits & 1u) != 0u)
    {
        uint updateCount = (particle.FlagBits >> uint(1)) & 255u;
        float deltaTime = _592_emitter.DeltaTime;
        uint seed = particle.Seed;
        uint param = seed;
        float2 param_1 = _659_paramData.LifeTime;
        float _666 = RandomFloatRange(param, param_1);
        seed = param;
        float lifeTime = _666;
        float lifeRatio = particle.LifeAge / lifeTime;
        uint param_2 = seed;
        float2 param_3 = _659_paramData.Damping;
        float _680 = RandomFloatRange(param_2, param_3);
        seed = param_2;
        float damping = _680 * 0.00999999977648258209228515625f;
        uint param_4 = seed;
        float4 param_5[2];
        param_5[0] = _659_paramData.AngularOffset[0];
        param_5[1] = _659_paramData.AngularOffset[1];
        float4 _696 = RandomFloat4Range(param_4, param_5);
        seed = param_4;
        float4 angularOffset = _696;
        uint param_6 = seed;
        float4 param_7[2];
        param_7[0] = _659_paramData.AngularVelocity[0];
        param_7[1] = _659_paramData.AngularVelocity[1];
        float4 _710 = RandomFloat4Range(param_6, param_7);
        seed = param_6;
        float4 angularVelocity = _710;
        float3 position = particle.Transform[3];
        uint2 param_8 = particle.DirectionSpeed;
        float4 directionSpeed = UnpackFloat4(param_8);
        float3 velocity = directionSpeed.xyz * directionSpeed.w;
        if (_592_emitter.TrailSize > 0u)
        {
            uint trailID = (_592_emitter.TrailHead + (dtid.x * _659_paramData.ShapeData)) + _592_emitter.TrailPhase;
            TrailData trail;
            trail.Position = position;
            float3 param_9 = directionSpeed.xyz;
            trail.Direction = PackNormalizedFloat3(param_9);
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
        velocity += (_659_paramData.Gravity * deltaTime);
        if ((_659_paramData.VortexRotation != 0.0f) || (_659_paramData.VortexAttraction != 0.0f))
        {
            float param_10 = _659_paramData.VortexRotation;
            float param_11 = _659_paramData.VortexAttraction;
            float3 param_12 = _659_paramData.VortexCenter;
            float3 param_13 = _659_paramData.VortexAxis;
            float3 param_14 = position;
            float4x3 param_15 = _592_emitter.Transform;
            float3 _834 = Vortex(param_10, param_11, param_12, param_13, param_14, param_15);
            velocity += (_834 * deltaTime);
        }
        if (_659_paramData.TurbulencePower != 0.0f)
        {
            float4 vfTexel = NoiseTex.SampleLevel(NoiseSamp, (position / 8.0f.xxx) + 0.5f.xxx, 0.0f);
            velocity += ((((vfTexel.xyz * 2.0f) - 1.0f.xxx) * _659_paramData.TurbulencePower) * deltaTime);
        }
        float speed = length(velocity);
        if (speed > 0.0f)
        {
            float newSpeed = max(0.0f, speed - (damping * deltaTime));
            velocity *= (newSpeed / speed);
        }
        position += (velocity * deltaTime);
        directionSpeed.w = length(velocity);
        if (directionSpeed.w > 9.9999997473787516355514526367188e-05f)
        {
            float3 _909 = normalize(velocity);
            directionSpeed.x = _909.x;
            directionSpeed.y = _909.y;
            directionSpeed.z = _909.z;
        }
        float3 rotation = angularOffset.xyz + (angularVelocity.xyz * particle.LifeAge);
        float4 scale = 1.0f.xxxx;
        uint scaleMode = _659_paramData.ScaleFlags & 7u;
        if (scaleMode == 0u)
        {
            uint param_16 = seed;
            float4 param_17[2];
            param_17[0] = _659_paramData.ScaleData1[0];
            param_17[1] = _659_paramData.ScaleData1[1];
            float4 _947 = RandomFloat4Range(param_16, param_17);
            seed = param_16;
            scale = _947;
        }
        else
        {
            if (scaleMode == 2u)
            {
                uint param_18 = seed;
                float4 param_19[2];
                param_19[0] = _659_paramData.ScaleData1[0];
                param_19[1] = _659_paramData.ScaleData1[1];
                float4 _964 = RandomFloat4Range(param_18, param_19);
                seed = param_18;
                float4 scale1 = _964;
                uint param_20 = seed;
                float4 param_21[2];
                param_21[0] = _659_paramData.ScaleData2[0];
                param_21[1] = _659_paramData.ScaleData2[1];
                float4 _978 = RandomFloat4Range(param_20, param_21);
                seed = param_20;
                float4 scale2 = _978;
                float param_22 = lifeRatio;
                float3 param_23 = _659_paramData.ScaleEasing;
                scale = lerp(scale1, scale2, EasingSpeed(param_22, param_23).xxxx);
            }
        }
        uint colorMode = _659_paramData.ColorFlags & 7u;
        float4 color = 1.0f.xxxx;
        if (colorMode == 0u)
        {
            uint param_24 = _659_paramData.ColorData.x;
            color = UnpackColor(param_24);
        }
        else
        {
            if (colorMode == 1u)
            {
                uint param_25 = seed;
                uint2 param_26 = _659_paramData.ColorData.xy;
                float4 _1017 = RandomColorRange(param_25, param_26);
                seed = param_25;
                color = _1017;
            }
            else
            {
                if (colorMode == 2u)
                {
                    uint param_27 = seed;
                    uint2 param_28 = _659_paramData.ColorData.xy;
                    float4 _1031 = RandomColorRange(param_27, param_28);
                    seed = param_27;
                    float4 colorStart = _1031;
                    uint param_29 = seed;
                    uint2 param_30 = _659_paramData.ColorData.zw;
                    float4 _1040 = RandomColorRange(param_29, param_30);
                    seed = param_29;
                    float4 colorEnd = _1040;
                    float param_31 = lifeRatio;
                    float3 param_32 = _659_paramData.ColorEasing;
                    color = lerp(colorStart, colorEnd, EasingSpeed(param_31, param_32).xxxx);
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
        if (((_659_paramData.ColorFlags >> uint(5)) & 1u) != 0u)
        {
            float3 param_33 = color.xyz;
            float3 _1082 = HSV2RGB(param_33);
            color.x = _1082.x;
            color.y = _1082.y;
            color.z = _1082.z;
        }
        uint colorInherit = (_659_paramData.ColorFlags >> uint(3)) & 3u;
        if ((colorInherit == 2u) || (colorInherit == 3u))
        {
            uint param_34 = _592_emitter.Color;
            color *= UnpackColor(param_34);
        }
        else
        {
            uint param_35 = particle.InheritColor;
            color *= UnpackColor(param_35);
        }
        color.w *= clamp(particle.LifeAge / _659_paramData.FadeIn, 0.0f, 1.0f);
        color.w *= clamp((lifeTime - particle.LifeAge) / _659_paramData.FadeOut, 0.0f, 1.0f);
        float3 param_36 = position;
        float3 param_37 = rotation;
        float3 param_38 = (scale.xyz * scale.w) * _659_paramData.ShapeSize;
        particle.Transform = TRSMatrix(param_36, param_37, param_38);
        float4 param_39 = directionSpeed;
        particle.DirectionSpeed = PackFloat4(param_39);
        float4 param_40 = color;
        particle.Color = PackColor(param_40);
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
