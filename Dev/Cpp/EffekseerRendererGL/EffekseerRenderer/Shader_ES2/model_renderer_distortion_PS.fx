#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Position;
    highp vec2 UV;
    highp vec4 Binormal;
    highp vec4 Tangent;
    highp vec4 Pos;
    highp vec4 Color;
    highp vec4 Alpha_Dist_UV;
    highp vec4 Blend_Alpha_Dist_UV;
    highp vec4 Blend_FBNextIndex_UV;
    highp vec2 Others;
};

struct AdvancedParameter
{
    highp vec2 AlphaUV;
    highp vec2 UVDistortionUV;
    highp vec2 BlendUV;
    highp vec2 BlendAlphaUV;
    highp vec2 BlendUVDistortionUV;
    highp vec2 FlipbookNextIndexUV;
    highp float FlipbookRate;
    highp float AlphaThreshold;
};

struct PS_ConstanBuffer
{
    highp vec4 g_scale;
    highp vec4 mUVInversedBack;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_g_uvDistortionSampler;
uniform highp sampler2D Sampler_g_sampler;
uniform highp sampler2D Sampler_g_alphaSampler;
uniform highp sampler2D Sampler_g_blendUVDistortionSampler;
uniform highp sampler2D Sampler_g_blendSampler;
uniform highp sampler2D Sampler_g_blendAlphaSampler;
uniform highp sampler2D Sampler_g_backSampler;

varying highp vec2 _VSPS_UV;
varying highp vec4 _VSPS_Binormal;
varying highp vec4 _VSPS_Tangent;
varying highp vec4 _VSPS_Pos;
varying highp vec4 _VSPS_Color;
varying highp vec4 _VSPS_Alpha_Dist_UV;
varying highp vec4 _VSPS_Blend_Alpha_Dist_UV;
varying highp vec4 _VSPS_Blend_FBNextIndex_UV;
varying highp vec2 _VSPS_Others;

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

highp vec2 UVDistortionOffset(highp vec2 uv, highp vec2 uvInversed, highp sampler2D SPIRV_Cross_Combinedts)
{
    highp vec2 UVOffset = (texture2D(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout highp vec4 dst, highp vec4 flipbookParameter, highp vec4 vcolor, highp vec2 nextUV, highp float flipbookRate, highp sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        highp vec4 NextPixelColor = texture2D(SPIRV_Cross_Combinedts, nextUV) * vcolor;
        if (flipbookParameter.y == 1.0)
        {
            dst = mix(dst, NextPixelColor, vec4(flipbookRate));
        }
    }
}

void ApplyTextureBlending(inout highp vec4 dstColor, highp vec4 blendColor, highp float blendType)
{
    if (blendType == 0.0)
    {
        highp vec3 _85 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_85.x, _85.y, _85.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            highp vec3 _97 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_97.x, _97.y, _97.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                highp vec3 _110 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_110.x, _110.y, _110.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    highp vec3 _123 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_123.x, _123.y, _123.z, dstColor.w);
                }
            }
        }
    }
}

highp vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    highp vec2 param_1 = advancedParam.UVDistortionUV;
    highp vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_g_uvDistortionSampler);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    highp vec4 Output = texture2D(Sampler_g_sampler, Input.UV + UVOffset);
    Output.w *= Input.Color.w;
    highp vec4 param_3 = Output;
    highp float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_g_sampler);
    Output = param_3;
    highp vec4 AlphaTexColor = texture2D(Sampler_g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    highp vec2 param_5 = advancedParam.BlendUVDistortionUV;
    highp vec2 param_6 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 BlendUVOffset = UVDistortionOffset(param_5, param_6, Sampler_g_blendUVDistortionSampler);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    highp vec4 BlendTextureColor = texture2D(Sampler_g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    highp vec4 BlendAlphaTextureColor = texture2D(Sampler_g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    highp vec4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_7;
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    highp vec2 pos = Input.Pos.xy / vec2(Input.Pos.w);
    highp vec2 posU = Input.Tangent.xy / vec2(Input.Tangent.w);
    highp vec2 posR = Input.Binormal.xy / vec2(Input.Binormal.w);
    highp float xscale = (((Output.x * 2.0) - 1.0) * Input.Color.x) * CBPS0.g_scale.x;
    highp float yscale = (((Output.y * 2.0) - 1.0) * Input.Color.y) * CBPS0.g_scale.x;
    highp vec2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0) * 0.5;
    uv.y = 1.0 - ((uv.y + 1.0) * 0.5);
    uv.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * uv.y);
    highp vec3 color = vec3(texture2D(Sampler_g_backSampler, uv).xyz);
    Output = vec4(color.x, color.y, color.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.UV = _VSPS_UV;
    Input.Binormal = _VSPS_Binormal;
    Input.Tangent = _VSPS_Tangent;
    Input.Pos = _VSPS_Pos;
    Input.Color = _VSPS_Color;
    Input.Alpha_Dist_UV = _VSPS_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = _VSPS_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = _VSPS_Blend_FBNextIndex_UV;
    Input.Others = _VSPS_Others;
    highp vec4 _467 = _main(Input);
    gl_FragData[0] = _467;
}

