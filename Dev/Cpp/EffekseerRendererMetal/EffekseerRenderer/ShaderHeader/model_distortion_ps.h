static const char metal_model_distortion_ps[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
};

struct PS_ConstantBuffer
{
    float4 g_scale;
    float4 mUVInversedBack;
    float4 fFlipbookParameter;
    float4 fUVDistortionParameter;
    float4 fBlendTextureParameter;
    float4 softParticleParam;
    float4 reconstructionParam1;
    float4 reconstructionParam2;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float2 Input_UV [[user(locn0), centroid_perspective]];
    float4 Input_ProjBinormal [[user(locn1)]];
    float4 Input_ProjTangent [[user(locn2)]];
    float4 Input_PosP [[user(locn3)]];
    float4 Input_Color [[user(locn4), centroid_perspective]];
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
float4 _main(PS_Input Input, texture2d<float> _colorTex, sampler sampler_colorTex, constant PS_ConstantBuffer& _155, texture2d<float> _backTex, sampler sampler_backTex, texture2d<float> _depthTex, sampler sampler_depthTex)
{
    float4 Output = _colorTex.sample(sampler_colorTex, Input.UV);
    Output.w *= Input.Color.w;
    float2 pos = Input.PosP.xy / float2(Input.PosP.w);
    float2 posR = Input.ProjTangent.xy / float2(Input.ProjTangent.w);
    float2 posU = Input.ProjBinormal.xy / float2(Input.ProjBinormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * _155.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * _155.g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = _155.mUVInversedBack.x + (_155.mUVInversedBack.y * uv.y);
    float3 color = float3(_backTex.sample(sampler_backTex, uv).xyz);
    Output.x = color.x;
    Output.y = color.y;
    Output.z = color.z;
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    if (_155.softParticleParam.w != 0.0)
    {
        float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
        float param = backgroundZ;
        float param_1 = screenPos.z;
        float4 param_2 = _155.softParticleParam;
        float4 param_3 = _155.reconstructionParam1;
        float4 param_4 = _155.reconstructionParam2;
        Output.w *= SoftParticle(param, param_1, param_2, param_3, param_4);
    }
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstantBuffer& _155 [[buffer(1)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _backTex [[texture(1)]], texture2d<float> _depthTex [[texture(2)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_backTex [[sampler(1)]], sampler sampler_depthTex [[sampler(2)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = in.Input_UV;
    Input.ProjBinormal = in.Input_ProjBinormal;
    Input.ProjTangent = in.Input_ProjTangent;
    Input.PosP = in.Input_PosP;
    Input.Color = in.Input_Color;
    float4 _314 = _main(Input, _colorTex, sampler_colorTex, _155, _backTex, sampler_backTex, _depthTex, sampler_depthTex);
    out._entryPointOutput = _314;
    return out;
}

)";

