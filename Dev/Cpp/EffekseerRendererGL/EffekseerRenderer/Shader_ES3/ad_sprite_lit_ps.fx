#version 300 es
precision mediump float;
precision highp int;

struct PS_Input
{
    highp vec4 PosVS;
    highp vec4 VColor;
    highp vec2 UV;
    highp vec3 WorldN;
    highp vec3 WorldT;
    highp vec3 WorldB;
    highp vec4 Alpha_Dist_UV;
    highp vec4 Blend_Alpha_Dist_UV;
    highp vec4 Blend_FBNextIndex_UV;
    highp vec2 Others;
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
    highp vec4 fEmissiveScaling;
    highp vec4 fEdgeColor;
    highp vec4 fEdgeParameter;
    highp vec4 softParticleAndReconstructionParam1;
    highp vec4 reconstructionParam2;
};

uniform PS_ConstanBuffer CBPS0;

uniform highp sampler2D Sampler_sampler_uvDistortionTex;
uniform highp sampler2D Sampler_sampler_normalTex;
uniform highp sampler2D Sampler_sampler_colorTex;
uniform highp sampler2D Sampler_sampler_depthTex;
uniform highp sampler2D Sampler_sampler_alphaTex;
uniform highp sampler2D Sampler_sampler_blendUVDistortionTex;
uniform highp sampler2D Sampler_sampler_blendTex;
uniform highp sampler2D Sampler_sampler_blendAlphaTex;

centroid in highp vec4 _VSPS_VColor;
centroid in highp vec2 _VSPS_UV;
in highp vec3 _VSPS_WorldN;
in highp vec3 _VSPS_WorldT;
in highp vec3 _VSPS_WorldB;
in highp vec4 _VSPS_Alpha_Dist_UV;
in highp vec4 _VSPS_Blend_Alpha_Dist_UV;
in highp vec4 _VSPS_Blend_FBNextIndex_UV;
in highp vec2 _VSPS_Others;
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
    ret.FlipbookRate = psinput.Others.x;
    ret.AlphaThreshold = psinput.Others.y;
    return ret;
}

highp vec2 UVDistortionOffset(highp vec2 uv, highp vec2 uvInversed, highp sampler2D SPIRV_Cross_Combinedts)
{
    highp vec2 UVOffset = (texture(SPIRV_Cross_Combinedts, uv).xy * 2.0) - vec2(1.0);
    UVOffset.y *= (-1.0);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
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

highp float SoftParticle(highp float backgroundZ, highp float meshZ, highp float softparticleParam, highp vec2 reconstruct1, highp vec4 reconstruct2)
{
    highp float _distance = softparticleParam;
    highp vec2 rescale = reconstruct1;
    highp vec4 params = reconstruct2;
    highp float tempY = params.y;
    params.y = params.z;
    params.z = tempY;
    highp vec2 zs = vec2((backgroundZ * rescale.x) + rescale.y, meshZ);
    highp vec2 depth = ((zs * params.w) - vec2(params.y)) / (vec2(params.x) - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0), 1.0);
}

void ApplyTextureBlending(inout highp vec4 dstColor, highp vec4 blendColor, highp float blendType)
{
    if (blendType == 0.0)
    {
        highp vec3 _93 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0 - blendColor.w));
        dstColor = vec4(_93.x, _93.y, _93.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0)
        {
            highp vec3 _105 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = vec4(_105.x, _105.y, _105.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0)
            {
                highp vec3 _118 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = vec4(_118.x, _118.y, _118.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0)
                {
                    highp vec3 _131 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = vec4(_131.x, _131.y, _131.z, dstColor.w);
                }
            }
        }
    }
}

highp vec4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    highp vec2 param_1 = advancedParam.UVDistortionUV;
    highp vec2 param_2 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= CBPS0.fUVDistortionParameter.x;
    highp float diffuse = 1.0;
    highp vec3 loN = texture(Sampler_sampler_normalTex, Input.UV + UVOffset).xyz;
    highp vec3 texNormal = (loN - vec3(0.5)) * 2.0;
    highp vec3 localNormal = normalize(mat3(vec3(Input.WorldT), vec3(Input.WorldB), vec3(Input.WorldN)) * texNormal);
    diffuse = max(dot(CBPS0.fLightDirection.xyz, localNormal), 0.0);
    highp vec4 Output = texture(Sampler_sampler_colorTex, Input.UV + UVOffset) * Input.VColor;
    highp vec4 param_3 = Output;
    highp float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, CBPS0.fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_sampler_colorTex);
    Output = param_3;
    highp vec4 screenPos = Input.PosP / vec4(Input.PosP.w);
    highp vec2 screenUV = (screenPos.xy + vec2(1.0)) / vec2(2.0);
    screenUV.y = 1.0 - screenUV.y;
    screenUV.y = 1.0 - screenUV.y;
    highp float backgroundZ = texture(Sampler_sampler_depthTex, screenUV).x;
    if (!(CBPS0.softParticleAndReconstructionParam1.x == 0.0))
    {
        highp float param_5 = backgroundZ;
        highp float param_6 = screenPos.z;
        highp float param_7 = CBPS0.softParticleAndReconstructionParam1.x;
        highp vec2 param_8 = CBPS0.softParticleAndReconstructionParam1.yz;
        highp vec4 param_9 = CBPS0.reconstructionParam2;
        Output.w *= SoftParticle(param_5, param_6, param_7, param_8, param_9);
    }
    highp vec4 AlphaTexColor = texture(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    highp vec2 param_10 = advancedParam.BlendUVDistortionUV;
    highp vec2 param_11 = CBPS0.fUVDistortionParameter.zw;
    highp vec2 BlendUVOffset = UVDistortionOffset(param_10, param_11, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= CBPS0.fUVDistortionParameter.y;
    highp vec4 BlendTextureColor = texture(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    highp vec4 BlendAlphaTextureColor = texture(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    highp vec4 param_12 = Output;
    ApplyTextureBlending(param_12, BlendTextureColor, CBPS0.fBlendTextureParameter.x);
    Output = param_12;
    highp vec3 _494 = Output.xyz * CBPS0.fEmissiveScaling.x;
    Output = vec4(_494.x, _494.y, _494.z, Output.w);
    if (Output.w <= max(0.0, advancedParam.AlphaThreshold))
    {
        discard;
    }
    highp vec3 _519 = Output.xyz * (vec3(diffuse, diffuse, diffuse) + vec3(CBPS0.fLightAmbient.xyz));
    Output = vec4(_519.x, _519.y, _519.z, Output.w);
    highp vec3 _540 = mix(CBPS0.fEdgeColor.xyz * CBPS0.fEdgeParameter.y, Output.xyz, vec3(ceil((Output.w - advancedParam.AlphaThreshold) - CBPS0.fEdgeParameter.x)));
    Output = vec4(_540.x, _540.y, _540.z, Output.w);
    return Output;
}

void main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.VColor = _VSPS_VColor;
    Input.UV = _VSPS_UV;
    Input.WorldN = _VSPS_WorldN;
    Input.WorldT = _VSPS_WorldT;
    Input.WorldB = _VSPS_WorldB;
    Input.Alpha_Dist_UV = _VSPS_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = _VSPS_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = _VSPS_Blend_FBNextIndex_UV;
    Input.Others = _VSPS_Others;
    Input.PosP = _VSPS_PosP;
    highp vec4 _586 = _main(Input);
    _entryPointOutput = _586;
}

