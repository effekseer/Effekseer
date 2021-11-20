#version 420

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

layout(set = 1, binding = 0, std140) uniform PS_ConstanBuffer
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
} _366;

layout(set = 1, binding = 4) uniform sampler2D Sampler_sampler_uvDistortionTex;
layout(set = 1, binding = 1) uniform sampler2D Sampler_sampler_colorTex;
layout(set = 1, binding = 2) uniform sampler2D Sampler_sampler_normalTex;
layout(set = 1, binding = 3) uniform sampler2D Sampler_sampler_alphaTex;
layout(set = 1, binding = 7) uniform sampler2D Sampler_sampler_blendUVDistortionTex;
layout(set = 1, binding = 5) uniform sampler2D Sampler_sampler_blendTex;
layout(set = 1, binding = 6) uniform sampler2D Sampler_sampler_blendAlphaTex;
layout(set = 1, binding = 8) uniform sampler2D Sampler_sampler_depthTex;

layout(location = 0) centroid in vec4 Input_Color;
layout(location = 1) centroid in vec4 Input_UV_Others;
layout(location = 2) in vec3 Input_WorldN;
layout(location = 3) in vec3 Input_WorldB;
layout(location = 4) in vec3 Input_WorldT;
layout(location = 5) in vec4 Input_Alpha_Dist_UV;
layout(location = 6) in vec4 Input_Blend_Alpha_Dist_UV;
layout(location = 7) in vec4 Input_Blend_FBNextIndex_UV;
layout(location = 8) in vec4 Input_PosP;
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
    if (_366.miscFlags.x == 0.0)
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
    vec2 param_2 = _366.fUVDistortionParameter.zw;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= _366.fUVDistortionParameter.x;
    vec4 param_3 = texture(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    vec4 Output = LinearToSRGB(param_3) * Input.Color;
    vec3 texNormal = (texture(Sampler_sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    vec4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, _366.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, Sampler_sampler_colorTex);
    Output = param_4;
    vec4 AlphaTexColor = texture(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_6 = advancedParam.BlendUVDistortionUV;
    vec2 param_7 = _366.fUVDistortionParameter.zw;
    vec2 BlendUVOffset = UVDistortionOffset(param_6, param_7, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _366.fUVDistortionParameter.y;
    vec4 param_8 = texture(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendTextureColor = LinearToSRGB(param_8);
    vec4 BlendAlphaTextureColor = texture(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, _366.fBlendTextureParameter.x);
    Output = param_9;
    float diffuse = max(dot(_366.fLightDirection.xyz, localNormal), 0.0);
    vec3 _567 = Output.xyz * ((_366.fLightColor.xyz * diffuse) + _366.fLightAmbient.xyz);
    Output = vec4(_567.x, _567.y, _567.z, Output.w);
    if (_366.fFalloffParameter.x == 1.0)
    {
        vec3 cameraVec = normalize(-_366.fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        vec4 FalloffBlendColor = mix(_366.fFalloffEndColor, _366.fFalloffBeginColor, vec4(pow(CdotN, _366.fFalloffParameter.z)));
        if (_366.fFalloffParameter.y == 0.0)
        {
            vec3 _613 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_613.x, _613.y, _613.z, Output.w);
        }
        else
        {
            if (_366.fFalloffParameter.y == 1.0)
            {
                vec3 _626 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_626.x, _626.y, _626.z, Output.w);
            }
            else
            {
                if (_366.fFalloffParameter.y == 2.0)
                {
                    vec3 _639 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_639.x, _639.y, _639.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    vec3 _653 = Output.xyz * _366.fEmissiveScaling.x;
    Output = vec4(_653.x, _653.y, _653.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = _366.mUVInversedBack.x + (_366.mUVInversedBack.y * screenUV.y);
    if (!(_366.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param_10 = backgroundZ;
        float param_11 = screenPos.z;
        vec4 param_12 = _366.softParticleParam;
        vec4 param_13 = _366.reconstructionParam1;
        vec4 param_14 = _366.reconstructionParam2;
        Output.w *= SoftParticle(param_10, param_11, param_12, param_13, param_14);
    }
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _747 = mix(_366.fEdgeColor.xyz * _366.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - _366.fEdgeParameter.x)));
    Output = vec4(_747.x, _747.y, _747.z, Output.w);
    vec4 param_15 = Output;
    return ConvertToScreen(param_15);
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV_Others = Input_UV_Others;
    Input.WorldN = Input_WorldN;
    Input.WorldB = Input_WorldB;
    Input.WorldT = Input_WorldT;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.PosP = Input_PosP;
    vec4 _791 = _main(Input);
    _entryPointOutput = _791;
}

