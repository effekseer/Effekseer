#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float4 PosP;
};

struct AdvancedParameter
{
    float2 AlphaUV;
    float2 UVDistortionUV;
    float2 BlendUV;
    float2 BlendAlphaUV;
    float2 BlendUVDistortionUV;
    float2 FlipbookNextIndexUV;
    float FlipbookRate;
    float AlphaThreshold;
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
    float4 Input_UV_Others [[user(locn1), centroid_perspective]];
    float3 Input_WorldN [[user(locn2)]];
    float4 Input_Alpha_Dist_UV [[user(locn3)]];
    float4 Input_Blend_Alpha_Dist_UV [[user(locn4)]];
    float4 Input_Blend_FBNextIndex_UV [[user(locn5)]];
    float4 Input_PosP [[user(locn6)]];
};

static inline __attribute__((always_inline))
AdvancedParameter DisolveAdvancedParameter(thread const PS_Input& psinput)
{
    AdvancedParameter ret;
    ret.AlphaUV = psinput.Alpha_Dist_UV.xy;
    ret.UVDistortionUV = psinput.Alpha_Dist_UV.zw;
    ret.BlendUV = psinput.Blend_FBNextIndex_UV.xy;
    ret.BlendAlphaUV = psinput.Blend_Alpha_Dist_UV.xy;
    ret.BlendUVDistortionUV = psinput.Blend_Alpha_Dist_UV.zw;
    ret.FlipbookNextIndexUV = psinput.Blend_FBNextIndex_UV.zw;
    ret.FlipbookRate = psinput.UV_Others.z;
    ret.AlphaThreshold = psinput.UV_Others.w;
    return ret;
}

static inline __attribute__((always_inline))
float2 UVDistortionOffset(texture2d<float> t, sampler s, thread const float2& uv, thread const float2& uvInversed)
{
    float2 UVOffset = (t.sample(s, uv).xy * 2.0) - float2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

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
float4 ConvertFromSRGBTexture(thread const float4& c, constant PS_ConstanBuffer& v_368)
{
    if (v_368.miscFlags.x == 0.0)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

static inline __attribute__((always_inline))
void ApplyFlipbook(thread float4& dst, texture2d<float> t, sampler s, float4 flipbookParameter, float4 vcolor, float2 nextUV, thread const float& flipbookRate)
{
    if (flipbookParameter.x > 0.0)
    {
        float4 NextPixelColor = t.sample(s, nextUV) * vcolor;
        if (flipbookParameter.y == 1.0)
        {
            dst = mix(dst, NextPixelColor, float4(flipbookRate));
        }
    }
}

static inline __attribute__((always_inline))
void ApplyTextureBlending(thread float4& dstColor, float4 blendColor, float blendType)
{
    if (blendType == 0.0)
    {
        float3 _119 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = float4(_119.x, _119.y, _119.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            float3 _131 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_131.x, _131.y, _131.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                float3 _144 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_144.x, _144.y, _144.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    float3 _157 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_157.x, _157.y, _157.z, dstColor.w);
                }
            }
        }
    }
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
float4 ConvertToScreen(thread const float4& c, constant PS_ConstanBuffer& v_368)
{
    if (v_368.miscFlags.x == 0.0)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, constant PS_ConstanBuffer& v_368, thread texture2d<float> _uvDistortionTex, thread sampler sampler_uvDistortionTex, thread texture2d<float> _colorTex, thread sampler sampler_colorTex, thread texture2d<float> _alphaTex, thread sampler sampler_alphaTex, thread texture2d<float> _blendUVDistortionTex, thread sampler sampler_blendUVDistortionTex, thread texture2d<float> _blendTex, thread sampler sampler_blendTex, thread texture2d<float> _blendAlphaTex, thread sampler sampler_blendAlphaTex, thread texture2d<float> _depthTex, thread sampler sampler_depthTex)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = v_368.fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2);
    UVOffset *= v_368.fUVDistortionParameter.x;
    float4 param_3 = _colorTex.sample(sampler_colorTex, (Input.UV_Others.xy + UVOffset));
    float4 Output = ConvertFromSRGBTexture(param_3, v_368) * Input.Color;
    float4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, _colorTex, sampler_colorTex, v_368.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5);
    Output = param_4;
    float4 AlphaTexColor = _alphaTex.sample(sampler_alphaTex, (advancedParam.AlphaUV + UVOffset));
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_6 = advancedParam.BlendUVDistortionUV;
    float2 param_7 = v_368.fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_6, param_7);
    BlendUVOffset *= v_368.fUVDistortionParameter.y;
    float4 param_8 = _blendTex.sample(sampler_blendTex, (advancedParam.BlendUV + BlendUVOffset));
    float4 BlendTextureColor = ConvertFromSRGBTexture(param_8, v_368);
    float4 BlendAlphaTextureColor = _blendAlphaTex.sample(sampler_blendAlphaTex, (advancedParam.BlendAlphaUV + BlendUVOffset));
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, v_368.fBlendTextureParameter.x);
    Output = param_9;
    if (v_368.fFalloffParameter.x == 1.0)
    {
        float3 cameraVec = normalize(-v_368.fCameraFrontDirection.xyz);
        float CdotN = fast::clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0, 1.0);
        float4 FalloffBlendColor = mix(v_368.fFalloffEndColor, v_368.fFalloffBeginColor, float4(pow(CdotN, v_368.fFalloffParameter.z)));
        if (v_368.fFalloffParameter.y == 0.0)
        {
            float3 _565 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_565.x, _565.y, _565.z, Output.w);
        }
        else
        {
            if (v_368.fFalloffParameter.y == 1.0)
            {
                float3 _578 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_578.x, _578.y, _578.z, Output.w);
            }
            else
            {
                if (v_368.fFalloffParameter.y == 2.0)
                {
                    float3 _591 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_591.x, _591.y, _591.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _605 = Output.xyz * v_368.fEmissiveScaling.x;
    Output = float4(_605.x, _605.y, _605.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = v_368.mUVInversedBack.x + (v_368.mUVInversedBack.y * screenUV.y);
    if ((isunordered(v_368.softParticleParam.w, 0.0) || v_368.softParticleParam.w != 0.0))
    {
        float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
        float param_10 = backgroundZ;
        float param_11 = screenPos.z;
        float4 param_12 = v_368.softParticleParam;
        float4 param_13 = v_368.reconstructionParam1;
        float4 param_14 = v_368.reconstructionParam2;
        Output.w *= SoftParticle(param_10, param_11, param_12, param_13, param_14);
    }
    if (Output.w <= fast::max(0.0, advancedParam.AlphaThreshold))
    {
        discard_fragment();
    }
    float3 _699 = mix(v_368.fEdgeColor.xyz * v_368.fEdgeParameter.y, Output.xyz, float3(ceil((Output.w - advancedParam.AlphaThreshold) - v_368.fEdgeParameter.x)));
    Output = float4(_699.x, _699.y, _699.z, Output.w);
    float4 param_15 = Output;
    return ConvertToScreen(param_15, v_368);
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_368 [[buffer(0)]], texture2d<float> _uvDistortionTex [[texture(2)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _alphaTex [[texture(1)]], texture2d<float> _blendUVDistortionTex [[texture(5)]], texture2d<float> _blendTex [[texture(3)]], texture2d<float> _blendAlphaTex [[texture(4)]], texture2d<float> _depthTex [[texture(6)]], sampler sampler_uvDistortionTex [[sampler(2)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_alphaTex [[sampler(1)]], sampler sampler_blendUVDistortionTex [[sampler(5)]], sampler sampler_blendTex [[sampler(3)]], sampler sampler_blendAlphaTex [[sampler(4)]], sampler sampler_depthTex [[sampler(6)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = in.Input_Color;
    Input.UV_Others = in.Input_UV_Others;
    Input.WorldN = in.Input_WorldN;
    Input.Alpha_Dist_UV = in.Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = in.Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = in.Input_Blend_FBNextIndex_UV;
    Input.PosP = in.Input_PosP;
    float4 _737 = _main(Input, v_368, _uvDistortionTex, sampler_uvDistortionTex, _colorTex, sampler_colorTex, _alphaTex, sampler_alphaTex, _blendUVDistortionTex, sampler_blendUVDistortionTex, _blendTex, sampler_blendTex, _blendAlphaTex, sampler_blendAlphaTex, _depthTex, sampler_depthTex);
    out._entryPointOutput = _737;
    return out;
}

