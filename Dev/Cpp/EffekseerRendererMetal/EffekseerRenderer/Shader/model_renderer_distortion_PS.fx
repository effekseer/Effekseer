#pragma clang diagnostic ignored "-Wmissing-prototypes"

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float4 Binormal;
    float4 Tangent;
    float4 PosP;
    float4 Color;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float2 Others;
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
    float4 g_scale;
    float4 mUVInversedBack;
    float4 fFlipbookParameter;
    float4 fUVDistortionParameter;
    float4 fBlendTextureParameter;
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
    float4 Input_Binormal [[user(locn1)]];
    float4 Input_Tangent [[user(locn2)]];
    float4 Input_PosP [[user(locn3)]];
    float4 Input_Color [[user(locn4), centroid_perspective]];
    float4 Input_Alpha_Dist_UV [[user(locn5)]];
    float4 Input_Blend_Alpha_Dist_UV [[user(locn6)]];
    float4 Input_Blend_FBNextIndex_UV [[user(locn7)]];
    float2 Input_Others [[user(locn8)]];
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
float4 _main(PS_Input Input, thread texture2d<float> g_uvDistortionTexture, thread sampler g_uvDistortionSampler, constant PS_ConstanBuffer& v_263, thread texture2d<float> g_texture, thread sampler g_sampler, thread texture2d<float> g_alphaTexture, thread sampler g_alphaSampler, thread texture2d<float> g_blendUVDistortionTexture, thread sampler g_blendUVDistortionSampler, thread texture2d<float> g_blendTexture, thread sampler g_blendSampler, thread texture2d<float> g_blendAlphaTexture, thread sampler g_blendAlphaSampler, thread texture2d<float> g_backTexture, thread sampler g_backSampler, thread texture2d<float> g_depthTexture, thread sampler g_depthSampler)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = v_263.fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(g_uvDistortionTexture, g_uvDistortionSampler, param_1, param_2);
    UVOffset *= v_263.fUVDistortionParameter.x;
    float4 Output = g_texture.sample(g_sampler, (Input.UV + UVOffset));
    Output.w *= Input.Color.w;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, g_texture, g_sampler, v_263.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 AlphaTexColor = g_alphaTexture.sample(g_alphaSampler, (advancedParam.AlphaUV + UVOffset));
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = v_263.fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(g_blendUVDistortionTexture, g_blendUVDistortionSampler, param_5, param_6);
    BlendUVOffset *= v_263.fUVDistortionParameter.y;
    float4 BlendTextureColor = g_blendTexture.sample(g_blendSampler, (advancedParam.BlendUV + BlendUVOffset));
    float4 BlendAlphaTextureColor = g_blendAlphaTexture.sample(g_blendAlphaSampler, (advancedParam.BlendAlphaUV + BlendUVOffset));
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, v_263.fBlendTextureParameter.x);
    Output = param_7;
    if (Output.w <= fast::max(0.0, advancedParam.AlphaThreshold))
    {
        discard_fragment();
    }
    float2 pos = Input.PosP.xy / float2(Input.PosP.w);
    float2 posU = Input.Tangent.xy / float2(Input.Tangent.w);
    float2 posR = Input.Binormal.xy / float2(Input.Binormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * v_263.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * v_263.g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = v_263.mUVInversedBack.x + (v_263.mUVInversedBack.y * uv.y);
    float3 color = float3(g_backTexture.sample(g_backSampler, uv).xyz);
    Output = float4(color.x, color.y, color.z, Output.w);
    float4 screenPos = Input.PosP / float4(Input.PosP.w);
    float2 screenUV = (screenPos.xy + float2(1.0)) / float2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = g_depthTexture.sample(g_depthSampler, screenUV).x;
    if ((isunordered(v_263.softParticleAndReconstructionParam1.x, 0.0) || v_263.softParticleAndReconstructionParam1.x != 0.0))
    {
        float param_8 = backgroundZ;
        float param_9 = screenPos.z;
        float param_10 = v_263.softParticleAndReconstructionParam1.x;
        float2 param_11 = v_263.softParticleAndReconstructionParam1.yz;
        float4 param_12 = v_263.reconstructionParam2;
        Output.w *= SoftParticle(param_8, param_9, param_10, param_11, param_12);
    }
    return Output;
}

fragment main0_out main0(main0_in in [[stage_in]], constant PS_ConstanBuffer& v_263 [[buffer(0)]], texture2d<float> g_uvDistortionTexture [[texture(3)]], texture2d<float> g_texture [[texture(0)]], texture2d<float> g_alphaTexture [[texture(2)]], texture2d<float> g_blendUVDistortionTexture [[texture(6)]], texture2d<float> g_blendTexture [[texture(4)]], texture2d<float> g_blendAlphaTexture [[texture(5)]], texture2d<float> g_backTexture [[texture(1)]], texture2d<float> g_depthTexture [[texture(7)]], sampler g_uvDistortionSampler [[sampler(3)]], sampler g_sampler [[sampler(0)]], sampler g_alphaSampler [[sampler(2)]], sampler g_blendUVDistortionSampler [[sampler(6)]], sampler g_blendSampler [[sampler(4)]], sampler g_blendAlphaSampler [[sampler(5)]], sampler g_backSampler [[sampler(1)]], sampler g_depthSampler [[sampler(7)]], float4 gl_FragCoord [[position]])
{
    main0_out out = {};
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = in.Input_UV;
    Input.Binormal = in.Input_Binormal;
    Input.Tangent = in.Input_Tangent;
    Input.PosP = in.Input_PosP;
    Input.Color = in.Input_Color;
    Input.Alpha_Dist_UV = in.Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = in.Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = in.Input_Blend_FBNextIndex_UV;
    Input.Others = in.Input_Others;
    float4 _571 = _main(Input, g_uvDistortionTexture, g_uvDistortionSampler, v_263, g_texture, g_sampler, g_alphaTexture, g_alphaSampler, g_blendUVDistortionTexture, g_blendUVDistortionSampler, g_blendTexture, g_blendSampler, g_blendAlphaTexture, g_blendAlphaSampler, g_backTexture, g_backSampler, g_depthTexture, g_depthSampler);
    out._entryPointOutput = _571;
    return out;
}

