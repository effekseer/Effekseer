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
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float2 Others;
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
    float2 Input_UV [[user(locn0), centroid_perspective]];
    float3 Input_Normal [[user(locn1)]];
    float3 Input_Binormal [[user(locn2)]];
    float3 Input_Tangent [[user(locn3)]];
    float4 Input_Color [[user(locn4), centroid_perspective]];
    float4 Input_Alpha_Dist_UV [[user(locn5)]];
    float4 Input_Blend_Alpha_Dist_UV [[user(locn6)]];
    float4 Input_Blend_FBNextIndex_UV [[user(locn7)]];
    float2 Input_Others [[user(locn8)]];
    float4 Input_PosP [[user(locn9)]];
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
    ret.FlipbookRate = psinput.Others.x;
    ret.AlphaThreshold = psinput.Others.y;
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
        float3 _93 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = float4(_93.x, _93.y, _93.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            float3 _105 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_105.x, _105.y, _105.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                float3 _118 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_118.x, _118.y, _118.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    float3 _131 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_131.x, _131.y, _131.z, dstColor.w);
                }
            }
        }
    }
}

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
float4 _main(PS_Input Input, thread texture2d<float> _uvDistortionTex, thread sampler sampler_uvDistortionTex, constant PS_ConstanBuffer& v_264, thread texture2d<float> _colorTex, thread sampler sampler_colorTex, thread texture2d<float> _alphaTex, thread sampler sampler_alphaTex, thread texture2d<float> _blendUVDistortionTex, thread sampler sampler_blendUVDistortionTex, thread texture2d<float> _blendTex, thread sampler sampler_blendTex, thread texture2d<float> _blendAlphaTex, thread sampler sampler_blendAlphaTex, thread texture2d<float> _depthTex, thread sampler sampler_depthTex)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = v_264.fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2);
    UVOffset *= v_264.fUVDistortionParameter.x;
    float4 Output = _colorTex.sample(sampler_colorTex, (Input.UV + UVOffset)) * Input.Color;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _colorTex, sampler_colorTex, v_264.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 AlphaTexColor = _alphaTex.sample(sampler_alphaTex, (advancedParam.AlphaUV + UVOffset));
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = v_264.fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_5, param_6);
    BlendUVOffset *= v_264.fUVDistortionParameter.y;
    float4 BlendTextureColor = _blendTex.sample(sampler_blendTex, (advancedParam.BlendUV + BlendUVOffset));
    float4 BlendAlphaTextureColor = _blendAlphaTex.sample(sampler_blendAlphaTex, (advancedParam.BlendAlphaUV + BlendUVOffset));
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, v_264.fBlendTextureParameter.x);
    Output = param_7;
    if (v_264.fFalloffParam.Param.x == 1.0)
    {
        float3 cameraVec = normalize(-v_264.fCameraFrontDirection.xyz);
        float CdotN = fast::clamp(dot(cameraVec, normalize(Input.Normal)), 0.0, 1.0);
        float4 FalloffBlendColor = mix(v_264.fFalloffParam.EndColor, v_264.fFalloffParam.BeginColor, float4(pow(CdotN, v_264.fFalloffParam.Param.z)));
        if (v_264.fFalloffParam.Param.y == 0.0)
        {
            float3 _420 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_420.x, _420.y, _420.z, Output.w);
        }
        else
        {
            if (v_264.fFalloffParam.Param.y == 1.0)
            {
                float3 _433 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_433.x, _433.y, _433.z, Output.w);
            }
            else
            {
                if (v_264.fFalloffParam.Param.y == 2.0)
                {
                    float3 _446 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_446.x, _446.y, _446.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _459 = Output.xyz * v_264.fEmissiveScaling.x;
    Output = float4(_459.x, _459.y, _459.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
    if ((isunordered(v_264.softParticleAndReconstructionParam1.x, 0.0) || v_264.softParticleAndReconstructionParam1.x != 0.0))
    {
        float param_8 = backgroundZ;
        float param_9 = screenPos.z;
        float param_10 = v_264.softParticleAndReconstructionParam1.x;
        float2 param_11 = v_264.softParticleAndReconstructionParam1.yz;
        float4 param_12 = v_264.reconstructionParam2;
        Output.w *= SoftParticle(param_8, param_9, param_10, param_11, param_12);
    }
    if (Output.w <= fast::max(0.0, advancedParam.AlphaThreshold))
    {
        discard_fragment();
    }
    float3 _542 = mix(v_264.fEdgeColor.xyz * v_264.fEdgeParameter.y, Output.xyz, float3(ceil((Output.w - advancedParam.AlphaThreshold) - v_264.fEdgeParameter.x)));
    Output = float4(_542.x, _542.y, _542.z, Output.w);
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_264 [[buffer(0)]], texture2d<float> _uvDistortionTex [[texture(2)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _alphaTex [[texture(1)]], texture2d<float> _blendUVDistortionTex [[texture(5)]], texture2d<float> _blendTex [[texture(3)]], texture2d<float> _blendAlphaTex [[texture(4)]], texture2d<float> _depthTex [[texture(6)]], sampler sampler_uvDistortionTex [[sampler(2)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_alphaTex [[sampler(1)]], sampler sampler_blendUVDistortionTex [[sampler(5)]], sampler sampler_blendTex [[sampler(3)]], sampler sampler_blendAlphaTex [[sampler(4)]], sampler sampler_depthTex [[sampler(6)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = in.Input_UV;
    Input.Normal = in.Input_Normal;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.Color = in.Input_Color;
    Input.Alpha_Dist_UV = in.Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = in.Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = in.Input_Blend_FBNextIndex_UV;
    Input.Others = in.Input_Others;
    Input.PosP = in.Input_PosP;
    float4 _588 = _main(Input, _uvDistortionTex, sampler_uvDistortionTex, v_264, _colorTex, sampler_colorTex, _alphaTex, sampler_alphaTex, _blendUVDistortionTex, sampler_blendUVDistortionTex, _blendTex, sampler_blendTex, _blendAlphaTex, sampler_blendAlphaTex, _depthTex, sampler_depthTex);
    out._entryPointOutput = _588;
    return out;
}

