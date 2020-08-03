#version 420

struct PS_Input
{
    vec4 Pos;
    vec2 UV;
    vec3 Normal;
    vec3 Binormal;
    vec3 Tangent;
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

struct FalloffParameter
{
    vec4 Param;
    vec4 BeginColor;
    vec4 EndColor;
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
    FalloffParameter fFalloffParam;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
} _187;

layout(set = 1, binding = 4) uniform sampler2D Sampler_g_uvDistortionSampler;
layout(set = 1, binding = 1) uniform sampler2D Sampler_g_colorSampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_normalSampler;
layout(set = 1, binding = 3) uniform sampler2D Sampler_g_alphaSampler;
layout(set = 1, binding = 7) uniform sampler2D Sampler_g_blendUVDistortionSampler;
layout(set = 1, binding = 5) uniform sampler2D Sampler_g_blendSampler;
layout(set = 1, binding = 6) uniform sampler2D Sampler_g_blendAlphaSampler;

layout(location = 0) in vec2 Input_UV;
layout(location = 1) in vec3 Input_Normal;
layout(location = 2) in vec3 Input_Binormal;
layout(location = 3) in vec3 Input_Tangent;
layout(location = 4) in vec4 Input_Color;
layout(location = 5) in vec4 Input_Alpha_Dist_UV;
layout(location = 6) in vec4 Input_Blend_Alpha_Dist_UV;
layout(location = 7) in vec4 Input_Blend_FBNextIndex_UV;
layout(location = 8) in vec2 Input_Others;
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
    ret.FlipbookRate = psinput.Others.x;
    ret.AlphaThreshold = psinput.Others.y;
    return ret;
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
        vec3 _77 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_77.x, _77.y, _77.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _89 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_89.x, _89.y, _89.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _102 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_102.x, _102.y, _102.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _115 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_115.x, _115.y, _115.z, dstColor.w);
                }
            }
        }
    }
}

vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    vec2 UVOffset = vec2(0.0);
    UVOffset = (texture(Sampler_g_uvDistortionSampler, advancedParam.UVDistortionUV).xy * 2.0) - vec2(1.0);
    UVOffset *= _187.fUVDistortionParameter.x;
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV + UVOffset) * Input.Color;
    vec3 texNormal = (texture(Sampler_g_normalSampler, Input.UV + UVOffset).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.Tangent), vec3(Input.Binormal), vec3(Input.Normal)) * texNormal);
    vec4 param_1 = Output;
    float param_2 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_1, _187.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_2, Sampler_g_colorSampler);
    Output = param_1;
    vec4 AlphaTexColor = texture(Sampler_g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 BlendUVOffset = (texture(Sampler_g_blendUVDistortionSampler, advancedParam.BlendUVDistortionUV).xy * 2.0) - vec2(1.0);
    BlendUVOffset *= _187.fUVDistortionParameter.y;
    vec4 BlendTextureColor = texture(Sampler_g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendAlphaTextureColor = texture(Sampler_g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_3 = Output;
    ApplyTextureBlending(param_3, BlendTextureColor, _187.fBlendTextureParameter.x);
    Output = param_3;
    float diffuse = max(dot(_187.fLightDirection.xyz, localNormal), 0.0);
    vec3 _352 = Output.xyz * ((_187.fLightColor.xyz * diffuse) + _187.fLightAmbient.xyz);
    Output = vec4(_352.x, _352.y, _352.z, Output.w);
    if (_187.fFalloffParam.Param.x == 1.0)
    {
        vec3 cameraVec = normalize(-_187.fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        vec4 FalloffBlendColor = mix(_187.fFalloffParam.EndColor, _187.fFalloffParam.BeginColor, vec4(pow(CdotN, _187.fFalloffParam.Param.z)));
        if (_187.fFalloffParam.Param.y == 0.0)
        {
            vec3 _398 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_398.x, _398.y, _398.z, Output.w);
        }
        else
        {
            if (_187.fFalloffParam.Param.y == 1.0)
            {
                vec3 _411 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_411.x, _411.y, _411.z, Output.w);
            }
            else
            {
                if (_187.fFalloffParam.Param.y == 2.0)
                {
                    vec3 _424 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_424.x, _424.y, _424.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    vec3 _437 = Output.xyz * _187.fEmissiveScaling.x;
    Output = vec4(_437.x, _437.y, _437.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _468 = mix(_187.fEdgeColor.xyz * _187.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - _187.fEdgeParameter.x)));
    Output = vec4(_468.x, _468.y, _468.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Pos = gl_FragCoord;
    Input.UV = Input_UV;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.Color = Input_Color;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    vec4 _511 = _main(Input);
    _entryPointOutput = _511;
}

