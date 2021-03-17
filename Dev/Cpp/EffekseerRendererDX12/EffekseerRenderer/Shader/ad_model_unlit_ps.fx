struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
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

cbuffer PS_ConstanBuffer : register(b1)
{
    float4 _281_fLightDirection : packoffset(c0);
    float4 _281_fLightColor : packoffset(c1);
    float4 _281_fLightAmbient : packoffset(c2);
    float4 _281_fFlipbookParameter : packoffset(c3);
    float4 _281_fUVDistortionParameter : packoffset(c4);
    float4 _281_fBlendTextureParameter : packoffset(c5);
    float4 _281_fCameraFrontDirection : packoffset(c6);
    float4 _281_fFalloffParameter : packoffset(c7);
    float4 _281_fFalloffBeginColor : packoffset(c8);
    float4 _281_fFalloffEndColor : packoffset(c9);
    float4 _281_fEmissiveScaling : packoffset(c10);
    float4 _281_fEdgeColor : packoffset(c11);
    float4 _281_fEdgeParameter : packoffset(c12);
    float4 _281_softParticleParam : packoffset(c13);
    float4 _281_reconstructionParam1 : packoffset(c14);
    float4 _281_reconstructionParam2 : packoffset(c15);
    float4 _281_mUVInversedBack : packoffset(c16);
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
static float4 Input_Color;
static float4 Input_UV_Others;
static float3 Input_WorldN;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float4 Input_UV_Others : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float4 Input_Alpha_Dist_UV : TEXCOORD3;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD4;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD5;
    float4 Input_PosP : TEXCOORD6;
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
    ret.FlipbookRate = psinput.UV_Others.z;
    ret.AlphaThreshold = psinput.UV_Others.w;
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

float SoftParticle(float backgroundZ, float meshZ, float4 softparticleParam, float4 reconstruct1, float4 reconstruct2)
{
    float distanceFar = softparticleParam.x;
    float distanceNear = softparticleParam.y;
    float distanceNearOffset = softparticleParam.z;
    float2 rescale = reconstruct1.xy;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - params.y.xx) / (params.x.xx - (zs * params.z));
    float alphaFar = (depth.y - depth.x) / distanceFar;
    float alphaNear = ((-distanceNearOffset) - depth.y) / distanceNear;
    return min(max(min(alphaFar, alphaNear), 0.0f), 1.0f);
}

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _281_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2);
    UVOffset *= _281_fUVDistortionParameter.x;
    float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV_Others.xy + UVOffset) * Input.Color;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _colorTex, sampler_colorTex, _281_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = _281_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_5, param_6);
    BlendUVOffset *= _281_fUVDistortionParameter.y;
    float4 BlendTextureColor = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _281_fBlendTextureParameter.x);
    Output = param_7;
    if (_281_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_281_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_281_fFalloffEndColor, _281_fFalloffBeginColor, pow(CdotN, _281_fFalloffParameter.z).xxxx);
        if (_281_fFalloffParameter.y == 0.0f)
        {
            float3 _440 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_440.x, _440.y, _440.z, Output.w);
        }
        else
        {
            if (_281_fFalloffParameter.y == 1.0f)
            {
                float3 _453 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_453.x, _453.y, _453.z, Output.w);
            }
            else
            {
                if (_281_fFalloffParameter.y == 2.0f)
                {
                    float3 _466 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_466.x, _466.y, _466.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _480 = Output.xyz * _281_fEmissiveScaling.x;
    Output = float4(_480.x, _480.y, _480.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    screenUV.y = _281_mUVInversedBack.x + (_281_mUVInversedBack.y * screenUV.y);
    if (_281_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param_8 = backgroundZ;
        float param_9 = screenPos.z;
        float4 param_10 = _281_softParticleParam;
        float4 param_11 = _281_reconstructionParam1;
        float4 param_12 = _281_reconstructionParam2;
        Output.w *= SoftParticle(param_8, param_9, param_10, param_11, param_12);
    }
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _574 = lerp(_281_fEdgeColor.xyz * _281_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _281_fEdgeParameter.x).xxx);
    Output = float4(_574.x, _574.y, _574.z, Output.w);
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV_Others = Input_UV_Others;
    Input.WorldN = Input_WorldN;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.PosP = Input_PosP;
    float4 _610 = _main(Input);
    _entryPointOutput = _610;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_WorldN = stage_input.Input_WorldN;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    Input_PosP = stage_input.Input_PosP;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
