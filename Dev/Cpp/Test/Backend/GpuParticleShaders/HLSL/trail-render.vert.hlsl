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
    float4 _326_ID2TPos : packoffset(c0);
    column_major float4x4 _326_ViewMatrix : packoffset(c1);
    column_major float4x4 _326_ProjMatrix : packoffset(c5);
    float4 _326_Trail : packoffset(c9);
};

Texture2D<float4> ParticleData0 : register(t0);
SamplerState ParticleData0Sampler : register(s0);
Texture2D<float4> ParticleData1 : register(t1);
SamplerState ParticleData1Sampler : register(s1);
Texture2DArray<float4> Histories : register(t3);
SamplerState HistoriesSampler : register(s3);
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

float3 unpackVec3(float s)
{
    uint bits = asuint(s);
    float3 v = float3(uint3(bits, bits >> uint(10), bits >> uint(20)) & uint3(1023u, 1023u, 1023u));
    return ((v / 1023.0f.xxx) * 2.0f) - 1.0f.xxx;
}

float2 mod289(float2 x)
{
    return x - (floor(x * 0.00346020772121846675872802734375f) * 289.0f);
}

float3 mod289(float3 x)
{
    return x - (floor(x * 0.00346020772121846675872802734375f) * 289.0f);
}

float3 permute(float3 x)
{
    float3 param = ((x * 34.0f) + 1.0f.xxx) * x;
    return mod289(param);
}

float snoise(float2 v)
{
    float2 i = floor(v + dot(v, 0.3660254180431365966796875f.xx).xx);
    float2 x0 = (v - i) + dot(i, 0.211324870586395263671875f.xx).xx;
    bool2 _149 = (x0.x > x0.y).xx;
    float2 i1 = float2(_149.x ? float2(1.0f, 0.0f).x : float2(0.0f, 1.0f).x, _149.y ? float2(1.0f, 0.0f).y : float2(0.0f, 1.0f).y);
    float4 x12 = x0.xyxy + float4(0.211324870586395263671875f, 0.211324870586395263671875f, -0.57735025882720947265625f, -0.57735025882720947265625f);
    float2 _161 = x12.xy - i1;
    x12 = float4(_161.x, _161.y, x12.z, x12.w);
    float2 param = i;
    i = mod289(param);
    float3 param_1 = i.y.xxx + float3(0.0f, i1.y, 1.0f);
    float3 param_2 = (permute(param_1) + i.x.xxx) + float3(0.0f, i1.x, 1.0f);
    float3 p = permute(param_2);
    float3 m = max(0.5f.xxx - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0f.xxx);
    m *= m;
    m *= m;
    float3 x = (frac(p * 0.024390242993831634521484375f.xxx) * 2.0f) - 1.0f.xxx;
    float3 h = abs(x) - 0.5f.xxx;
    float3 ox = floor(x + 0.5f.xxx);
    float3 a0 = x - ox;
    m *= (1.792842864990234375f.xxx - (((a0 * a0) + (h * h)) * 0.8537347316741943359375f));
    float3 g;
    g.x = (a0.x * x0.x) + (h.x * x0.y);
    float2 _273 = (a0.yz * x12.xz) + (h.yz * x12.yw);
    g = float3(g.x, _273.x, _273.y);
    return 130.0f * dot(m, g);
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
    int2 ID2TPos2i = int2(int(_326_ID2TPos.x), int(_326_ID2TPos.y));
    int2 texPos = int2(particleID & ID2TPos2i.x, particleID >> ID2TPos2i.y);
    float4 data0 = ParticleData0.SampleLevel(ParticleData0Sampler, float2(float4(float2(texPos), 0.0f, 0.0f).xy), 0.0f);
    float4 data1 = ParticleData1.SampleLevel(ParticleData1Sampler, float2(float4(float2(texPos), 0.0f, 0.0f).xy), 0.0f);
    float age = data1.x;
    float lifetime = data1.y;
    VS_OUTPUT _output;
    if ((age >= lifetime) || (age <= 0.0f))
    {
        _output.Position = 0.0f.xxxx;
        _output.v_Color = 0.0f.xxxx;
    }
    else
    {
        float historyID = _input.a_VertexPosition.x * min(_326_Trail.y, age);
        float3 position;
        float3 direction;
        if (historyID >= 1.0f)
        {
            int texIndex = ((int(_326_Trail.x) + int(historyID)) - 1) % int(_326_Trail.y);
            float4 trailData = Histories.SampleLevel(HistoriesSampler, float3(float4(float2(texPos), float(texIndex), 0.0f).xyz), 0.0f);
            position = trailData.xyz;
            float param = trailData.w;
            direction = unpackVec3(param);
        }
        else
        {
            position = data0.xyz;
            float param_1 = data0.w;
            direction = unpackVec3(param_1);
        }
        float width = 0.0500000007450580596923828125f;
        float3 vertex = (cross(float3(0.0f, 1.0f, 0.0f), direction) * _input.a_VertexPosition.y) * width;
        _output.Position = mul(mul(float4(position + vertex, 1.0f), _326_ViewMatrix), _326_ProjMatrix);
        float2 param_2 = float2(texPos) / 512.0f.xx;
        float2 texCoord = snoise(param_2).xx;
        _output.v_Color = ColorTable.SampleLevel(ColorTableSampler, float2(float4(texCoord, 0.0f, 0.0f).xy), 0.0f);
        float param_3 = 10.0f;
        float param_4 = 10.0f;
        float param_5 = age;
        float param_6 = lifetime;
        _output.v_Color.w *= (0.5f * fadeInOut(param_3, param_4, param_5, param_6));
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
