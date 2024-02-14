#pragma clang diagnostic ignored "-Wmissing-prototypes"
#pragma clang diagnostic ignored "-Wmissing-braces"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

template<typename T, size_t Num>
struct spvUnsafeArray
{
    T elements[Num ? Num : 1];
    
    thread T& operator [] (size_t pos) thread
    {
        return elements[pos];
    }
    constexpr const thread T& operator [] (size_t pos) const thread
    {
        return elements[pos];
    }
    
    device T& operator [] (size_t pos) device
    {
        return elements[pos];
    }
    constexpr const device T& operator [] (size_t pos) const device
    {
        return elements[pos];
    }
    
    constexpr const constant T& operator [] (size_t pos) const constant
    {
        return elements[pos];
    }
    
    threadgroup T& operator [] (size_t pos) threadgroup
    {
        return elements[pos];
    }
    constexpr const threadgroup T& operator [] (size_t pos) const threadgroup
    {
        return elements[pos];
    }
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
    uint _437 = RandomUint(param);
    seed = param;
    return float(_437) / 4294967296.0;
}

static inline __attribute__((always_inline))
float RandomFloatRange(thread uint& seed, thread const float2& maxmin)
{
    uint param = seed;
    float _450 = RandomFloat(param);
    seed = param;
    return mix(maxmin.y, maxmin.x, _450);
}

static inline __attribute__((always_inline))
float4 RandomFloat4Range(thread uint& seed, thread const spvUnsafeArray<float4, 2>& maxmin)
{
    uint param = seed;
    float _461 = RandomFloat(param);
    seed = param;
    return mix(maxmin[1], maxmin[0], float4(_461));
}

static inline __attribute__((always_inline))
float4 UnpackFloat4(thread const uint2& bits)
{
    return float4(float2(as_type<half2>(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).x)).x, float2(as_type<half2>(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).y)).x, float2(as_type<half2>(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).z)).x, float2(as_type<half2>(uint4(bits.x, bits.x >> uint(16), bits.y, bits.y >> uint(16)).w)).x);
}

static inline __attribute__((always_inline))
uint PackNormalizedFloat3(thread const float3& v)
{
    uint3 i = uint3(((v + float3(1.0)) * 0.5) * 1023.0);
    return (i.x | (i.y << uint(10))) | (i.z << uint(20));
}

static inline __attribute__((always_inline))
float3 Vortex(thread const float& rotation, thread const float& attraction, thread float3& center, thread float3& axis, thread const float3& position, thread const float4x3& transform)
{
    center = transform[3] + center;
    axis = fast::normalize(transform * float4(axis, 0.0));
    float3 localPos = position - center;
    float3 axisToPos = localPos - (axis * dot(axis, localPos));
    float _distance = length(axisToPos);
    if (_distance < 9.9999997473787516355514526367188e-05)
    {
        return float3(0.0);
    }
    float3 radial = fast::normalize(axisToPos);
    float3 tangent = cross(axis, radial);
    return (tangent * rotation) - (radial * attraction);
}

static inline __attribute__((always_inline))
float EasingSpeed(thread const float& t, thread const float3& params)
{
    return ((((params.x * t) * t) * t) + ((params.y * t) * t)) + (params.z * t);
}

static inline __attribute__((always_inline))
float4 UnpackColor(thread const uint& color32)
{
    return float4(float(color32 & 255u), float((color32 >> uint(8)) & 255u), float((color32 >> uint(16)) & 255u), float((color32 >> uint(24)) & 255u)) / float4(255.0);
}

static inline __attribute__((always_inline))
float4 RandomColorRange(thread uint& seed, thread const uint2& maxmin)
{
    uint param = maxmin.y;
    uint param_1 = maxmin.x;
    uint param_2 = seed;
    float _477 = RandomFloat(param_2);
    seed = param_2;
    return mix(UnpackColor(param), UnpackColor(param_1), float4(_477));
}

static inline __attribute__((always_inline))
float3 HSV2RGB(thread const float3& c)
{
    float4 k = float4(1.0, 0.666666686534881591796875, 0.3333333432674407958984375, 3.0);
    float3 p = abs((fract(c.xxx + k.xyz) * 6.0) - k.www);
    return mix(k.xxx, fast::clamp(p - k.xxx, float3(0.0), float3(1.0)), float3(c.y)) * c.z;
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
uint PackColor(thread const float4& color)
{
    uint4 colori = uint4(fast::clamp(color * 255.0, float4(0.0), float4(255.0)));
    return ((colori.x | (colori.y << uint(8))) | (colori.z << uint(16))) | (colori.w << uint(24));
}

static inline __attribute__((always_inline))
void _main(thread const uint3& dtid, constant cb1& _596, device Particles& Particles_1, constant cb0& _663, device Trails& Trails_1, texture3d<float> NoiseTex, sampler NoiseSamp, texture2d<float> GradientTex, sampler GradientSamp)
{
    uint particleID = _596.emitter.ParticleHead + dtid.x;
    ParticleData particle;
    particle.FlagBits = Particles_1._data[particleID].FlagBits;
    particle.Seed = Particles_1._data[particleID].Seed;
    particle.LifeAge = Particles_1._data[particleID].LifeAge;
    particle.InheritColor = Particles_1._data[particleID].InheritColor;
    particle.DirectionSpeed = Particles_1._data[particleID].DirectionSpeed;
    particle.Color = Particles_1._data[particleID].Color;
    particle.Padding = Particles_1._data[particleID].Padding;
    particle.Transform = transpose(Particles_1._data[particleID].Transform);
    if ((particle.FlagBits & 1u) != 0u)
    {
        uint updateCount = (particle.FlagBits >> uint(1)) & 255u;
        float deltaTime = _596.emitter.DeltaTime;
        uint seed = particle.Seed;
        uint param = seed;
        float2 param_1 = _663.paramData.LifeTime;
        float _670 = RandomFloatRange(param, param_1);
        seed = param;
        float lifeTime = _670;
        float lifeRatio = particle.LifeAge / lifeTime;
        uint param_2 = seed;
        float2 param_3 = _663.paramData.Damping;
        float _684 = RandomFloatRange(param_2, param_3);
        seed = param_2;
        float damping = _684 * 0.00999999977648258209228515625;
        uint param_4 = seed;
        spvUnsafeArray<float4, 2> param_5;
        param_5[0] = _663.paramData.AngularOffset[0];
        param_5[1] = _663.paramData.AngularOffset[1];
        float4 _700 = RandomFloat4Range(param_4, param_5);
        seed = param_4;
        float4 angularOffset = _700;
        uint param_6 = seed;
        spvUnsafeArray<float4, 2> param_7;
        param_7[0] = _663.paramData.AngularVelocity[0];
        param_7[1] = _663.paramData.AngularVelocity[1];
        float4 _714 = RandomFloat4Range(param_6, param_7);
        seed = param_6;
        float4 angularVelocity = _714;
        float3 position = particle.Transform[3];
        uint2 param_8 = particle.DirectionSpeed;
        float4 directionSpeed = UnpackFloat4(param_8);
        float3 velocity = directionSpeed.xyz * directionSpeed.w;
        if (_596.emitter.TrailSize > 0u)
        {
            uint trailID = (_596.emitter.TrailHead + (dtid.x * _663.paramData.ShapeData)) + _596.emitter.TrailPhase;
            TrailData trail;
            trail.Position = position;
            float3 param_9 = directionSpeed.xyz;
            trail.Direction = PackNormalizedFloat3(param_9);
            Trails_1._data[trailID].Position = trail.Position;
            Trails_1._data[trailID].Direction = trail.Direction;
        }
        particle.FlagBits &= 4294966785u;
        particle.FlagBits |= (clamp(updateCount + 1u, 0u, 255u) << uint(1));
        particle.LifeAge += deltaTime;
        if (particle.LifeAge >= lifeTime)
        {
            particle.FlagBits &= 4294967294u;
        }
        velocity += (float3(_663.paramData.Gravity) * deltaTime);
        if ((_663.paramData.VortexRotation != 0.0) || (_663.paramData.VortexAttraction != 0.0))
        {
            float param_10 = _663.paramData.VortexRotation;
            float param_11 = _663.paramData.VortexAttraction;
            float3 param_12 = float3(_663.paramData.VortexCenter);
            float3 param_13 = float3(_663.paramData.VortexAxis);
            float3 param_14 = position;
            float4x3 param_15 = transpose(_596.emitter.Transform);
            float3 _838 = Vortex(param_10, param_11, param_12, param_13, param_14, param_15);
            velocity += (_838 * deltaTime);
        }
        if (_663.paramData.TurbulencePower != 0.0)
        {
            float4 vfTexel = NoiseTex.sample(NoiseSamp, ((position * _663.paramData.TurbulenceScale) + float3(0.5)), level(0.0));
            velocity += ((((vfTexel.xyz * 2.0) - float3(1.0)) * _663.paramData.TurbulencePower) * deltaTime);
        }
        float speed = length(velocity);
        if (speed > 0.0)
        {
            float newSpeed = fast::max(0.0, speed - (damping * deltaTime));
            velocity *= (newSpeed / speed);
        }
        position += (velocity * deltaTime);
        directionSpeed.w = length(velocity);
        if (directionSpeed.w > 9.9999997473787516355514526367188e-05)
        {
            float3 _913 = fast::normalize(velocity);
            directionSpeed.x = _913.x;
            directionSpeed.y = _913.y;
            directionSpeed.z = _913.z;
        }
        float3 rotation = angularOffset.xyz + (angularVelocity.xyz * particle.LifeAge);
        float4 scale = float4(1.0);
        uint scaleMode = _663.paramData.ScaleFlags & 7u;
        if (scaleMode == 0u)
        {
            uint param_16 = seed;
            spvUnsafeArray<float4, 2> param_17;
            param_17[0] = _663.paramData.ScaleData1[0];
            param_17[1] = _663.paramData.ScaleData1[1];
            float4 _951 = RandomFloat4Range(param_16, param_17);
            seed = param_16;
            scale = _951;
        }
        else
        {
            if (scaleMode == 2u)
            {
                uint param_18 = seed;
                spvUnsafeArray<float4, 2> param_19;
                param_19[0] = _663.paramData.ScaleData1[0];
                param_19[1] = _663.paramData.ScaleData1[1];
                float4 _968 = RandomFloat4Range(param_18, param_19);
                seed = param_18;
                float4 scale1 = _968;
                uint param_20 = seed;
                spvUnsafeArray<float4, 2> param_21;
                param_21[0] = _663.paramData.ScaleData2[0];
                param_21[1] = _663.paramData.ScaleData2[1];
                float4 _982 = RandomFloat4Range(param_20, param_21);
                seed = param_20;
                float4 scale2 = _982;
                float param_22 = lifeRatio;
                float3 param_23 = float3(_663.paramData.ScaleEasing);
                scale = mix(scale1, scale2, float4(EasingSpeed(param_22, param_23)));
            }
        }
        uint colorMode = _663.paramData.ColorFlags & 7u;
        float4 color = float4(1.0);
        if (colorMode == 0u)
        {
            uint param_24 = _663.paramData.ColorData.x;
            color = UnpackColor(param_24);
        }
        else
        {
            if (colorMode == 1u)
            {
                uint param_25 = seed;
                uint2 param_26 = _663.paramData.ColorData.xy;
                float4 _1021 = RandomColorRange(param_25, param_26);
                seed = param_25;
                color = _1021;
            }
            else
            {
                if (colorMode == 2u)
                {
                    uint param_27 = seed;
                    uint2 param_28 = _663.paramData.ColorData.xy;
                    float4 _1035 = RandomColorRange(param_27, param_28);
                    seed = param_27;
                    float4 colorStart = _1035;
                    uint param_29 = seed;
                    uint2 param_30 = _663.paramData.ColorData.zw;
                    float4 _1044 = RandomColorRange(param_29, param_30);
                    seed = param_29;
                    float4 colorEnd = _1044;
                    float param_31 = lifeRatio;
                    float3 param_32 = float3(_663.paramData.ColorEasing);
                    color = mix(colorStart, colorEnd, float4(EasingSpeed(param_31, param_32)));
                }
                else
                {
                    if ((colorMode == 3u) || (colorMode == 4u))
                    {
                        color = GradientTex.sample(GradientSamp, float2(lifeRatio, 0.0), level(0.0));
                    }
                }
            }
        }
        if (((_663.paramData.ColorFlags >> uint(5)) & 1u) != 0u)
        {
            float3 param_33 = color.xyz;
            float3 _1086 = HSV2RGB(param_33);
            color.x = _1086.x;
            color.y = _1086.y;
            color.z = _1086.z;
        }
        uint colorInherit = (_663.paramData.ColorFlags >> uint(3)) & 3u;
        if ((colorInherit == 2u) || (colorInherit == 3u))
        {
            uint param_34 = _596.emitter.Color;
            color *= UnpackColor(param_34);
        }
        else
        {
            uint param_35 = particle.InheritColor;
            color *= UnpackColor(param_35);
        }
        color.w *= fast::clamp(particle.LifeAge / _663.paramData.FadeIn, 0.0, 1.0);
        color.w *= fast::clamp((lifeTime - particle.LifeAge) / _663.paramData.FadeOut, 0.0, 1.0);
        float3 param_36 = position;
        float3 param_37 = rotation;
        float3 param_38 = (scale.xyz * scale.w) * _663.paramData.ShapeSize;
        particle.Transform = TRSMatrix(param_36, param_37, param_38);
        float4 param_39 = directionSpeed;
        particle.DirectionSpeed = PackFloat4(param_39);
        float4 param_40 = color;
        particle.Color = PackColor(param_40);
        Particles_1._data[particleID].FlagBits = particle.FlagBits;
        Particles_1._data[particleID].Seed = particle.Seed;
        Particles_1._data[particleID].LifeAge = particle.LifeAge;
        Particles_1._data[particleID].InheritColor = particle.InheritColor;
        Particles_1._data[particleID].DirectionSpeed = particle.DirectionSpeed;
        Particles_1._data[particleID].Color = particle.Color;
        Particles_1._data[particleID].Padding = particle.Padding;
        Particles_1._data[particleID].Transform = transpose(particle.Transform);
    }
}

kernel void main0(constant cb0& _663 [[buffer(0)]], constant cb1& _596 [[buffer(1)]], device Particles& Particles_1 [[buffer(10)]], device Trails& Trails_1 [[buffer(11)]], texture3d<float> NoiseTex [[texture(2)]], texture2d<float> GradientTex [[texture(4)]], sampler NoiseSamp [[sampler(2)]], sampler GradientSamp [[sampler(4)]], uint3 gl_GlobalInvocationID [[thread_position_in_grid]])
{
    uint3 dtid = gl_GlobalInvocationID;
    uint3 param = dtid;
    _main(param, _596, Particles_1, _663, Trails_1, NoiseTex, NoiseSamp, GradientTex, GradientSamp);
}

