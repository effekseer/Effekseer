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

struct PS_ConstantBuffer
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

uniform PS_ConstantBuffer CBPS0;

uniform sampler2D Sampler_sampler_uvDistortionTex;
uniform sampler2D Sampler_sampler_colorTex;
uniform sampler2D Sampler_sampler_alphaTex;
uniform sampler2D Sampler_sampler_blendUVDistortionTex;
uniform sampler2D Sampler_sampler_blendTex;
uniform sampler2D Sampler_sampler_blendAlphaTex;

centroid varying vec4 _VSPS_Color;
centroid varying vec4 _VSPS_UV_Others;
varying vec3 _VSPS_WorldN;
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

vec4 ConvertFromSRGBTexture(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return LinearToSRGB(param);
}

vec2 UVDistortionOffset(vec2 uv, vec2 uvInversed, bool convertFromSRGB, sampler2D SPIRV_Cross_Combinedts)
{
    vec4 sampledColor = texture2D(SPIRV_Cross_Combinedts, uv);
    if (convertFromSRGB)
    {
        vec4 param = sampledColor;
        bool param_1 = convertFromSRGB;
        sampledColor = ConvertFromSRGBTexture(param, param_1);
    }
    vec2 UVOffset = (sampledColor.xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout vec4 dst, vec4 flipbookParameter, vec4 vcolor, vec2 nextUV, float flipbookRate, bool convertFromSRGB, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        vec4 sampledColor = texture2D(SPIRV_Cross_Combinedts, nextUV);
        if (convertFromSRGB)
        {
            vec4 param = sampledColor;
            bool param_1 = convertFromSRGB;
            sampledColor = ConvertFromSRGBTexture(param, param_1);
        }
        vec4 NextPixelColor = sampledColor * vcolor;
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
        vec4 _211 = dstColor;
        vec3 _214 = (blendColor.xyz * blendColor.w) + (_211.xyz * (1.0 - blendColor.w));
        dstColor.x = _214.x;
        dstColor.y = _214.y;
        dstColor.z = _214.z;
    }
    else
    {
        if (blendType == 1.0)
        {
            vec4 _229 = dstColor;
            vec3 _231 = _229.xyz + (blendColor.xyz * blendColor.w);
            dstColor.x = _231.x;
            dstColor.y = _231.y;
            dstColor.z = _231.z;
        }
        else
        {
            if (blendType == 2.0)
            {
                vec4 _246 = dstColor;
                vec3 _248 = _246.xyz - (blendColor.xyz * blendColor.w);
                dstColor.x = _248.x;
                dstColor.y = _248.y;
                dstColor.z = _248.z;
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec4 _263 = dstColor;
                    vec3 _265 = _263.xyz * (blendColor.xyz * blendColor.w);
                    dstColor.x = _265.x;
                    dstColor.y = _265.y;
                    dstColor.z = _265.z;
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

vec4 ConvertToScreen(vec4 c, bool isValid)
{
    if (!isValid)
    {
        return c;
    }
    vec4 param = c;
    return SRGBToLinear(param);
}

vec4 _main(PS_Input Input)
{
    bool convertColorSpace = CBPS0.miscFlags.x != 0.0;
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    vec2 param_1 = advancedParam.UVDistortionUV;
    vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    bool param_3 = convertColorSpace;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, param_3, Sampler_sampler_uvDistortionTex);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    vec4 param_4 = texture2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    bool param_5 = convertColorSpace;
    vec4 Output = ConvertFromSRGBTexture(param_4, param_5) * Input.Color;
    vec4 param_6 = Output;
    float param_7 = advancedParam.FlipbookRate;
    bool param_8 = convertColorSpace;
    ApplyFlipbook(param_6, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_7, param_8, Sampler_sampler_colorTex);
    Output = param_6;
    vec4 param_9 = texture2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    bool param_10 = convertColorSpace;
    vec4 AlphaTexColor = ConvertFromSRGBTexture(param_9, param_10);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_11 = advancedParam.BlendUVDistortionUV;
    vec2 param_12 = CBPS0.fUVDistortionParameter.zw;
    bool param_13 = convertColorSpace;
    vec2 BlendUVOffset = UVDistortionOffset(param_11, param_12, param_13, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    vec4 param_14 = texture2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    bool param_15 = convertColorSpace;
    vec4 BlendTextureColor = ConvertFromSRGBTexture(param_14, param_15);
    vec4 param_16 = texture2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    bool param_17 = convertColorSpace;
    vec4 BlendAlphaTextureColor = ConvertFromSRGBTexture(param_16, param_17);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_18 = Output;
    ApplyTextureBlending(param_18, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_18;
    if (CBPS0.fFalloffParameter.x == 1.0)
    {
        vec3 cameraVec = normalize(-CBPS0.fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0, 1.0);
        vec4 FalloffBlendColor = mix(CBPS0.fFalloffEndColor, CBPS0.fFalloffBeginColor, vec4(pow(CdotN, CBPS0.fFalloffParameter.z)));
        if (CBPS0.fFalloffParameter.y == 0.0)
        {
            vec4 _546 = Output;
            vec3 _548 = _546.xyz + FalloffBlendColor.xyz;
            Output.x = _548.x;
            Output.y = _548.y;
            Output.z = _548.z;
        }
        else
        {
            if (CBPS0.fFalloffParameter.y == 1.0)
            {
                vec4 _563 = Output;
                vec3 _565 = _563.xyz - FalloffBlendColor.xyz;
                Output.x = _565.x;
                Output.y = _565.y;
                Output.z = _565.z;
            }
            else
            {
                if (CBPS0.fFalloffParameter.y == 2.0)
                {
                    vec4 _580 = Output;
                    vec3 _582 = _580.xyz * FalloffBlendColor.xyz;
                    Output.x = _582.x;
                    Output.y = _582.y;
                    Output.z = _582.z;
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    vec4 _598 = Output;
    vec3 _600 = _598.xyz * CBPS0.fEmissiveScaling.x;
    Output.x = _600.x;
    Output.y = _600.y;
    Output.z = _600.z;
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec4 _624 = Output;
    float _627 = Output.w;
    vec3 _636 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, _624.xyz, vec3(ceil((_627 - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output.x = _636.x;
    Output.y = _636.y;
    Output.z = _636.z;
    vec4 param_19 = Output;
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
    vec4 _677 = _main(Input);
    gl_FragData[0] = _677;
}

