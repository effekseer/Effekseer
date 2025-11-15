#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float4 UV_Others;
    float4 ProjBinormal;
    float4 ProjTangent;
    float4 PosP;
    float4 Color;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
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
    float4 Input_UV_Others [[user(locn0), centroid_perspective]];
    float4 Input_ProjBinormal [[user(locn1)]];
    float4 Input_ProjTangent [[user(locn2)]];
    float4 Input_PosP [[user(locn3)]];
    float4 Input_Color [[user(locn4), centroid_perspective]];
    float4 Input_Alpha_Dist_UV [[user(locn5)]];
    float4 Input_Blend_Alpha_Dist_UV [[user(locn6)]];
    float4 Input_Blend_FBNextIndex_UV [[user(locn7)]];
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
        float4 _169 = dstColor;
        float3 _172 = (blendColor.xyz * blendColor.w) + (_169.xyz * (1.0 - blendColor.w));
        dstColor.x = _172.x;
        dstColor.y = _172.y;
        dstColor.z = _172.z;
    }
    else
    {
        if (blendType == 1.0)
        {
            float4 _187 = dstColor;
            float3 _189 = _187.xyz + (blendColor.xyz * blendColor.w);
            dstColor.x = _189.x;
            dstColor.y = _189.y;
            dstColor.z = _189.z;
        }
        else
        {
            if (blendType == 2.0)
            {
                float4 _204 = dstColor;
                float3 _206 = _204.xyz - (blendColor.xyz * blendColor.w);
                dstColor.x = _206.x;
                dstColor.y = _206.y;
                dstColor.z = _206.z;
            }
            else
            {
                if (blendType == 3.0)
                {
                    float4 _221 = dstColor;
                    float3 _223 = _221.xyz * (blendColor.xyz * blendColor.w);
                    dstColor.x = _223.x;
                    dstColor.y = _223.y;
                    dstColor.z = _223.z;
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
float4 _main(PS_Input Input, texture2d<float> _uvDistortionTex, sampler sampler_uvDistortionTex, constant PS_ConstantBuffer& _393, texture2d<float> _colorTex, sampler sampler_colorTex, texture2d<float> _alphaTex, sampler sampler_alphaTex, texture2d<float> _blendUVDistortionTex, sampler sampler_blendUVDistortionTex, texture2d<float> _blendTex, sampler sampler_blendTex, texture2d<float> _blendAlphaTex, sampler sampler_blendAlphaTex, texture2d<float> _backTex, sampler sampler_backTex, texture2d<float> _depthTex, sampler sampler_depthTex)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _393.fUVDistortionParameter.zw;
    bool param_3 = false;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2, param_3);
    UVOffset *= _393.fUVDistortionParameter.x;
    float4 Output = _colorTex.sample(sampler_colorTex, (float2(Input.UV_Others.xy) + UVOffset));
    Output.w *= Input.Color.w;
    float4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    bool param_6 = false;
    ApplyFlipbook(param_4, _colorTex, sampler_colorTex, _393.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, param_6);
    Output = param_4;
    float4 AlphaTexColor = _alphaTex.sample(sampler_alphaTex, (advancedParam.AlphaUV + UVOffset));
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_7 = advancedParam.BlendUVDistortionUV;
    float2 param_8 = _393.fUVDistortionParameter.zw;
    bool param_9 = false;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_7, param_8, param_9);
    BlendUVOffset *= _393.fUVDistortionParameter.y;
    float4 BlendTextureColor = _blendTex.sample(sampler_blendTex, (advancedParam.BlendUV + BlendUVOffset));
    float4 BlendAlphaTextureColor = _blendAlphaTex.sample(sampler_blendAlphaTex, (advancedParam.BlendAlphaUV + BlendUVOffset));
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_10 = Output;
    ApplyTextureBlending(param_10, BlendTextureColor, _393.fBlendTextureParameter.x);
    Output = param_10;
    if (Output.w <= fast::max(0.0, advancedParam.AlphaThreshold))
    {
        discard_fragment();
    }
    float2 pos = Input.PosP.xy / float2(Input.PosP.w);
    float2 posR = Input.ProjTangent.xy / float2(Input.ProjTangent.w);
    float2 posU = Input.ProjBinormal.xy / float2(Input.ProjBinormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * _393.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * _393.g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = _393.mUVInversedBack.x + (_393.mUVInversedBack.y * uv.y);
    float3 color = float3(_backTex.sample(sampler_backTex, uv).xyz);
    Output.x = color.x;
    Output.y = color.y;
    Output.z = color.z;
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    if (_393.softParticleParam.w != 0.0)
    {
        float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
        float param_11 = backgroundZ;
        float param_12 = screenPos.z;
        float4 param_13 = _393.softParticleParam;
        float4 param_14 = _393.reconstructionParam1;
        float4 param_15 = _393.reconstructionParam2;
        Output.w *= SoftParticle(param_11, param_12, param_13, param_14, param_15);
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstantBuffer& _393 [[buffer(1)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _backTex [[texture(1)]], texture2d<float> _alphaTex [[texture(2)]], texture2d<float> _uvDistortionTex [[texture(3)]], texture2d<float> _blendTex [[texture(4)]], texture2d<float> _blendAlphaTex [[texture(5)]], texture2d<float> _blendUVDistortionTex [[texture(6)]], texture2d<float> _depthTex [[texture(7)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_backTex [[sampler(1)]], sampler sampler_alphaTex [[sampler(2)]], sampler sampler_uvDistortionTex [[sampler(3)]], sampler sampler_blendTex [[sampler(4)]], sampler sampler_blendAlphaTex [[sampler(5)]], sampler sampler_blendUVDistortionTex [[sampler(6)]], sampler sampler_depthTex [[sampler(7)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV_Others = in.Input_UV_Others;
    Input.ProjBinormal = in.Input_ProjBinormal;
    Input.ProjTangent = in.Input_ProjTangent;
    Input.PosP = in.Input_PosP;
    Input.Color = in.Input_Color;
    Input.Alpha_Dist_UV = in.Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = in.Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = in.Input_Blend_FBNextIndex_UV;
    float4 _706 = _main(Input, _uvDistortionTex, sampler_uvDistortionTex, _393, _colorTex, sampler_colorTex, _alphaTex, sampler_alphaTex, _blendUVDistortionTex, sampler_blendUVDistortionTex, _blendTex, sampler_blendTex, _blendAlphaTex, sampler_blendAlphaTex, _backTex, sampler_backTex, _depthTex, sampler_depthTex);
    out._entryPointOutput = _706;
    return out;
}

