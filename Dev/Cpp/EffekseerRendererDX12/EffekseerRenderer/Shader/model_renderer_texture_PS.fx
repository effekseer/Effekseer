struct PS_Input
{
    float4 PosVS;
    float2 UV;
    float3 Normal;
    float3 Binormal;
    float3 Tangent;
    float4 Color;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float2 Others;
    float4 PosP;
};

struct AdvancedParameter
{
    float2 AlphaUV;
    float2 UVDistortionUV;
    float2 BlendUV;
    float2 BlendAlphaUV;
    float2 BlendUVDistortionUV;
    float2 FlipbookNextIndexUV;
    float FlipbookRate;
    float AlphaThreshold;
};

struct FalloffParameter
{
    float4 Param;
    float4 BeginColor;
    float4 EndColor;
};

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _264_fLightDirection : packoffset(c0);
    float4 _264_fLightColor : packoffset(c1);
    float4 _264_fLightAmbient : packoffset(c2);
    float4 _264_fFlipbookParameter : packoffset(c3);
    float4 _264_fUVDistortionParameter : packoffset(c4);
    float4 _264_fBlendTextureParameter : packoffset(c5);
    float4 _264_fCameraFrontDirection : packoffset(c6);
    FalloffParameter _264_fFalloffParam : packoffset(c7);
    float4 _264_fEmissiveScaling : packoffset(c10);
    float4 _264_fEdgeColor : packoffset(c11);
    float4 _264_fEdgeParameter : packoffset(c12);
    float4 _264_softParticleAndReconstructionParam1 : packoffset(c13);
    float4 _264_reconstructionParam2 : packoffset(c14);
};

Texture2D<float4> _uvDistortionTex : register(t2);
SamplerState sampler_uvDistortionTex : register(s2);
Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _alphaTex : register(t1);
SamplerState sampler_alphaTex : register(s1);
Texture2D<float4> _blendUVDistortionTex : register(t5);
SamplerState sampler_blendUVDistortionTex : register(s5);
Texture2D<float4> _blendTex : register(t3);
SamplerState sampler_blendTex : register(s3);
Texture2D<float4> _blendAlphaTex : register(t4);
SamplerState sampler_blendAlphaTex : register(s4);
Texture2D<float4> _depthTex : register(t6);
SamplerState sampler_depthTex : register(s6);

static float4 gl_FragCoord;
static float2 Input_UV;
static float3 Input_Normal;
static float3 Input_Binormal;
static float3 Input_Tangent;
static float4 Input_Color;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float2 Input_Others;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float2 Input_UV : TEXCOORD0;
    float3 Input_Normal : TEXCOORD1;
    float3 Input_Binormal : TEXCOORD2;
    float3 Input_Tangent : TEXCOORD3;
    centroid float4 Input_Color : TEXCOORD4;
    float4 Input_Alpha_Dist_UV : TEXCOORD5;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD6;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD7;
    float2 Input_Others : TEXCOORD8;
    float4 Input_PosP : TEXCOORD9;
    float4 gl_FragCoord : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : SV_Target0;
};

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

float2 UVDistortionOffset(Texture2D<float4> t, SamplerState s, float2 uv, float2 uvInversed)
{
    float2 UVOffset = (t.Sample(s, uv).xy * 2.0f) - 1.0f.xx;
    UVOffset.y *= (-1.0f);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout float4 dst, Texture2D<float4> t, SamplerState s, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate)
{
    if (flipbookParameter.x > 0.0f)
    {
        float4 NextPixelColor = t.Sample(s, nextUV) * vcolor;
        if (flipbookParameter.y == 1.0f)
        {
            dst = lerp(dst, NextPixelColor, flipbookRate.xxxx);
        }
    }
}

void ApplyTextureBlending(inout float4 dstColor, float4 blendColor, float blendType)
{
    if (blendType == 0.0f)
    {
        float3 _93 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_93.x, _93.y, _93.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _105 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_105.x, _105.y, _105.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _118 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_118.x, _118.y, _118.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _131 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_131.x, _131.y, _131.z, dstColor.w);
                }
            }
        }
    }
}

float SoftParticle(float backgroundZ, float meshZ, float softparticleParam, float2 reconstruct1, float4 reconstruct2)
{
    float _distance = softparticleParam;
    float2 rescale = reconstruct1;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - params.y.xx) / (params.x.xx - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0f), 1.0f);
}

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _264_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2);
    UVOffset *= _264_fUVDistortionParameter.x;
    float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV + UVOffset) * Input.Color;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _colorTex, sampler_colorTex, _264_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = _264_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_5, param_6);
    BlendUVOffset *= _264_fUVDistortionParameter.y;
    float4 BlendTextureColor = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _264_fBlendTextureParameter.x);
    Output = param_7;
    if (_264_fFalloffParam.Param.x == 1.0f)
    {
        float3 cameraVec = normalize(-_264_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.Normal)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_264_fFalloffParam.EndColor, _264_fFalloffParam.BeginColor, pow(CdotN, _264_fFalloffParam.Param.z).xxxx);
        if (_264_fFalloffParam.Param.y == 0.0f)
        {
            float3 _420 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_420.x, _420.y, _420.z, Output.w);
        }
        else
        {
            if (_264_fFalloffParam.Param.y == 1.0f)
            {
                float3 _433 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_433.x, _433.y, _433.z, Output.w);
            }
            else
            {
                if (_264_fFalloffParam.Param.y == 2.0f)
                {
                    float3 _446 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_446.x, _446.y, _446.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _459 = Output.xyz * _264_fEmissiveScaling.x;
    Output = float4(_459.x, _459.y, _459.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
    if (_264_softParticleAndReconstructionParam1.x != 0.0f)
    {
        float param_8 = backgroundZ;
        float param_9 = screenPos.z;
        float param_10 = _264_softParticleAndReconstructionParam1.x;
        float2 param_11 = _264_softParticleAndReconstructionParam1.yz;
        float4 param_12 = _264_reconstructionParam2;
        Output.w *= SoftParticle(param_8, param_9, param_10, param_11, param_12);
    }
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _542 = lerp(_264_fEdgeColor.xyz * _264_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _264_fEdgeParameter.x).xxx);
    Output = float4(_542.x, _542.y, _542.z, Output.w);
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.UV = Input_UV;
    Input.Normal = Input_Normal;
    Input.Binormal = Input_Binormal;
    Input.Tangent = Input_Tangent;
    Input.Color = Input_Color;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    Input.PosP = Input_PosP;
    float4 _588 = _main(Input);
    _entryPointOutput = _588;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_UV = stage_input.Input_UV;
    Input_Normal = stage_input.Input_Normal;
    Input_Binormal = stage_input.Input_Binormal;
    Input_Tangent = stage_input.Input_Tangent;
    Input_Color = stage_input.Input_Color;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    Input_Others = stage_input.Input_Others;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
