#version 420

struct PS_Input
{
    vec4 PosVS;
    vec4 VColor;
    vec2 UV;
    vec3 WorldN;
    vec3 WorldT;
    vec3 WorldB;
    vec4 Alpha_Dist_UV;
    vec4 Blend_Alpha_Dist_UV;
    vec4 Blend_FBNextIndex_UV;
    vec2 Others;
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
    vec4 fEmissiveScaling;
    vec4 fEdgeColor;
    vec4 fEdgeParameter;
    vec4 softParticleAndReconstructionParam1;
    vec4 reconstructionParam2;
} _263;

layout(set = 1, binding = 4) uniform sampler2D Sampler_g_uvDistortionSampler;
layout(set = 1, binding = 2) uniform sampler2D Sampler_g_normalSampler;
layout(set = 1, binding = 1) uniform sampler2D Sampler_g_colorSampler;
layout(set = 1, binding = 8) uniform sampler2D Sampler_g_depthSampler;
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
layout(location = 9) in vec4 Input_PosP;
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

float SoftParticle(float backgroundZ, float meshZ, float softparticleParam, vec2 reconstruct1, vec4 reconstruct2)
{
    float _distance = softparticleParam;
    vec2 rescale = reconstruct1;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0), 1.0);
}

void ApplyTextureBlending(inout vec4 dstColor, vec4 blendColor, float blendType)
{
    if (blendType == 0.0)
    {
        vec3 _93 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_93.x, _93.y, _93.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            vec3 _105 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_105.x, _105.y, _105.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                vec3 _118 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_118.x, _118.y, _118.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    vec3 _131 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_131.x, _131.y, _131.z, dstColor.w);
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
    vec2 param_2 = _263.fUVDistortionParameter.zw;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_g_uvDistortionSampler);
    UVOffset *= _263.fUVDistortionParameter.x;
    float diffuse = 1.0;
    vec3 loN = texture(Sampler_g_normalSampler, Input.UV + UVOffset).xyz;
    vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    diffuse = max(dot(_263.fLightDirection.xyz, localNormal), 0.0);
    vec4 Output = texture(Sampler_g_colorSampler, Input.UV + UVOffset) * Input.VColor;
    vec4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _263.fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_g_colorSampler);
    Output = param_3;
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    float backgroundZ = texture(Sampler_g_depthSampler, screenUV).x;
    if (!(_263.softParticleAndReconstructionParam1.x == 0.0))
    {
        float param_5 = backgroundZ;
        float param_6 = screenPos.z;
        float param_7 = _263.softParticleAndReconstructionParam1.x;
        vec2 param_8 = _263.softParticleAndReconstructionParam1.yz;
        vec4 param_9 = _263.reconstructionParam2;
        Output.w *= SoftParticle(param_5, param_6, param_7, param_8, param_9);
    }
    vec4 AlphaTexColor = texture(Sampler_g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_10 = advancedParam.BlendUVDistortionUV;
    vec2 param_11 = _263.fUVDistortionParameter.zw;
    vec2 BlendUVOffset = UVDistortionOffset(param_10, param_11, Sampler_g_blendUVDistortionSampler);
    BlendUVOffset *= _263.fUVDistortionParameter.y;
    vec4 BlendTextureColor = texture(Sampler_g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendAlphaTextureColor = texture(Sampler_g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_12 = Output;
    ApplyTextureBlending(param_12, BlendTextureColor, _263.fBlendTextureParameter.x);
    Output = param_12;
    vec3 _482 = Output.xyz * _263.fEmissiveScaling.x;
    Output = vec4(_482.x, _482.y, _482.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _507 = Output.xyz * (vec3(diffuse, diffuse, diffuse) + vec3(_263.fLightAmbient.xyz));
    Output = vec4(_507.x, _507.y, _507.z, Output.w);
    vec3 _528 = mix(_263.fEdgeColor.xyz * _263.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - _263.fEdgeParameter.x)));
    Output = vec4(_528.x, _528.y, _528.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.VColor = Input_VColor;
    Input.UV = Input_UV;
    Input.WorldN = Input_WorldN;
    Input.WorldT = Input_WorldT;
    Input.WorldB = Input_WorldB;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    Input.PosP = Input_PosP;
    vec4 _574 = _main(Input);
    _entryPointOutput = _574;
}

