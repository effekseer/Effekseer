#if !defined(__EMSCRIPTEN__)
static const char ad_model_lit_ps_gl2[] = R"(#version 120
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

varying vec4 _VSPS_Color;
varying vec4 _VSPS_UV_Others;
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
        vec3 _108 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_108.x, _108.y, _108.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _120 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_120.x, _120.y, _120.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _133 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_133.x, _133.y, _133.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _146 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_146.x, _146.y, _146.z, dstColor.w);
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
    vec4 Output = LinearToSRGB(param_3) * Input.Color;
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
    vec4 BlendTextureColor = LinearToSRGB(param_8);
    vec4 BlendAlphaTextureColor = texture2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_9;
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec3 _488 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_488.x, _488.y, _488.z, Output.w);
    if (CBPS0.fFalloffParameter.x == 1.0)
    {
        vec3 cameraVec = normalize(-CBPS0.fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        vec4 FalloffBlendColor = mix(CBPS0.fFalloffEndColor, CBPS0.fFalloffBeginColor, vec4(pow(CdotN, CBPS0.fFalloffParameter.z)));
        if (CBPS0.fFalloffParameter.y == 0.0)
        {
            vec3 _534 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_534.x, _534.y, _534.z, Output.w);
        }
        else
        {
            if (CBPS0.fFalloffParameter.y == 1.0)
            {
                vec3 _547 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_547.x, _547.y, _547.z, Output.w);
            }
            else
            {
                if (CBPS0.fFalloffParameter.y == 2.0)
                {
                    vec3 _560 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_560.x, _560.y, _560.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    vec3 _574 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_574.x, _574.y, _574.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _606 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output = vec4(_606.x, _606.y, _606.z, Output.w);
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
    vec4 _647 = _main(Input);
    gl_FragData[0] = _647;
}

)";

static const char ad_model_lit_ps_gl3[] = R"(#version 330
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
    vec4 PosP;
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
uniform sampler2D Sampler_sampler_depthTex;

centroid in vec4 _VSPS_Color;
centroid in vec4 _VSPS_UV_Others;
in vec3 _VSPS_WorldN;
in vec3 _VSPS_WorldB;
in vec3 _VSPS_WorldT;
in vec4 _VSPS_Alpha_Dist_UV;
in vec4 _VSPS_Blend_Alpha_Dist_UV;
in vec4 _VSPS_Blend_FBNextIndex_UV;
in vec4 _VSPS_PosP;
layout(location = 0) out vec4 _entryPointOutput;

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
    vec2 UVOffset = (texture(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
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

void ApplyFlipbook(inout vec4 dst, vec4 flipbookParameter, vec4 vcolor, vec2 nextUV, float flipbookRate, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        vec4 NextPixelColor = texture(SPIRV_Cross_Combinedts, nextUV) * vcolor;
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
        vec3 _116 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_116.x, _116.y, _116.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _128 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_128.x, _128.y, _128.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _141 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_141.x, _141.y, _141.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _154 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_154.x, _154.y, _154.z, dstColor.w);
                }
            }
        }
    }
}

float SoftParticle(float backgroundZ, float meshZ, vec4 softparticleParam, vec4 reconstruct1, vec4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    vec2 rescale = reconstruct1.xy;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    float dir = sign(depth.x);
    depth *= dir;
    float alphaFar = (depth.x - depth.y) / distanceFar;
    float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
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
    vec4 param_3 = texture(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    vec4 Output = LinearToSRGB(param_3) * Input.Color;
    vec3 texNormal = (texture(Sampler_sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    vec4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, Sampler_sampler_colorTex);
    Output = param_4;
    vec4 AlphaTexColor = texture(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_6 = advancedParam.BlendUVDistortionUV;
    vec2 param_7 = CBPS0.fUVDistortionParameter.zw;
    vec2 BlendUVOffset = UVDistortionOffset(param_6, param_7, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    vec4 param_8 = texture(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendTextureColor = LinearToSRGB(param_8);
    vec4 BlendAlphaTextureColor = texture(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_9;
    float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    vec3 _567 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_567.x, _567.y, _567.z, Output.w);
    if (CBPS0.fFalloffParameter.x == 1.0)
    {
        vec3 cameraVec = normalize(-CBPS0.fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        vec4 FalloffBlendColor = mix(CBPS0.fFalloffEndColor, CBPS0.fFalloffBeginColor, vec4(pow(CdotN, CBPS0.fFalloffParameter.z)));
        if (CBPS0.fFalloffParameter.y == 0.0)
        {
            vec3 _613 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_613.x, _613.y, _613.z, Output.w);
        }
        else
        {
            if (CBPS0.fFalloffParameter.y == 1.0)
            {
                vec3 _626 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_626.x, _626.y, _626.z, Output.w);
            }
            else
            {
                if (CBPS0.fFalloffParameter.y == 2.0)
                {
                    vec3 _639 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_639.x, _639.y, _639.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    vec3 _653 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_653.x, _653.y, _653.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * screenUV.y);
    if (!(CBPS0.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param_10 = backgroundZ;
        float param_11 = screenPos.z;
        vec4 param_12 = CBPS0.softParticleParam;
        vec4 param_13 = CBPS0.reconstructionParam1;
        vec4 param_14 = CBPS0.reconstructionParam2;
        Output.w *= SoftParticle(param_10, param_11, param_12, param_13, param_14);
    }
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _751 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output = vec4(_751.x, _751.y, _751.z, Output.w);
    vec4 param_15 = Output;
    return ConvertToScreen(param_15);
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
    Input.PosP = _VSPS_PosP;
    vec4 _795 = _main(Input);
    _entryPointOutput = _795;
}

)";

#endif

static const char ad_model_lit_ps_gles2[] = R"(
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec4 UV_Others;
    highp vec3 WorldN;
    highp vec3 WorldB;
    highp vec3 WorldT;
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

struct PS_ConstanBuffer
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

uniform PS_ConstanBuffer CBPS0;

uniform  sampler2D Sampler_sampler_uvDistortionTex;
uniform  sampler2D Sampler_sampler_colorTex;
uniform  sampler2D Sampler_sampler_normalTex;
uniform  sampler2D Sampler_sampler_alphaTex;
uniform  sampler2D Sampler_sampler_blendUVDistortionTex;
uniform  sampler2D Sampler_sampler_blendTex;
uniform  sampler2D Sampler_sampler_blendAlphaTex;

varying  vec4 _VSPS_Color;
varying  vec4 _VSPS_UV_Others;
varying  vec3 _VSPS_WorldN;
varying  vec3 _VSPS_WorldB;
varying  vec3 _VSPS_WorldT;
varying  vec4 _VSPS_Alpha_Dist_UV;
varying  vec4 _VSPS_Blend_Alpha_Dist_UV;
varying  vec4 _VSPS_Blend_FBNextIndex_UV;

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

highp vec2 UVDistortionOffset(highp vec2 uv, highp vec2 uvInversed, highp sampler2D SPIRV_Cross_Combinedts)
{
    highp vec2 UVOffset = (texture2D(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
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
        highp vec3 _108 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_108.x, _108.y, _108.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            highp vec3 _120 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_120.x, _120.y, _120.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                highp vec3 _133 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_133.x, _133.y, _133.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    highp vec3 _146 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_146.x, _146.y, _146.z, dstColor.w);
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

highp vec4 ConvertToScreen(highp vec4 c)
{
    if (CBPS0.miscFlags.x == 0.0)
    {
        return c;
    }
    highp vec4 param = c;
    return SRGBToLinear(param);
}

highp vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    highp vec2 param_1 = advancedParam.UVDistortionUV;
    highp vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    highp vec4 param_3 = texture2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    highp vec4 Output = LinearToSRGB(param_3) * Input.Color;
    highp vec3 texNormal = (texture2D(Sampler_sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp vec4 param_4 = Output;
    highp float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, Sampler_sampler_colorTex);
    Output = param_4;
    highp vec4 AlphaTexColor = texture2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    highp vec2 param_6 = advancedParam.BlendUVDistortionUV;
    highp vec2 param_7 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 BlendUVOffset = UVDistortionOffset(param_6, param_7, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    highp vec4 param_8 = texture2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    highp vec4 BlendTextureColor = LinearToSRGB(param_8);
    highp vec4 BlendAlphaTextureColor = texture2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    highp vec4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_9;
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec3 _488 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_488.x, _488.y, _488.z, Output.w);
    if (CBPS0.fFalloffParameter.x == 1.0)
    {
        highp vec3 cameraVec = normalize(-CBPS0.fCameraFrontDirection.xyz);
        highp float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        highp vec4 FalloffBlendColor = mix(CBPS0.fFalloffEndColor, CBPS0.fFalloffBeginColor, vec4(pow(CdotN, CBPS0.fFalloffParameter.z)));
        if (CBPS0.fFalloffParameter.y == 0.0)
        {
            highp vec3 _534 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_534.x, _534.y, _534.z, Output.w);
        }
        else
        {
            if (CBPS0.fFalloffParameter.y == 1.0)
            {
                highp vec3 _547 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_547.x, _547.y, _547.z, Output.w);
            }
            else
            {
                if (CBPS0.fFalloffParameter.y == 2.0)
                {
                    highp vec3 _560 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_560.x, _560.y, _560.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    highp vec3 _574 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_574.x, _574.y, _574.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    highp vec3 _606 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output = vec4(_606.x, _606.y, _606.z, Output.w);
    highp vec4 param_10 = Output;
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
    highp vec4 _647 = _main(Input);
    gl_FragData[0] = _647;
}

)";

static const char ad_model_lit_ps_gles3[] = R"(#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 Color;
    highp vec4 UV_Others;
    highp vec3 WorldN;
    highp vec3 WorldB;
    highp vec3 WorldT;
    highp vec4 Alpha_Dist_UV;
    highp vec4 Blend_Alpha_Dist_UV;
    highp vec4 Blend_FBNextIndex_UV;
    highp vec4 PosP;
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

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_uvDistortionTex;
uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_normalTex;
uniform highp sampler2D Sampler_sampler_alphaTex;
uniform highp sampler2D Sampler_sampler_blendUVDistortionTex;
uniform highp sampler2D Sampler_sampler_blendTex;
uniform highp sampler2D Sampler_sampler_blendAlphaTex;
uniform highp sampler2D Sampler_sampler_depthTex;

centroid in highp vec4 _VSPS_Color;
centroid in highp vec4 _VSPS_UV_Others;
in highp vec3 _VSPS_WorldN;
in highp vec3 _VSPS_WorldB;
in highp vec3 _VSPS_WorldT;
in highp vec4 _VSPS_Alpha_Dist_UV;
in highp vec4 _VSPS_Blend_Alpha_Dist_UV;
in highp vec4 _VSPS_Blend_FBNextIndex_UV;
in highp vec4 _VSPS_PosP;
layout(location = 0) out highp vec4 _entryPointOutput;

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

highp vec2 UVDistortionOffset(highp vec2 uv, highp vec2 uvInversed, highp sampler2D SPIRV_Cross_Combinedts)
{
    highp vec2 UVOffset = (texture(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
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

void ApplyFlipbook(inout highp vec4 dst, highp vec4 flipbookParameter, highp vec4 vcolor, highp vec2 nextUV, highp float flipbookRate, highp sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0)
    {
        highp vec4 NextPixelColor = texture(SPIRV_Cross_Combinedts, nextUV) * vcolor;
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
        highp vec3 _116 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_116.x, _116.y, _116.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            highp vec3 _128 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_128.x, _128.y, _128.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                highp vec3 _141 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_141.x, _141.y, _141.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    highp vec3 _154 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_154.x, _154.y, _154.z, dstColor.w);
                }
            }
        }
    }
}

highp float SoftParticle(highp float backgroundZ, highp float meshZ, highp vec4 softparticleParam, highp vec4 reconstruct1, highp vec4 reconstruct2)
{
    highp float distanceFar = softparticleParam.x;
    highp float distanceNear = softparticleParam.y;
    highp float distanceNearOffset = softparticleParam.z;
    highp vec2 rescale = reconstruct1.xy;
    highp vec4 params = reconstruct2;
    highp vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    highp vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    highp float dir = sign(depth.x);
    depth *= dir;
    highp float alphaFar = (depth.x - depth.y) / distanceFar;
    highp float alphaNear = (depth.y - distanceNearOffset) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
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

highp vec4 ConvertToScreen(highp vec4 c)
{
    if (CBPS0.miscFlags.x == 0.0)
    {
        return c;
    }
    highp vec4 param = c;
    return SRGBToLinear(param);
}

highp vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    highp vec2 param_1 = advancedParam.UVDistortionUV;
    highp vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    highp vec4 param_3 = texture(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    highp vec4 Output = LinearToSRGB(param_3) * Input.Color;
    highp vec3 texNormal = (texture(Sampler_sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    highp vec4 param_4 = Output;
    highp float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, CBPS0.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, Sampler_sampler_colorTex);
    Output = param_4;
    highp vec4 AlphaTexColor = texture(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    highp vec2 param_6 = advancedParam.BlendUVDistortionUV;
    highp vec2 param_7 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 BlendUVOffset = UVDistortionOffset(param_6, param_7, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    highp vec4 param_8 = texture(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    highp vec4 BlendTextureColor = LinearToSRGB(param_8);
    highp vec4 BlendAlphaTextureColor = texture(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    highp vec4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_9;
    highp float diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec3 _567 = Output.xyz * ((CBPS0.fLightColor.xyz * diffuse) + CBPS0.fLightAmbient.xyz);
    Output = vec4(_567.x, _567.y, _567.z, Output.w);
    if (CBPS0.fFalloffParameter.x == 1.0)
    {
        highp vec3 cameraVec = normalize(-CBPS0.fCameraFrontDirection.xyz);
        highp float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        highp vec4 FalloffBlendColor = mix(CBPS0.fFalloffEndColor, CBPS0.fFalloffBeginColor, vec4(pow(CdotN, CBPS0.fFalloffParameter.z)));
        if (CBPS0.fFalloffParameter.y == 0.0)
        {
            highp vec3 _613 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_613.x, _613.y, _613.z, Output.w);
        }
        else
        {
            if (CBPS0.fFalloffParameter.y == 1.0)
            {
                highp vec3 _626 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_626.x, _626.y, _626.z, Output.w);
            }
            else
            {
                if (CBPS0.fFalloffParameter.y == 2.0)
                {
                    highp vec3 _639 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_639.x, _639.y, _639.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    highp vec3 _653 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_653.x, _653.y, _653.z, Output.w);
    highp vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    highp vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = CBPS0.mUVInversedBack.x + (CBPS0.mUVInversedBack.y * screenUV.y);
    if (!(CBPS0.softParticleParam.w == 0.0))
    {
        highp float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        highp float param_10 = backgroundZ;
        highp float param_11 = screenPos.z;
        highp vec4 param_12 = CBPS0.softParticleParam;
        highp vec4 param_13 = CBPS0.reconstructionParam1;
        highp vec4 param_14 = CBPS0.reconstructionParam2;
        Output.w *= SoftParticle(param_10, param_11, param_12, param_13, param_14);
    }
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    highp vec3 _751 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output = vec4(_751.x, _751.y, _751.z, Output.w);
    highp vec4 param_15 = Output;
    return ConvertToScreen(param_15);
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
    Input.PosP = _VSPS_PosP;
    highp vec4 _795 = _main(Input);
    _entryPointOutput = _795;
}

)";


    static const char* get_ad_model_lit_ps (EffekseerRendererGL::OpenGLDeviceType deviceType)
    {
    #if !defined(__EMSCRIPTEN__)
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL3)
            return ad_model_lit_ps_gl3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGL2)
            return ad_model_lit_ps_gl2;
    #endif
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES3)
            return ad_model_lit_ps_gles3;
        if (deviceType == EffekseerRendererGL::OpenGLDeviceType::OpenGLES2)
            return ad_model_lit_ps_gles2;
        return nullptr;
    }
    