struct VS_INPUT
{
    uint instanceID;
    float2 a_VertexPosition;
};

struct VS_OUTPUT
{
    float4 Position;
    float4 v_Color;
};

cbuffer CB : register(b0)
{
    float4 _485_ID2TPos : packoffset(c0);
    column_major float4x4 _485_ViewMatrix : packoffset(c1);
    column_major float4x4 _485_ProjMatrix : packoffset(c5);
};

Texture2D<float4> ParticleData0 : register(t0);
SamplerState ParticleData0Sampler : register(s0);
Texture2D<float4> ParticleData1 : register(t1);
SamplerState ParticleData1Sampler : register(s1);
Texture2D<float4> ColorTable : register(t2);
SamplerState ColorTableSampler : register(s2);

static float4 gl_Position;
static int gl_InstanceIndex;
static float2 input_a_VertexPosition;
static float4 _entryPointOutput_v_Color;

struct SPIRV_Cross_Input
{
    float2 input_a_VertexPosition : TEXCOORD0;
    uint gl_InstanceIndex : SV_InstanceID;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput_v_Color : TEXCOORD0;
    float4 gl_Position : SV_Position;
};

float2 rotate(float2 pos, float deg)
{
    float c = cos(deg * 0.01745328865945339202880859375f);
    float s = sin(deg * 0.01745328865945339202880859375f);
    return mul(pos, float2x2(float2(c, -s), float2(s, c)));
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

float fadeIn(float duration, float age, float lifetime)
{
    return clamp(age / duration, 0.0f, 1.0f);
}

float fadeOut(float duration, float age, float lifetime)
{
    return clamp((lifetime - age) / duration, 0.0f, 1.0f);
}

float fadeInOut(float fadeinDuration, float fadeoutDuration, float age, float lifetime)
{
    float param = fadeinDuration;
    float param_1 = age;
    float param_2 = lifetime;
    float param_3 = fadeoutDuration;
    float param_4 = age;
    float param_5 = lifetime;
    return fadeIn(param, param_1, param_2) * fadeOut(param_3, param_4, param_5);
}

VS_OUTPUT _main(VS_INPUT _input)
{
    int particleID = int(_input.instanceID);
    int2 ID2TPos2i = int2(int(_485_ID2TPos.x), int(_485_ID2TPos.y));
    int2 texPos = int2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
    float4 data0 = ParticleData0.SampleLevel(ParticleData0Sampler, float2(float4(float2(texPos), 0.0f, 0.0f).xy), 0.0f);
    float4 data1 = ParticleData1.SampleLevel(ParticleData1Sampler, float2(float4(float2(texPos), 0.0f, 0.0f).xy), 0.0f);
    float age = data1.x;
    float lifetime = data1.y;
    VS_OUTPUT _output;
    if (age >= lifetime)
    {
        _output.Position = 0.0f.xxxx;
        _output.v_Color = 0.0f.xxxx;
    }
    else
    {
        float3 position = data0.xyz;
        float2 param = _input.a_VertexPosition * 0.0030000000260770320892333984375f;
        float param_1 = 45.0f;
        position += float3(rotate(param, param_1), 0.0f);
        _output.Position = mul(float4(position, 1.0f), mul(_485_ViewMatrix, _485_ProjMatrix));
        float3 param_2 = float3(float2(texPos), 0.0f) / 512.0f.xxx;
        float2 texCoord = snoise(param_2).xx;
        _output.v_Color = ColorTable.SampleLevel(ColorTableSampler, float2(float4(texCoord, 0.0f, 0.0f).xy), 0.0f);
        float param_3 = 1.0f;
        float param_4 = 10.0f;
        float param_5 = age;
        float param_6 = lifetime;
        _output.v_Color.w *= fadeInOut(param_3, param_4, param_5, param_6);
    }
    return _output;
}

void vert_main()
{
    VS_INPUT _input;
    _input.instanceID = uint(gl_InstanceIndex);
    _input.a_VertexPosition = input_a_VertexPosition;
    VS_INPUT param = _input;
    VS_OUTPUT flattenTemp = _main(param);
    gl_Position = flattenTemp.Position;
    _entryPointOutput_v_Color = flattenTemp.v_Color;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_InstanceIndex = int(stage_input.gl_InstanceIndex);
    input_a_VertexPosition = stage_input.input_a_VertexPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.gl_Position = gl_Position;
    stage_output._entryPointOutput_v_Color = _entryPointOutput_v_Color;
    return stage_output;
}
