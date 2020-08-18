static const char metal_sprite_lit_ps[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 Position;
    float4 VColor;
    float2 UV1;
    float2 UV2;
    float3 WorldP;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
    float2 ScreenUV;
};

struct VS_ConstantBuffer
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    float4 fFlipbookParameter;
    float4 fUVDistortionParameter;
    float4 fBlendTextureParameter;
    float4 fEmissiveScaling;
    float4 fEdgeColor;
    float4 fEdgeParameter;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 Input_VColor [[user(locn0)]];
    float2 Input_UV1 [[user(locn1)]];
    float2 Input_UV2 [[user(locn2)]];
    float3 Input_WorldP [[user(locn3)]];
    float3 Input_WorldN [[user(locn4)]];
    float3 Input_WorldT [[user(locn5)]];
    float3 Input_WorldB [[user(locn6)]];
    float2 Input_ScreenUV [[user(locn7)]];
};

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, thread texture2d<float> g_normalTexture, thread sampler g_normalSampler, constant VS_ConstantBuffer& v_69, thread texture2d<float> g_colorTexture, thread sampler g_colorSampler)
{
    float3 loN = g_normalTexture.sample(g_normalSampler, Input.UV1).xyz;
    float3 texNormal = (loN - float3(0.5)) * 2.0;
    float3 localNormal = normalize(float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN)) * texNormal);
    float diffuse = fast::max(dot(v_69.fLightDirection.xyz, localNormal), 0.0);
    float4 Output = g_colorTexture.sample(g_colorSampler, Input.UV1) * Input.VColor;
    float3 _104 = Output.xyz * ((v_69.fLightColor.xyz * diffuse) + float3(v_69.fLightAmbient.xyz));
    Output = float4(_104.x, _104.y, _104.z, Output.w);
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_69 [[buffer(0)]], texture2d<float> g_normalTexture [[texture(1)]], texture2d<float> g_colorTexture [[texture(0)]], sampler g_normalSampler [[sampler(1)]], sampler g_colorSampler [[sampler(0)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.VColor = in.Input_VColor;
    Input.UV1 = in.Input_UV1;
    Input.UV2 = in.Input_UV2;
    Input.WorldP = in.Input_WorldP;
    Input.WorldN = in.Input_WorldN;
    Input.WorldT = in.Input_WorldT;
    Input.WorldB = in.Input_WorldB;
    Input.ScreenUV = in.Input_ScreenUV;
    float4 _158 = _main(Input, g_normalTexture, g_normalSampler, v_69, g_colorTexture, g_colorSampler);
    out._entryPointOutput = _158;
    return out;
}

)";

