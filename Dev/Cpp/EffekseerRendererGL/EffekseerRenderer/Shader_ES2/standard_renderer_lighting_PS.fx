#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 Position;
    highp vec4 VColor;
    highp vec2 UV;
    highp vec3 WorldP;
    highp vec3 WorldN;
    highp vec3 WorldT;
    highp vec3 WorldB;
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
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_g_uvDistortionSampler;
uniform highp sampler2D Sampler_g_normalSampler;
uniform highp sampler2D Sampler_g_colorSampler;
uniform highp sampler2D Sampler_g_alphaSampler;
uniform highp sampler2D Sampler_g_blendUVDistortionSampler;
uniform highp sampler2D Sampler_g_blendSampler;
uniform highp sampler2D Sampler_g_blendAlphaSampler;

varying highp vec4 _VSPS_VColor;
varying highp vec2 _VSPS_UV;
varying highp vec3 _VSPS_WorldP;
varying highp vec3 _VSPS_WorldN;
varying highp vec3 _VSPS_WorldT;
varying highp vec3 _VSPS_WorldB;
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
        highp vec3 _77 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_77.x, _77.y, _77.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            highp vec3 _89 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_89.x, _89.y, _89.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                highp vec3 _102 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_102.x, _102.y, _102.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    highp vec3 _115 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_115.x, _115.y, _115.z, dstColor.w);
                }
            }
        }
    }
}

highp vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    highp vec2 UVOffset = vec2(0.0);
    UVOffset = (texture2D(Sampler_g_uvDistortionSampler, advancedParam.UVDistortionUV).xy * 2.0) - vec2(1.0);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    highp float diffuse = 1.0;
    highp vec3 loN = texture2D(Sampler_g_normalSampler, Input.UV + UVOffset).xyz;
    highp vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec4 Output = texture2D(Sampler_g_colorSampler, Input.UV + UVOffset) * Input.VColor;
    highp vec4 param_1 = Output;
    highp float param_2 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_1, CBPS0.fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, param_2, Sampler_g_colorSampler);
    Output = param_1;
    highp vec4 AlphaTexColor = texture2D(Sampler_g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    highp vec2 BlendUVOffset = (texture2D(Sampler_g_blendUVDistortionSampler, advancedParam.BlendUVDistortionUV).xy * 2.0) - vec2(1.0);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    highp vec4 BlendTextureColor = texture2D(Sampler_g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    highp vec4 BlendAlphaTextureColor = texture2D(Sampler_g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    highp vec4 param_3 = Output;
    ApplyTextureBlending(param_3, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_3;
    highp vec3 _347 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_347.x, _347.y, _347.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    highp vec3 _372 = Output.xyz * (vec3(diffuse, diffuse, diffuse) + vec3(CBPS0.fLightAmbient.xyz));
    Output = vec4(_372.x, _372.y, _372.z, Output.w);
    highp vec3 _393 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output = vec4(_393.x, _393.y, _393.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.VColor = _VSPS_VColor;
    Input.UV = _VSPS_UV;
    Input.WorldP = _VSPS_WorldP;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldT = _VSPS_WorldT;
    Input.WorldB = _VSPS_WorldB;
    Input.Alpha_Dist_UV = _VSPS_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = _VSPS_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = _VSPS_Blend_FBNextIndex_UV;
    Input.Others = _VSPS_Others;
    highp vec4 _439 = _main(Input);
    gl_FragData[0] = _439;
}

