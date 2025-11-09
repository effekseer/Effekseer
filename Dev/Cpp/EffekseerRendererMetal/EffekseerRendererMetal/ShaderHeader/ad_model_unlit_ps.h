static const char metal_ad_model_unlit_ps[] = R"(mtlcode
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

struct PS_ConstantBuffer
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
float3 PositivePow(thread const float3& base, thread const float3& power)
{
    return powr(fast::max(abs(base), float3(1.1920928955078125e-07)), power);
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
float4 ConvertFromSRGBTexture(thread const float4& c, thread const bool& isValid)
{
    if (!isValid)
    {
        return c;
    }
    float4 param = c;
    return LinearToSRGB(param);
}

static inline __attribute__((always_inline))
float2 UVDistortionOffset(texture2d<float> t, sampler s, thread const float2& uv, thread const float2& uvInversed, thread const bool& convertFromSRGB)
{
    float4 sampledColor = t.sample(s, uv);
    if (convertFromSRGB)
    {
        float4 param = sampledColor;
        bool param_1 = convertFromSRGB;
        sampledColor = ConvertFromSRGBTexture(param, param_1);
    }
    float2 UVOffset = (sampledColor.xy * 2.0) - float2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

static inline __attribute__((always_inline))
void ApplyFlipbook(thread float4& dst, texture2d<float> t, sampler s, float4 flipbookParameter, float4 vcolor, float2 nextUV, thread const float& flipbookRate, thread const bool& convertFromSRGB)
{
    if (flipbookParameter.x > 0.0)
    {
        float4 sampledColor = t.sample(s, nextUV);
        if (convertFromSRGB)
        {
            float4 param = sampledColor;
            bool param_1 = convertFromSRGB;
            sampledColor = ConvertFromSRGBTexture(param, param_1);
        }
        float4 NextPixelColor = sampledColor * vcolor;
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
        float4 _219 = dstColor;
        float3 _222 = (blendColor.xyz * blendColor.w) + (_219.xyz * (1.0 - blendColor.w));
        dstColor.x = _222.x;
        dstColor.y = _222.y;
        dstColor.z = _222.z;
    }
    else
    {
        if (blendType == 1.0)
        {
            float4 _237 = dstColor;
            float3 _239 = _237.xyz + (blendColor.xyz * blendColor.w);
            dstColor.x = _239.x;
            dstColor.y = _239.y;
            dstColor.z = _239.z;
        }
        else
        {
            if (blendType == 2.0)
            {
                float4 _254 = dstColor;
                float3 _256 = _254.xyz - (blendColor.xyz * blendColor.w);
                dstColor.x = _256.x;
                dstColor.y = _256.y;
                dstColor.z = _256.z;
            }
            else
            {
                if (blendType == 3.0)
                {
                    float4 _271 = dstColor;
                    float3 _273 = _271.xyz * (blendColor.xyz * blendColor.w);
                    dstColor.x = _273.x;
                    dstColor.y = _273.y;
                    dstColor.z = _273.z;
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
float4 ConvertToScreen(thread const float4& c, thread const bool& isValid)
{
    if (!isValid)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

static inline __attribute__((always_inline))
float4 _main(PS_Input Input, constant PS_ConstantBuffer& _433, texture2d<float> _uvDistortionTex, sampler sampler_uvDistortionTex, texture2d<float> _colorTex, sampler sampler_colorTex, texture2d<float> _alphaTex, sampler sampler_alphaTex, texture2d<float> _blendUVDistortionTex, sampler sampler_blendUVDistortionTex, texture2d<float> _blendTex, sampler sampler_blendTex, texture2d<float> _blendAlphaTex, sampler sampler_blendAlphaTex, texture2d<float> _depthTex, sampler sampler_depthTex)
{
    bool convertColorSpace = _433.miscFlags.x != 0.0;
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _433.fUVDistortionParameter.zw;
    bool param_3 = convertColorSpace;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2, param_3);
    UVOffset *= _433.fUVDistortionParameter.x;
    float4 param_4 = _colorTex.sample(sampler_colorTex, (Input.UV_Others.xy + UVOffset));
    bool param_5 = convertColorSpace;
    float4 Output = ConvertFromSRGBTexture(param_4, param_5) * Input.Color;
    float4 param_6 = Output;
    float param_7 = advancedParam.FlipbookRate;
    bool param_8 = convertColorSpace;
    ApplyFlipbook(param_6, _colorTex, sampler_colorTex, _433.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_7, param_8);
    Output = param_6;
    float4 param_9 = _alphaTex.sample(sampler_alphaTex, (advancedParam.AlphaUV + UVOffset));
    bool param_10 = convertColorSpace;
    float4 AlphaTexColor = ConvertFromSRGBTexture(param_9, param_10);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_11 = advancedParam.BlendUVDistortionUV;
    float2 param_12 = _433.fUVDistortionParameter.zw;
    bool param_13 = convertColorSpace;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_11, param_12, param_13);
    BlendUVOffset *= _433.fUVDistortionParameter.y;
    float4 param_14 = _blendTex.sample(sampler_blendTex, (advancedParam.BlendUV + BlendUVOffset));
    bool param_15 = convertColorSpace;
    float4 BlendTextureColor = ConvertFromSRGBTexture(param_14, param_15);
    float4 param_16 = _blendAlphaTex.sample(sampler_blendAlphaTex, (advancedParam.BlendAlphaUV + BlendUVOffset));
    bool param_17 = convertColorSpace;
    float4 BlendAlphaTextureColor = ConvertFromSRGBTexture(param_16, param_17);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_18 = Output;
    ApplyTextureBlending(param_18, BlendTextureColor, _433.fBlendTextureParameter.x);
    Output = param_18;
    if (_433.fFalloffParameter.x == 1.0)
    {
        float3 cameraVec = fast::normalize(-_433.fCameraFrontDirection.xyz);
        float CdotN = fast::clamp(dot(cameraVec, fast::normalize(Input.WorldN)), 0.0, 1.0);
        float4 FalloffBlendColor = mix(_433.fFalloffEndColor, _433.fFalloffBeginColor, float4(powr(CdotN, _433.fFalloffParameter.z)));
        if (_433.fFalloffParameter.y == 0.0)
        {
            float4 _625 = Output;
            float3 _627 = _625.xyz + FalloffBlendColor.xyz;
            Output.x = _627.x;
            Output.y = _627.y;
            Output.z = _627.z;
        }
        else
        {
            if (_433.fFalloffParameter.y == 1.0)
            {
                float4 _642 = Output;
                float3 _644 = _642.xyz - FalloffBlendColor.xyz;
                Output.x = _644.x;
                Output.y = _644.y;
                Output.z = _644.z;
            }
            else
            {
                if (_433.fFalloffParameter.y == 2.0)
                {
                    float4 _659 = Output;
                    float3 _661 = _659.xyz * FalloffBlendColor.xyz;
                    Output.x = _661.x;
                    Output.y = _661.y;
                    Output.z = _661.z;
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float4 _677 = Output;
    float3 _679 = _677.xyz * _433.fEmissiveScaling.x;
    Output.x = _679.x;
    Output.y = _679.y;
    Output.z = _679.z;
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = _433.mUVInversedBack.x + (_433.mUVInversedBack.y * screenUV.y);
    if (_433.softParticleParam.w != 0.0)
    {
        float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
        float param_19 = backgroundZ;
        float param_20 = screenPos.z;
        float4 param_21 = _433.softParticleParam;
        float4 param_22 = _433.reconstructionParam1;
        float4 param_23 = _433.reconstructionParam2;
        Output.w *= SoftParticle(param_19, param_20, param_21, param_22, param_23);
    }
    if (Output.w <= fast::max(0.0, advancedParam.AlphaThreshold))
    {
        discard_fragment();
    }
    float4 _765 = Output;
    float _768 = Output.w;
    float3 _777 = mix(_433.fEdgeColor.xyz * _433.fEdgeParameter.y, _765.xyz, float3(ceil((_768 - advancedParam.AlphaThreshold) - _433.fEdgeParameter.x)));
    Output.x = _777.x;
    Output.y = _777.y;
    Output.z = _777.z;
    float4 param_24 = Output;
    bool param_25 = convertColorSpace;
    return ConvertToScreen(param_24, param_25);
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstantBuffer& _433 [[buffer(1)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _alphaTex [[texture(1)]], texture2d<float> _uvDistortionTex [[texture(2)]], texture2d<float> _blendTex [[texture(3)]], texture2d<float> _blendAlphaTex [[texture(4)]], texture2d<float> _blendUVDistortionTex [[texture(5)]], texture2d<float> _depthTex [[texture(6)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_alphaTex [[sampler(1)]], sampler sampler_uvDistortionTex [[sampler(2)]], sampler sampler_blendTex [[sampler(3)]], sampler sampler_blendAlphaTex [[sampler(4)]], sampler sampler_blendUVDistortionTex [[sampler(5)]], sampler sampler_depthTex [[sampler(6)]], float4 gl_FragCoord [[position]])
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
    float4 _821 = _main(Input, _433, _uvDistortionTex, sampler_uvDistortionTex, _colorTex, sampler_colorTex, _alphaTex, sampler_alphaTex, _blendUVDistortionTex, sampler_blendUVDistortionTex, _blendTex, sampler_blendTex, _blendAlphaTex, sampler_blendAlphaTex, _depthTex, sampler_depthTex);
    out._entryPointOutput = _821;
    return out;
}

)";

