#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float4 VColor;
    float2 UV;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
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

struct PS_ConstanBuffer
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
    float2 Input_UV [[user(locn1), centroid_perspective]];
    float3 Input_WorldN [[user(locn2)]];
    float3 Input_WorldT [[user(locn3)]];
    float3 Input_WorldB [[user(locn4)]];
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
float4 _main(PS_Input Input, thread texture2d<float> _uvDistortionTex, thread sampler sampler_uvDistortionTex, constant PS_ConstanBuffer& v_263, thread texture2d<float> _normalTex, thread sampler sampler_normalTex, thread texture2d<float> _colorTex, thread sampler sampler_colorTex, thread texture2d<float> _depthTex, thread sampler sampler_depthTex, thread texture2d<float> _alphaTex, thread sampler sampler_alphaTex, thread texture2d<float> _blendUVDistortionTex, thread sampler sampler_blendUVDistortionTex, thread texture2d<float> _blendTex, thread sampler sampler_blendTex, thread texture2d<float> _blendAlphaTex, thread sampler sampler_blendAlphaTex)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = v_263.fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2);
    UVOffset *= v_263.fUVDistortionParameter.x;
    float diffuse = 1.0;
    float3 loN = _normalTex.sample(sampler_normalTex, (Input.UV + UVOffset)).xyz;
    float3 texNormal = (loN - float3(0.5)) * 2.0;
    float3 localNormal = normalize(float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN)) * texNormal);
    diffuse = fast::max(dot(v_263.fLightDirection.xyz, localNormal), 0.0);
    float4 Output = _colorTex.sample(sampler_colorTex, (Input.UV + UVOffset)) * Input.VColor;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _colorTex, sampler_colorTex, v_263.fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = _depthTex.sample(sampler_depthTex, screenUV).x;
    if ((isunordered(v_263.softParticleAndReconstructionParam1.x, 0.0) || v_263.softParticleAndReconstructionParam1.x != 0.0))
    {
        float param_5 = backgroundZ;
        float param_6 = screenPos.z;
        float param_7 = v_263.softParticleAndReconstructionParam1.x;
        float2 param_8 = v_263.softParticleAndReconstructionParam1.yz;
        float4 param_9 = v_263.reconstructionParam2;
        Output.w *= SoftParticle(param_5, param_6, param_7, param_8, param_9);
    }
    float4 AlphaTexColor = _alphaTex.sample(sampler_alphaTex, (advancedParam.AlphaUV + UVOffset));
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_10 = advancedParam.BlendUVDistortionUV;
    float2 param_11 = v_263.fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_10, param_11);
    BlendUVOffset *= v_263.fUVDistortionParameter.y;
    float4 BlendTextureColor = _blendTex.sample(sampler_blendTex, (advancedParam.BlendUV + BlendUVOffset));
    float4 BlendAlphaTextureColor = _blendAlphaTex.sample(sampler_blendAlphaTex, (advancedParam.BlendAlphaUV + BlendUVOffset));
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_12 = Output;
    ApplyTextureBlending(param_12, BlendTextureColor, v_263.fBlendTextureParameter.x);
    Output = param_12;
    float3 _482 = Output.xyz * v_263.fEmissiveScaling.x;
    Output = float4(_482.x, _482.y, _482.z, Output.w);
    if (Output.w <= fast::max(0.0, advancedParam.AlphaThreshold))
    {
        discard_fragment();
    }
    float3 _507 = Output.xyz * (float3(diffuse, diffuse, diffuse) + float3(v_263.fLightAmbient.xyz));
    Output = float4(_507.x, _507.y, _507.z, Output.w);
    float3 _528 = mix(v_263.fEdgeColor.xyz * v_263.fEdgeParameter.y, Output.xyz, float3(ceil((Output.w - advancedParam.AlphaThreshold) - v_263.fEdgeParameter.x)));
    Output = float4(_528.x, _528.y, _528.z, Output.w);
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_263 [[buffer(0)]], texture2d<float> _uvDistortionTex [[texture(3)]], texture2d<float> _normalTex [[texture(1)]], texture2d<float> _colorTex [[texture(0)]], texture2d<float> _depthTex [[texture(7)]], texture2d<float> _alphaTex [[texture(2)]], texture2d<float> _blendUVDistortionTex [[texture(6)]], texture2d<float> _blendTex [[texture(4)]], texture2d<float> _blendAlphaTex [[texture(5)]], sampler sampler_uvDistortionTex [[sampler(3)]], sampler sampler_normalTex [[sampler(1)]], sampler sampler_colorTex [[sampler(0)]], sampler sampler_depthTex [[sampler(7)]], sampler sampler_alphaTex [[sampler(2)]], sampler sampler_blendUVDistortionTex [[sampler(6)]], sampler sampler_blendTex [[sampler(4)]], sampler sampler_blendAlphaTex [[sampler(5)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.VColor = in.Input_VColor;
    Input.UV = in.Input_UV;
    Input.WorldN = in.Input_WorldN;
    Input.WorldT = in.Input_WorldT;
    Input.WorldB = in.Input_WorldB;
    Input.Alpha_Dist_UV = in.Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = in.Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = in.Input_Blend_FBNextIndex_UV;
    Input.Others = in.Input_Others;
    Input.PosP = in.Input_PosP;
    float4 _574 = _main(Input, _uvDistortionTex, sampler_uvDistortionTex, v_263, _normalTex, sampler_normalTex, _colorTex, sampler_colorTex, _depthTex, sampler_depthTex, _alphaTex, sampler_alphaTex, _blendUVDistortionTex, sampler_blendUVDistortionTex, _blendTex, sampler_blendTex, _blendAlphaTex, sampler_blendAlphaTex);
    out._entryPointOutput = _574;
    return out;
}

