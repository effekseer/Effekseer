#version 120
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_Input
{
    vec4 PosVS;
    vec4 Color;
    vec4 UV_Others;
    vec3 WorldN;
    vec3 WorldB;
    vec3 WorldT;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
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
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fCameraFrontDirection;
    vec4 fFalloffParameter;
    vec4 fFalloffBeginColor;
    vec4 fFalloffEndColor;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleParam;
    vec4 reconstructionParam1;
    vec4 reconstructionParam2;
    vec4 mUVInversedBack;
    vec4 miscFlags;
};

uniform PS_ConstanBuffer CBPS0;

uniform sampler2D Sampler_sampler_uvDistortionTex;
uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_normalTex;
uniform sampler2D Sampler_sampler_alphaTex;
uniform sampler2D Sampler_sampler_blendUVDistortionTex;
uniform sampler2D Sampler_sampler_blendTex;
uniform sampler2D Sampler_sampler_blendAlphaTex;

centroid varying vec4 _VSPS_Color;
centroid varying vec4 _VSPS_UV_Others;
varying vec3 _VSPS_WorldN;
varying vec3 _VSPS_WorldB;
varying vec3 _VSPS_WorldT;
varying vec4 _VSPS_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_Alpha_Dist_UV;
varying vec4 _VSPS_Blend_FBNextIndex_UV;

AdvancedParameter DisolveAdvancedParameter(PS_Input psinput)
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

vec2 UVDistortionOffset(vec2 uv, vec2 uvInversed, sampler2D SPIRV_Cross_Combinedts)
{
    vec2 UVOffset = (texture2D(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

vec3 PositivePow(vec3 base, vec3 power)
{
    return pow(max(abs(base), vec3(1.1920928955078125e-07)), power);
}

vec3 LinearToSRGB(vec3 c)
{
    vec3 param = c;
    vec3 param_1 = vec3(0.4166666567325592041015625);
    return max((PositivePow(param, param_1) * 1.05499994754791259765625) - vec3(0.054999999701976776123046875), vec3(0.0));
}

vec4 LinearToSRGB(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(LinearToSRGB(param), c.w);
}

vec4 ConvertFromSRGBTexture(vec4 c)
{
    if (CBPS0.miscFlags.x == 0.0)
    {
        return c;
    }
    vec4 param = c;
    return LinearToSRGB(param);
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
        vec3 _111 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_111.x, _111.y, _111.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _123 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_123.x, _123.y, _123.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _136 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_136.x, _136.y, _136.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _149 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_149.x, _149.y, _149.z, dstColor.w);
                }
            }
        }
    }
}

vec3 SRGBToLinear(vec3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625) + vec3(0.6821711063385009765625))) + vec3(0.01252287812530994415283203125)));
}

vec4 SRGBToLinear(vec4 c)
{
    vec3 param = c.xyz;
    return vec4(SRGBToLinear(param), c.w);
}

vec4 ConvertToScreen(vec4 c)
{
    if (CBPS0.miscFlags.x == 0.0)
    {
        return c;
    }
    vec4 param = c;
    return SRGBToLinear(param);
}

vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    vec2 param_1 = advancedParam.UVDistortionUV;
    vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    vec4 param_3 = texture2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    vec4 Output = ConvertFromSRGBTexture(param_3) * Input.Color;
    vec3 texNormal = (texture2D(Sampler_sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    vec4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, Sampler_sampler_colorTex);
    Output = param_4;
    vec4 AlphaTexColor = texture2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_6 = advancedParam.BlendUVDistortionUV;
    vec2 param_7 = CBPS0.fUVDistortionParameter.zw;
    vec2 BlendUVOffset = UVDistortionOffset(param_6, param_7, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    vec4 param_8 = texture2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendTextureColor = ConvertFromSRGBTexture(param_8);
    vec4 BlendAlphaTextureColor = texture2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_9;
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec3 _503 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_503.x, _503.y, _503.z, Output.w);
    if (CBPS0.fFalloffParameter.x == 1.0)
    {
        vec3 cameraVec = normalize(-CBPS0.fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        vec4 FalloffBlendColor = mix(CBPS0.fFalloffEndColor, CBPS0.fFalloffBeginColor, vec4(pow(CdotN, CBPS0.fFalloffParameter.z)));
        if (CBPS0.fFalloffParameter.y == 0.0)
        {
            vec3 _549 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_549.x, _549.y, _549.z, Output.w);
        }
        else
        {
            if (CBPS0.fFalloffParameter.y == 1.0)
            {
                vec3 _562 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_562.x, _562.y, _562.z, Output.w);
            }
            else
            {
                if (CBPS0.fFalloffParameter.y == 2.0)
                {
                    vec3 _575 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_575.x, _575.y, _575.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    vec3 _589 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_589.x, _589.y, _589.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _621 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output = vec4(_621.x, _621.y, _621.z, Output.w);
    vec4 param_10 = Output;
    return ConvertToScreen(param_10);
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV_Others = _VSPS_UV_Others;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldB = _VSPS_WorldB;
    Input.WorldT = _VSPS_WorldT;
    Input.Alpha_Dist_UV = _VSPS_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = _VSPS_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = _VSPS_Blend_FBNextIndex_UV;
    vec4 _662 = _main(Input);
    gl_FragData[0] = _662;
}

