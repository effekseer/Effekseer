#version 200 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec4 UV_Others;
    highp vec3 WorldN;
    highp vec4 Alpha_Dist_UV;
    highp vec4 Blend_Alpha_Dist_UV;
    highp vec4 Blend_FBNextIndex_UV;
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

struct PS_ConstantBuffer
{
    highp vec4 fLightDirection;
    highp vec4 fLightColor;
    highp vec4 fLightAmbient;
    highp vec4 fFlipbookParameter;
    highp vec4 fUVDistortionParameter;
    highp vec4 fBlendTextureParameter;
    highp vec4 fCameraFrontDirection;
    highp vec4 fFalloffParameter;
    highp vec4 fFalloffBeginColor;
    highp vec4 fFalloffEndColor;
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
    highp vec4 softParticleParam;
    highp vec4 reconstructionParam1;
    highp vec4 reconstructionParam2;
    highp vec4 mUVInversedBack;
    highp vec4 miscFlags;
};

uniform PS_ConstantBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_uvDistortionTex;
uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_alphaTex;
uniform highp sampler2D Sampler_sampler_blendUVDistortionTex;
uniform highp sampler2D Sampler_sampler_blendTex;
uniform highp sampler2D Sampler_sampler_blendAlphaTex;

centroid varying highp vec4 _VSPS_Color;
centroid varying highp vec4 _VSPS_UV_Others;
varying highp vec3 _VSPS_WorldN;
varying highp vec4 _VSPS_Alpha_Dist_UV;
varying highp vec4 _VSPS_Blend_Alpha_Dist_UV;
varying highp vec4 _VSPS_Blend_FBNextIndex_UV;

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

highp vec3 PositivePow(highp vec3 base, highp vec3 power)
{
    return pow(max(abs(base), vec3(1.1920928955078125e-07)), power);
}

highp vec3 LinearToSRGB(highp vec3 c)
{
    highp vec3 param = c;
    highp vec3 param_1 = vec3(0.4166666567325592041015625);
    return max((PositivePow(param, param_1) * 1.05499994754791259765625) - vec3(0.054999999701976776123046875), vec3(0.0));
}

highp vec4 LinearToSRGB(highp vec4 c)
{
    highp vec3 param = c.xyz;
    return vec4(LinearToSRGB(param), c.w);
}

highp vec4 ConvertFromSRGBTexture(highp vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    highp vec4 param = c;
    return LinearToSRGB(param);
}

highp vec2 UVDistortionOffset(highp vec2 uv, highp vec2 uvInversed, bool convertFromSRGB, highp sampler2D SPIRV_Cross_Combinedts)
{
    highp vec4 sampledColor = texture2D(SPIRV_Cross_Combinedts, uv);
    if (convertFromSRGB)
    {
        highp vec4 param = sampledColor;
        bool param_1 = convertFromSRGB;
        sampledColor = ConvertFromSRGBTexture(param, param_1);
    }
    highp vec2 UVOffset = (sampledColor.xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout highp vec4 dst, highp vec4 flipbookParameter, highp vec4 vcolor, highp vec2 nextUV, highp float flipbookRate, bool convertFromSRGB, highp sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        highp vec4 sampledColor = texture2D(SPIRV_Cross_Combinedts, nextUV);
        if (convertFromSRGB)
        {
            highp vec4 param = sampledColor;
            bool param_1 = convertFromSRGB;
            sampledColor = ConvertFromSRGBTexture(param, param_1);
        }
        highp vec4 NextPixelColor = sampledColor * vcolor;
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
        highp vec4 _211 = dstColor;
        highp vec3 _214 = (blendColor.xyz * blendColor.w) + (_211.xyz * (1.0 - blendColor.w));
        dstColor.x = _214.x;
        dstColor.y = _214.y;
        dstColor.z = _214.z;
    }
    else
    {
        if (blendType == 1.0)
        {
            highp vec4 _229 = dstColor;
            highp vec3 _231 = _229.xyz + (blendColor.xyz * blendColor.w);
            dstColor.x = _231.x;
            dstColor.y = _231.y;
            dstColor.z = _231.z;
        }
        else
        {
            if (blendType == 2.0)
            {
                highp vec4 _246 = dstColor;
                highp vec3 _248 = _246.xyz - (blendColor.xyz * blendColor.w);
                dstColor.x = _248.x;
                dstColor.y = _248.y;
                dstColor.z = _248.z;
            }
            else
            {
                if (blendType == 3.0)
                {
                    highp vec4 _263 = dstColor;
                    highp vec3 _265 = _263.xyz * (blendColor.xyz * blendColor.w);
                    dstColor.x = _265.x;
                    dstColor.y = _265.y;
                    dstColor.z = _265.z;
                }
            }
        }
    }
}

highp vec3 SRGBToLinear(highp vec3 c)
{
    return min(c, c * ((c * ((c * 0.305306017398834228515625) + vec3(0.6821711063385009765625))) + vec3(0.01252287812530994415283203125)));
}

highp vec4 SRGBToLinear(highp vec4 c)
{
    highp vec3 param = c.xyz;
    return vec4(SRGBToLinear(param), c.w);
}

highp vec4 ConvertToScreen(highp vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    highp vec4 param = c;
    return SRGBToLinear(param);
}

highp vec4 _main(PS_Input Input)
{
    bool convertColorSpace = CBPS0.miscFlags.x != 0.0;
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    highp vec2 param_1 = advancedParam.UVDistortionUV;
    highp vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    bool param_3 = convertColorSpace;
    highp vec2 UVOffset = UVDistortionOffset(param_1, param_2, param_3, Sampler_sampler_uvDistortionTex);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    highp vec4 param_4 = texture2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    bool param_5 = convertColorSpace;
    highp vec4 Output = ConvertFromSRGBTexture(param_4, param_5) * Input.Color;
    highp vec4 param_6 = Output;
    highp float param_7 = advancedParam.FlipbookRate;
    bool param_8 = convertColorSpace;
    ApplyFlipbook(param_6, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_7, param_8, Sampler_sampler_colorTex);
    Output = param_6;
    highp vec4 param_9 = texture2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    bool param_10 = convertColorSpace;
    highp vec4 AlphaTexColor = ConvertFromSRGBTexture(param_9, param_10);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    highp vec2 param_11 = advancedParam.BlendUVDistortionUV;
    highp vec2 param_12 = CBPS0.fUVDistortionParameter.zw;
    bool param_13 = convertColorSpace;
    highp vec2 BlendUVOffset = UVDistortionOffset(param_11, param_12, param_13, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    highp vec4 param_14 = texture2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    bool param_15 = convertColorSpace;
    highp vec4 BlendTextureColor = ConvertFromSRGBTexture(param_14, param_15);
    highp vec4 param_16 = texture2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    bool param_17 = convertColorSpace;
    highp vec4 BlendAlphaTextureColor = ConvertFromSRGBTexture(param_16, param_17);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    highp vec4 param_18 = Output;
    ApplyTextureBlending(param_18, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_18;
    if (CBPS0.fFalloffParameter.x == 1.0)
    {
        highp vec3 cameraVec = normalize(-CBPS0.fCameraFrontDirection.xyz);
        highp float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0, 1.0);
        highp vec4 FalloffBlendColor = mix(CBPS0.fFalloffEndColor, CBPS0.fFalloffBeginColor, vec4(pow(CdotN, CBPS0.fFalloffParameter.z)));
        if (CBPS0.fFalloffParameter.y == 0.0)
        {
            highp vec4 _546 = Output;
            highp vec3 _548 = _546.xyz + FalloffBlendColor.xyz;
            Output.x = _548.x;
            Output.y = _548.y;
            Output.z = _548.z;
        }
        else
        {
            if (CBPS0.fFalloffParameter.y == 1.0)
            {
                highp vec4 _563 = Output;
                highp vec3 _565 = _563.xyz - FalloffBlendColor.xyz;
                Output.x = _565.x;
                Output.y = _565.y;
                Output.z = _565.z;
            }
            else
            {
                if (CBPS0.fFalloffParameter.y == 2.0)
                {
                    highp vec4 _580 = Output;
                    highp vec3 _582 = _580.xyz * FalloffBlendColor.xyz;
                    Output.x = _582.x;
                    Output.y = _582.y;
                    Output.z = _582.z;
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    highp vec4 _598 = Output;
    highp vec3 _600 = _598.xyz * CBPS0.fEmissiveScaling.x;
    Output.x = _600.x;
    Output.y = _600.y;
    Output.z = _600.z;
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    highp vec4 _624 = Output;
    highp float _627 = Output.w;
    highp vec3 _636 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, _624.xyz, vec3(ceil((_627 - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output.x = _636.x;
    Output.y = _636.y;
    Output.z = _636.z;
    highp vec4 param_19 = Output;
    bool param_20 = convertColorSpace;
    return ConvertToScreen(param_19, param_20);
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = _VSPS_Color;
    Input.UV_Others = _VSPS_UV_Others;
    Input.WorldN = _VSPS_WorldN;
    Input.Alpha_Dist_UV = _VSPS_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = _VSPS_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = _VSPS_Blend_FBNextIndex_UV;
    highp vec4 _677 = _main(Input);
    gl_FragData[0] = _677;
}

