#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float4 Color;
    float4 PosP;
};

struct VS_ConstantBuffer
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    float4 softParticleAndReconstructionParam1;
    float4 reconstructionParam2;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float2 Input_UV [[user(locn0), centroid_perspective]];
    float3 Input_Normal [[user(locn1)]];
    float3 Input_Binormal [[user(locn2)]];
    float3 Input_Tangent [[user(locn3)]];
    float4 Input_Color [[user(locn4), centroid_perspective]];
    float4 Input_PosP [[user(locn5)]];
};

static inline __attribute__((always_inline))
float SoftParticle(thread const float& backgroundZ, thread const float& meshZ, thread const float& softparticleParam, thread const float2& reconstruct1, thread const float4& reconstruct2)
{
    float _distance = softparticleParam;
    float2 rescale = reconstruct1;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - float2(params.y)) / (float2(params.x) - (zs * params.z));
    return fast::min(fast::max((depth.y - depth.x) / _distance, 0.0), 1.0);
}

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, thread texture2d<float> g_colorTexture, thread sampler g_colorSampler, thread texture2d<float> g_normalTexture, thread sampler g_normalSampler, constant VS_ConstantBuffer& v_138, thread texture2d<float> g_depthTexture, thread sampler g_depthSampler)
{
    float4 Output = g_colorTexture.sample(g_colorSampler, Input.UV) * Input.Color;
    float3 texNormal = (g_normalTexture.sample(g_normalSampler, Input.UV).xyz - float3(0.5)) * 2.0;
    float3 localNormal = normalize(float3x3(float3(Input.Tangent), float3(Input.Binormal), float3(Input.Normal)) * texNormal);
    float diffuse = fast::max(dot(v_138.fLightDirection.xyz, localNormal), 0.0);
    float3 _158 = Output.xyz * ((v_138.fLightColor.xyz * diffuse) + v_138.fLightAmbient.xyz);
    Output = float4(_158.x, _158.y, _158.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = g_depthTexture.sample(g_depthSampler, screenUV).x;
    if ((isunordered(v_138.softParticleAndReconstructionParam1.x, 0.0) || v_138.softParticleAndReconstructionParam1.x != 0.0))
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = v_138.softParticleAndReconstructionParam1.x;
        float2 param_3 = v_138.softParticleAndReconstructionParam1.yz;
        float4 param_4 = v_138.reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant VS_ConstantBuffer& v_138 [[buffer(0)]], texture2d<float> g_colorTexture [[texture(0)]], texture2d<float> g_normalTexture [[texture(1)]], texture2d<float> g_depthTexture [[texture(2)]], sampler g_colorSampler [[sampler(0)]], sampler g_normalSampler [[sampler(1)]], sampler g_depthSampler [[sampler(2)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = in.Input_UV;
    Input.Normal = in.Input_Normal;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.Color = in.Input_Color;
    Input.PosP = in.Input_PosP;
    float4 _252 = _main(Input, g_colorTexture, g_colorSampler, g_normalTexture, g_normalSampler, v_138, g_depthTexture, g_depthSampler);
    out._entryPointOutput = _252;
    return out;
}

