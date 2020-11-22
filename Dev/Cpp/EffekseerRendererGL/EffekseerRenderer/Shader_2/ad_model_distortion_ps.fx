#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec2 UV;
    vec4 Binormal;
    vec4 Tangent;
    vec4 PosP;
    vec4 Color;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
    vec2 Others;
};

struct AdvancedParameter
{
    vec2 AlphaUV;
    vec2 UVDistortionUV;
    vec2 BlendUV;
    vec2 BlendAlphaUV;
    vec2 BlendUVDistortionUV;
    vec2 FlipbookNextIndexUV;
    float FlipbookRate;
    float AlphaThreshold;
};

struct PS_ConstanBuffer
{
    vec4 g_scale;
    vec4 mUVInversedBack;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_uvDistortionTex;
uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_alphaTex;
uniform sampler2D Sampler_sampler_blendUVDistortionTex;
uniform sampler2D Sampler_sampler_blendTex;
uniform sampler2D Sampler_sampler_blendAlphaTex;
uniform sampler2D Sampler_sampler_backTex;

centroid varying vec2 _VSPS_UV;
varying vec4 _VSPS_Binormal;
varying vec4 _VSPS_Tangent;
varying vec4 _VSPS_PosP;
centroid varying vec4 _VSPS_Color;
varying vec4 _VSPS_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_FBNextIndex_UV;
varying vec2 _VSPS_Others;

AdvancedParameter DisolveAdvancedParameter(PS_Input psinput)
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

vec2 UVDistortionOffset(vec2 uv, vec2 uvInversed, sampler2D SPIRV_Cross_Combinedts)
{
    vec2 UVOffset = (texture2D(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout vec4 dst, vec4 flipbookParameter, vec4 vcolor, vec2 nextUV, float flipbookRate, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        vec4 NextPixelColor = texture2D(SPIRV_Cross_Combinedts, nextUV) * vcolor;
        if (flipbookParameter.y == 1.0)
        {
            dst = mix(dst, NextPixelColor, vec4(flipbookRate));
        }
    }
}

void ApplyTextureBlending(inout vec4 dstColor, vec4 blendColor, float blendType)
{
    if (blendType == 0.0)
    {
        vec3 _85 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_85.x, _85.y, _85.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _97 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_97.x, _97.y, _97.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _110 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_110.x, _110.y, _110.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _123 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_123.x, _123.y, _123.z, dstColor.w);
                }
            }
        }
    }
}

vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    vec2 param_1 = advancedParam.UVDistortionUV;
    vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    vec4 Output = texture2D(Sampler_sampler_colorTex, Input.UV + UVOffset);
    Output.w *= Input.Color.w;
    vec4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_sampler_colorTex);
    Output = param_3;
    vec4 AlphaTexColor = texture2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_5 = advancedParam.BlendUVDistortionUV;
    vec2 param_6 = CBPS0.fUVDistortionParameter.zw;
    vec2 BlendUVOffset = UVDistortionOffset(param_5, param_6, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    vec4 BlendTextureColor = texture2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendAlphaTextureColor = texture2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_7;
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec2 pos = Input.PosP.xy / vec2(Input.PosP.w);
    vec2 posU = Input.Tangent.xy / vec2(Input.Tangent.w);
    vec2 posR = Input.Binormal.xy / vec2(Input.Binormal.w);
    float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    uv.y = 1.0 - uv.y;
    vec3 color = vec3(texture2D(Sampler_sampler_backTex, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Binormal = _VSPS_Binormal;
    Input.Tangent = _VSPS_Tangent;
    Input.PosP = _VSPS_PosP;
    Input.Color = _VSPS_Color;
    Input.Alpha_Dist_UV = _VSPS_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = _VSPS_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = _VSPS_Blend_FBNextIndex_UV;
    Input.Others = _VSPS_Others;
    vec4 _471 = _main(Input);
    gl_FragData[0] = _471;
}

