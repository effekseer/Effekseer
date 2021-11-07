struct PS_INPUT
{
    float2 ScreenUV;
};

struct VS_OUTPUT
{
    float4 o_ParticleData0;
    float4 o_ParticleData1;
};

cbuffer CB : register(b0)
{
    float4 _515_DeltaTime : packoffset(c0);
};

Texture2D<float4> i_ParticleData0 : register(t0);
SamplerState i_ParticleData0Sampler : register(s0);
Texture2D<float4> i_ParticleData1 : register(t1);
SamplerState i_ParticleData1Sampler : register(s1);

static float2 input_ScreenUV;
static float4 _entryPointOutput_o_ParticleData0;
static float4 _entryPointOutput_o_ParticleData1;

struct SPIRV_Cross_Input
{
    float2 input_ScreenUV : TEXCOORD0;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput_o_ParticleData0 : TEXCOORD0;
    float4 _entryPointOutput_o_ParticleData1 : TEXCOORD1;
};

float3 unpackVec3(float s)
{
    uint bits = asuint(s);
    float3 v = float3(uint3(bits, bits >> uint(10), bits >> uint(20)) & uint3(1023u, 1023u, 1023u));
    return ((v / 1023.0f.xxx) * 2.0f) - 1.0f.xxx;
}

float3 mod289(float3 x)
{
    return x - (floor(x * 0.00346020772121846675872802734375f) * 289.0f);
}

float4 mod289(float4 x)
{
    return x - (floor(x * 0.00346020772121846675872802734375f) * 289.0f);
}

float4 permute(float4 x)
{
    float4 param = ((x * 34.0f) + 1.0f.xxxx) * x;
    return mod289(param);
}

float4 taylorInvSqrt(float4 r)
{
    return 1.792842864990234375f.xxxx - (r * 0.8537347316741943359375f);
}

float snoise(float3 v)
{
    float3 i = floor(v + dot(v, 0.3333333432674407958984375f.xxx).xxx);
    float3 x0 = (v - i) + dot(i, 0.16666667163372039794921875f.xxx).xxx;
    float3 g = step(x0.yzx, x0);
    float3 l = 1.0f.xxx - g;
    float3 i1 = min(g, l.zxy);
    float3 i2 = max(g, l.zxy);
    float3 x1 = (x0 - i1) + 0.16666667163372039794921875f.xxx;
    float3 x2 = (x0 - i2) + 0.3333333432674407958984375f.xxx;
    float3 x3 = x0 - 0.5f.xxx;
    float3 param = i;
    i = mod289(param);
    float4 param_1 = i.z.xxxx + float4(0.0f, i1.z, i2.z, 1.0f);
    float4 param_2 = (permute(param_1) + i.y.xxxx) + float4(0.0f, i1.y, i2.y, 1.0f);
    float4 param_3 = (permute(param_2) + i.x.xxxx) + float4(0.0f, i1.x, i2.x, 1.0f);
    float4 p = permute(param_3);
    float n_ = 0.14285714924335479736328125f;
    float3 ns = (float3(2.0f, 0.5f, 1.0f) * n_) - float3(0.0f, 1.0f, 0.0f);
    float4 j = p - (floor((p * ns.z) * ns.z) * 49.0f);
    float4 x_ = floor(j * ns.z);
    float4 y_ = floor(j - (x_ * 7.0f));
    float4 x = (x_ * ns.x) + ns.yyyy;
    float4 y = (y_ * ns.x) + ns.yyyy;
    float4 h = (1.0f.xxxx - abs(x)) - abs(y);
    float4 b0 = float4(x.xy, y.xy);
    float4 b1 = float4(x.zw, y.zw);
    float4 s0 = (floor(b0) * 2.0f) + 1.0f.xxxx;
    float4 s1 = (floor(b1) * 2.0f) + 1.0f.xxxx;
    float4 sh = -step(h, 0.0f.xxxx);
    float4 a0 = b0.xzyw + (s0.xzyw * sh.xxyy);
    float4 a1 = b1.xzyw + (s1.xzyw * sh.zzww);
    float3 p0 = float3(a0.xy, h.x);
    float3 p1 = float3(a0.zw, h.y);
    float3 p2 = float3(a1.xy, h.z);
    float3 p3 = float3(a1.zw, h.w);
    float4 param_4 = float4(dot(p0, p0), dot(p1, p1), dot(p2, p2), dot(p3, p3));
    float4 norm = taylorInvSqrt(param_4);
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;
    float4 m = max(0.60000002384185791015625f.xxxx - float4(dot(x0, x0), dot(x1, x1), dot(x2, x2), dot(x3, x3)), 0.0f.xxxx);
    m *= m;
    return 42.0f * dot(m * m, float4(dot(p0, x0), dot(p1, x1), dot(p2, x2), dot(p3, x3)));
}

float3 noise3(float3 seed)
{
    float3 param = seed;
    float3 param_1 = seed.yzx;
    float3 param_2 = seed.zxy;
    return float3(snoise(param), snoise(param_1), snoise(param_2));
}

float packVec3(float3 v)
{
    uint3 i = uint3(((v + 1.0f.xxx) * 0.5f) * 1023.0f);
    return asfloat((i.x | (i.y << uint(10))) | (i.z << uint(20)));
}

VS_OUTPUT _main(PS_INPUT _input)
{
    float4 data0 = i_ParticleData0.SampleLevel(i_ParticleData0Sampler, float2(float4(_input.ScreenUV, 0.0f, 0.0f).xy), 0.0f);
    float4 data1 = i_ParticleData1.SampleLevel(i_ParticleData1Sampler, float2(float4(_input.ScreenUV, 0.0f, 0.0f).xy), 0.0f);
    float3 position = data0.xyz;
    float param = data1.w;
    float3 direction = unpackVec3(param);
    float DeltaTime1f = _515_DeltaTime.x;
    data1.x += DeltaTime1f;
    float lifetimeRatio = data1.x / data1.y;
    float3 velocity = 0.0f.xxx;
    velocity += (direction * lerp(0.00999999977648258209228515625f, 0.0f, lifetimeRatio));
    float3 param_1 = position;
    velocity += (noise3(param_1) * 0.00999999977648258209228515625f);
    float3 targetPosition = float3(0.20000000298023223876953125f, 0.0f, 0.0f);
    float3 diff = targetPosition - position;
    velocity += (normalize(diff) * 0.00999999977648258209228515625f);
    position += velocity;
    float3 _567 = normalize(velocity);
    bool3 _569 = (length(velocity) < 9.9999997473787516355514526367188e-05f).xxx;
    direction = float3(_569.x ? direction.x : _567.x, _569.y ? direction.y : _567.y, _569.z ? direction.z : _567.z);
    float3 param_2 = direction;
    VS_OUTPUT _output;
    _output.o_ParticleData0 = float4(position, packVec3(param_2));
    _output.o_ParticleData1 = data1;
    return _output;
}

void vert_main()
{
    PS_INPUT _input;
    _input.ScreenUV = input_ScreenUV;
    PS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    _entryPointOutput_o_ParticleData0 = flattenTemp.o_ParticleData0;
    _entryPointOutput_o_ParticleData1 = flattenTemp.o_ParticleData1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    input_ScreenUV = stage_input.input_ScreenUV;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput_o_ParticleData0 = _entryPointOutput_o_ParticleData0;
    stage_output._entryPointOutput_o_ParticleData1 = _entryPointOutput_o_ParticleData1;
    return stage_output;
}
