#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float4 VColor;
    float2 UV1;
    float2 UV2;
    float3 WorldP;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
    float2 ScreenUV;
    float4 PosP;
};

struct FalloffParameter
{
    float4 Param;
    float4 BeginColor;
    float4 EndColor;
};

struct PS_ConstanBuffer
{
    float4 fLightDirection;
    float4 fLightColor;
    float4 fLightAmbient;
    float4 fFlipbookParameter;
    float4 fUVDistortionParameter;
    float4 fBlendTextureParameter;
    float4 fCameraFrontDirection;
    FalloffParameter fFalloffParam;
    float4 fEmissiveScaling;
    float4 fEdgeColor;
    float4 fEdgeParameter;
    float4 softParticleAndReconstructionParam1;
    float4 reconstructionParam2;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 Input_VColor [[user(locn0), centroid_perspective]];
    float2 Input_UV1 [[user(locn1), centroid_perspective]];
    float2 Input_UV2 [[user(locn2), centroid_perspective]];
    float3 Input_WorldP [[user(locn3)]];
    float3 Input_WorldN [[user(locn4)]];
    float3 Input_WorldT [[user(locn5)]];
    float3 Input_WorldB [[user(locn6)]];
    float2 Input_ScreenUV [[user(locn7)]];
    float4 Input_PosP [[user(locn8)]];
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
float4 _main(PS_Input Input, thread texture2d<float> _normalTex, thread sampler sampler_normalTex, constant PS_ConstanBuffer& v_130, thread texture2d<float> _colorTex, thread sampler sampler_colorTex, thread texture2d<float> _depthTex, thread sampler sampler_depthTex)
{
    float3 loN = _normalTex.sample(sampler_normalTex, Input.UV1).xyz;
    float3 texNormal = (loN - float3(0.5)) * 2.0;
    float3 localNormal = normalize(float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN)) * texNormal);
    float diffuse = fast::max(dot(v_130.fLightDirection.xyz, localNormal), 0.0);
    float4 Output = _colorTex.sample(sampler_colorTex, Input.UV1) * Input.VColor;
    float3 _164 = Output.xyz * ((v_130.fLightColor.xyz * diffuse) + float3(v_130.fLightAmbient.xyz));
    Output = float4(_164.x, _164.y, _164.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
    if ((isunordered(v_130.softParticleAndReconstructionParam1.x, 0.0) || v_130.softParticleAndReconstructionParam1.x != 0.0))
    {
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float param_2 = v_130.softParticleAndReconstructionParam1.x;
        float2 param_3 = v_130.softParticleAndReconstructionParam1.yz;
        float4 param_4 = v_130.reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_130 [[buffer(0)]], texture2d<float> _normalTex [[texture(1)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _depthTex [[texture(2)]], sampler sampler_normalTex [[sampler(1)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_depthTex [[sampler(2)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.VColor = in.Input_VColor;
    Input.UV1 = in.Input_UV1;
    Input.UV2 = in.Input_UV2;
    Input.WorldP = in.Input_WorldP;
    Input.WorldN = in.Input_WorldN;
    Input.WorldT = in.Input_WorldT;
    Input.WorldB = in.Input_WorldB;
    Input.ScreenUV = in.Input_ScreenUV;
    Input.PosP = in.Input_PosP;
    float4 _272 = _main(Input, _normalTex, sampler_normalTex, v_130, _colorTex, sampler_colorTex, _depthTex, sampler_depthTex);
    out._entryPointOutput = _272;
    return out;
}

