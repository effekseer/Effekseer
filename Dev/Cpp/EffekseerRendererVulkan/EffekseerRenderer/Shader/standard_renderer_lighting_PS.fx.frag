#version 420

struct PS_Input
{
    vec4 Position;
    vec4 VColor;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
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

layout(set = 1, binding = 0, std140) uniform PS_ConstanBuffer
{
    vec4 fLightDirection;
    vec4 fLightColor;
    vec4 fLightAmbient;
    vec4 fFlipbookParameter;
    vec4 fUVDistortionParameter;
    vec4 fBlendTextureParameter;
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
} _209;

layout(set = 1, binding = 4) uniform sampler2D Sampler_g_uvDistortionSampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_normalSampler;
layout(set = 1, binding = 1) uniform sampler2D Sampler_g_colorSampler;
layout(set = 1, binding = 3) uniform sampler2D Sampler_g_alphaSampler;
layout(set = 1, binding = 7) uniform sampler2D Sampler_g_blendUVDistortionSampler;
layout(set = 1, binding = 5) uniform sampler2D Sampler_g_blendSampler;
layout(set = 1, binding = 6) uniform sampler2D Sampler_g_blendAlphaSampler;

layout(location = 0) centroid in vec4 Input_VColor;
layout(location = 1) centroid in vec2 Input_UV;
layout(location = 2) in vec3 Input_WorldN;
layout(location = 3) in vec3 Input_WorldT;
layout(location = 4) in vec3 Input_WorldB;
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

vec2 UVDistortionOffset(vec2 uv, vec2 uvInversed, sampler2D SPIRV_Cross_Combinedts)
{
    vec2 UVOffset = (texture(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
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
    vec2 param_2 = _209.fUVDistortionParameter.zw;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_g_uvDistortionSampler);
    UVOffset *= _209.fUVDistortionParameter.x;
    float diffuse = 1.0;
    vec3 loN = texture(Sampler_g_normalSampler, Input.UV + UVOffset).xyz;
    vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    diffuse = max(dot(_209.fLightDirection.xyz, localNormal), 0.0);
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV + UVOffset) * Input.VColor;
    vec4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _209.fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_g_colorSampler);
    Output = param_3;
    vec4 AlphaTexColor = texture(Sampler_g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_5 = advancedParam.BlendUVDistortionUV;
    vec2 param_6 = _209.fUVDistortionParameter.zw;
    vec2 BlendUVOffset = UVDistortionOffset(param_5, param_6, Sampler_g_blendUVDistortionSampler);
    BlendUVOffset *= _209.fUVDistortionParameter.y;
    vec4 BlendTextureColor = texture(Sampler_g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendAlphaTextureColor = texture(Sampler_g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _209.fBlendTextureParameter.x);
    Output = param_7;
    vec3 _377 = Output.xyz * _209.fEmissiveScaling.x;
    Output = vec4(_377.x, _377.y, _377.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _402 = Output.xyz * (vec3(diffuse, diffuse, diffuse) + vec3(_209.fLightAmbient.xyz));
    Output = vec4(_402.x, _402.y, _402.z, Output.w);
    vec3 _423 = mix(_209.fEdgeColor.xyz * _209.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - _209.fEdgeParameter.x)));
    Output = vec4(_423.x, _423.y, _423.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.VColor = Input_VColor;
    Input.UV = Input_UV;
    Input.WorldN = Input_WorldN;
    Input.WorldT = Input_WorldT;
    Input.WorldB = Input_WorldB;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    vec4 _466 = _main(Input);
    _entryPointOutput = _466;
}

