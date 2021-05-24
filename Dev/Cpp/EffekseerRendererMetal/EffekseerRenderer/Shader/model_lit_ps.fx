#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
    float4 PosP;
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
    float4 fFalloffParameter;
    float4 fFalloffBeginColor;
    float4 fFalloffEndColor;
    float4 fEmissiveScaling;
    float4 fEdgeColor;
    float4 fEdgeParameter;
    float4 softParticleParam;
    float4 reconstructionParam1;
    float4 reconstructionParam2;
    float4 mUVInversedBack;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 Input_Color [[user(locn0), centroid_perspective]];
    float2 Input_UV [[user(locn1), centroid_perspective]];
    float3 Input_WorldN [[user(locn2)]];
    float3 Input_WorldB [[user(locn3)]];
    float3 Input_WorldT [[user(locn4)]];
    float4 Input_PosP [[user(locn5)]];
};

static inline __attribute__((always_inline))
float SoftParticle(thread const float& backgroundZ, thread const float& meshZ, thread const float4& softparticleParam, thread const float4& reconstruct1, thread const float4& reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    float2 rescale = reconstruct1.xy;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - float2(params.y)) / (float2(params.x) - (zs * params.z));
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return fast::min(fast::max(fast::min(alphaFar, alphaNear), 0.0), 1.0);
}

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, thread texture2d<float> _colorTex, thread sampler sampler_colorTex, thread texture2d<float> _normalTex, thread sampler sampler_normalTex, constant PS_ConstanBuffer& v_164, thread texture2d<float> _depthTex, thread sampler sampler_depthTex)
{
    float4 Output = _colorTex.sample(sampler_colorTex, Input.UV) * Input.Color;
    float3 texNormal = (_normalTex.sample(sampler_normalTex, Input.UV).xyz - float3(0.5)) * 2.0;
    float3 localNormal = normalize(float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN)) * texNormal);
    float diffuse = fast::max(dot(v_164.fLightDirection.xyz, localNormal), 0.0);
    float3 _184 = Output.xyz * ((v_164.fLightColor.xyz * diffuse) + v_164.fLightAmbient.xyz);
    Output = float4(_184.x, _184.y, _184.z, Output.w);
    float3 _193 = Output.xyz * v_164.fEmissiveScaling.x;
    Output = float4(_193.x, _193.y, _193.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = v_164.mUVInversedBack.x + (v_164.mUVInversedBack.y * screenUV.y);
    if ((isunordered(v_164.softParticleParam.w, 0.0) || v_164.softParticleParam.w != 0.0))
    {
        float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float4 param_2 = v_164.softParticleParam;
        float4 param_3 = v_164.reconstructionParam1;
        float4 param_4 = v_164.reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_164 [[buffer(0)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _normalTex [[texture(1)]], texture2d<float> _depthTex [[texture(2)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_normalTex [[sampler(1)]], sampler sampler_depthTex [[sampler(2)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    Input.WorldN = in.Input_WorldN;
    Input.WorldB = in.Input_WorldB;
    Input.WorldT = in.Input_WorldT;
    Input.PosP = in.Input_PosP;
    float4 _298 = _main(Input, _colorTex, sampler_colorTex, _normalTex, sampler_normalTex, v_164, _depthTex, sampler_depthTex);
    out._entryPointOutput = _298;
    return out;
}

