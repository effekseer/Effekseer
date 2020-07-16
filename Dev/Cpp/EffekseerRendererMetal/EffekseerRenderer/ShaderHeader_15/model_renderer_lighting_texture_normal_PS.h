static const char metal_model_renderer_lighting_texture_normal_PS[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 Pos;
    float2 UV;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float4 Color;
};

struct VS_ConstantBuffer
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float2 Input_UV [[user(locn0)]];
    float3 Input_Normal [[user(locn1)]];
    float3 Input_Binormal [[user(locn2)]];
    float3 Input_Tangent [[user(locn3)]];
    float4 Input_Color [[user(locn4)]];
};

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, thread texture2d<float> g_colorTexture, thread sampler g_colorSampler, thread texture2d<float> g_normalTexture, thread sampler g_normalSampler, constant VS_ConstantBuffer& v_79)
{
    float4 Output = g_colorTexture.sample(g_colorSampler, Input.UV) * Input.Color;
    float3 texNormal = (g_normalTexture.sample(g_normalSampler, Input.UV).xyz - float3(0.5)) * 2.0;
    float3 localNormal = normalize(float3x3(float3(Input.Tangent), float3(Input.Binormal), float3(Input.Normal)) * texNormal);
    float diffuse = fast::max(dot(v_79.fLightDirection.xyz, localNormal), 0.0);
    float3 _99 = Output.xyz * ((v_79.fLightColor.xyz * diffuse) + v_79.fLightAmbient.xyz);
    Output = float4(_99.x, _99.y, _99.z, Output.w);
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_79 [[buffer(0)]], texture2d<float> g_colorTexture [[texture(0)]], texture2d<float> g_normalTexture [[texture(1)]], sampler g_colorSampler [[sampler(0)]], sampler g_normalSampler [[sampler(1)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = in.Input_UV;
    Input.Normal = in.Input_Normal;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.Color = in.Input_Color;
    float4 _141 = _main(Input, g_colorTexture, g_colorSampler, g_normalTexture, g_normalSampler, v_79);
    out._entryPointOutput = _141;
    return out;
}

)";

