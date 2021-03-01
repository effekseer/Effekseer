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
} _282;

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

float SoftParticle(float backgroundZ, float meshZ, vec4 softparticleParam, vec4 reconstruct1, vec4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    vec2 rescale = reconstruct1.xy;
    vec4 params = reconstruct2;
    vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    float alphaFar = (depth.y - depth.x) / distanceFar;
    float alphaNear = ((-distanceNearOffset) - depth.y) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0), 1.0);
}

vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    vec2 param_1 = advancedParam.UVDistortionUV;
    vec2 param_2 = _282.fUVDistortionParameter.zw;
    vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= _282.fUVDistortionParameter.x;
    vec4 Output = texture(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset) * Input.Color;
    vec3 texNormal = (texture(Sampler_sampler_normalTex, Input.UV_Others.xy + UVOffset).xyz - vec3(0.5)) * 2.0;
    vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    vec4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _282.fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_sampler_colorTex);
    Output = param_3;
    vec4 AlphaTexColor = texture(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    vec2 param_5 = advancedParam.BlendUVDistortionUV;
    vec2 param_6 = _282.fUVDistortionParameter.zw;
    vec2 BlendUVOffset = UVDistortionOffset(param_5, param_6, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _282.fUVDistortionParameter.y;
    vec4 BlendTextureColor = texture(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    vec4 BlendAlphaTextureColor = texture(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    vec4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _282.fBlendTextureParameter.x);
    Output = param_7;
    float diffuse = max(dot(_282.fLightDirection.xyz, localNormal), 0.0);
    vec3 _457 = Output.xyz * ((_282.fLightColor.xyz * diffuse) + _282.fLightAmbient.xyz);
    Output = vec4(_457.x, _457.y, _457.z, Output.w);
    if (_282.fFalloffParameter.x == 1.0)
    {
        vec3 cameraVec = normalize(-_282.fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, vec3(localNormal.x, localNormal.y, localNormal.z)), 0.0, 1.0);
        vec4 FalloffBlendColor = mix(_282.fFalloffEndColor, _282.fFalloffBeginColor, vec4(pow(CdotN, _282.fFalloffParameter.z)));
        if (_282.fFalloffParameter.y == 0.0)
        {
            vec3 _503 = Output.xyz + FalloffBlendColor.xyz;
            Output = vec4(_503.x, _503.y, _503.z, Output.w);
        }
        else
        {
            if (_282.fFalloffParameter.y == 1.0)
            {
                vec3 _516 = Output.xyz - FalloffBlendColor.xyz;
                Output = vec4(_516.x, _516.y, _516.z, Output.w);
            }
            else
            {
                if (_282.fFalloffParameter.y == 2.0)
                {
                    vec3 _529 = Output.xyz * FalloffBlendColor.xyz;
                    Output = vec4(_529.x, _529.y, _529.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    vec3 _543 = Output.xyz * _282.fEmissiveScaling.x;
    Output = vec4(_543.x, _543.y, _543.z, Output.w);
    vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = _282.mUVInversedBack.x + (_282.mUVInversedBack.y * screenUV.y);
    if (!(_282.softParticleParam.w == 0.0))
    {
        float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
        float param_8 = backgroundZ;
        float param_9 = screenPos.z;
        vec4 param_10 = _282.softParticleParam;
        vec4 param_11 = _282.reconstructionParam1;
        vec4 param_12 = _282.reconstructionParam2;
        Output.w *= SoftParticle(param_8, param_9, param_10, param_11, param_12);
    }
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    vec3 _637 = mix(_282.fEdgeColor.xyz * _282.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - _282.fEdgeParameter.x)));
    Output = vec4(_637.x, _637.y, _637.z, Output.w);
    return Output;
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
    vec4 _679 = _main(Input);
    _entryPointOutput = _679;
}

