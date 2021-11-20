static const char metal_model_unlit_ps[] = R"(mtlcode
#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
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
    float4 miscFlags;
};

struct main0_out
{
    float4 _entryPointOutput [[color(0)]];
};

struct main0_in
{
    float4 Input_Color [[user(locn0), centroid_perspective]];
    float2 Input_UV [[user(locn1), centroid_perspective]];
    float4 Input_PosP [[user(locn2)]];
};

static inline __attribute__((always_inline))
float3 PositivePow(thread const float3& base, thread const float3& power)
{
    return pow(fast::max(abs(base), float3(1.1920928955078125e-07)), power);
}

static inline __attribute__((always_inline))
float3 LinearToSRGB(thread const float3& c)
{
    float3 param = c;
    float3 param_1 = float3(0.4166666567325592041015625);
    return fast::max((PositivePow(param, param_1) * 1.05499994754791259765625) - float3(0.054999999701976776123046875), float3(0.0));
}

static inline __attribute__((always_inline))
float4 LinearToSRGB(thread const float4& c)
{
    float3 param = c.xyz;
    return float4(LinearToSRGB(param), c.w);
}

static inline __attribute__((always_inline))
float4 ConvertFromSRGBTexture(thread const float4& c, constant PS_ConstanBuffer& v_197)
{
    if (v_197.miscFlags.x == 0.0)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

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
float3 SRGBToLinear(thread const float3& c)
{
    return fast::min(c, c * ((c * ((c * 0.305306017398834228515625) + float3(0.6821711063385009765625))) + float3(0.01252287812530994415283203125)));
}

static inline __attribute__((always_inline))
float4 SRGBToLinear(thread const float4& c)
{
    float3 param = c.xyz;
    return float4(SRGBToLinear(param), c.w);
}

static inline __attribute__((always_inline))
float4 ConvertToScreen(thread const float4& c, constant PS_ConstanBuffer& v_197)
{
    if (v_197.miscFlags.x == 0.0)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, constant PS_ConstanBuffer& v_197, thread texture2d<float> _colorTex, thread sampler sampler_colorTex, thread texture2d<float> _depthTex, thread sampler sampler_depthTex)
{
    float4 param = _colorTex.sample(sampler_colorTex, Input.UV);
    float4 Output = ConvertFromSRGBTexture(param, v_197) * Input.Color;
    float3 _250 = Output.xyz * v_197.fEmissiveScaling.x;
    Output = float4(_250.x, _250.y, _250.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = v_197.mUVInversedBack.x + (v_197.mUVInversedBack.y * screenUV.y);
    if ((isunordered(v_197.softParticleParam.w, 0.0) || v_197.softParticleParam.w != 0.0))
    {
        float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
        float param_1 = backgroundZ;
        float param_2 = screenPos.z;
        float4 param_3 = v_197.softParticleParam;
        float4 param_4 = v_197.reconstructionParam1;
        float4 param_5 = v_197.reconstructionParam2;
        Output.w *= SoftParticle(param_1, param_2, param_3, param_4, param_5);
    }
    if (Output.w == 0.0)
    {
        discard_fragment();
    }
    float4 param_6 = Output;
    return ConvertToScreen(param_6, v_197);
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_197 [[buffer(0)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _depthTex [[texture(1)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_depthTex [[sampler(1)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = in.Input_Color;
    Input.UV = in.Input_UV;
    Input.PosP = in.Input_PosP;
    float4 _349 = _main(Input, v_197, _colorTex, sampler_colorTex, _depthTex, sampler_depthTex);
    out._entryPointOutput = _349;
    return out;
}

)";

