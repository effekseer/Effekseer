struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float3 WorldN;
    float3 WorldB;
    float3 WorldT;
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
    float4 _284_fLightDirection : packoffset(c0);
    float4 _284_fLightColor : packoffset(c1);
    float4 _284_fLightAmbient : packoffset(c2);
    float4 _284_fFlipbookParameter : packoffset(c3);
    float4 _284_fUVDistortionParameter : packoffset(c4);
    float4 _284_fBlendTextureParameter : packoffset(c5);
    float4 _284_fCameraFrontDirection : packoffset(c6);
    FalloffParameter _284_fFalloffParam : packoffset(c7);
    float4 _284_fEmissiveScaling : packoffset(c10);
    float4 _284_fEdgeColor : packoffset(c11);
    float4 _284_fEdgeParameter : packoffset(c12);
    float4 _284_softParticleParam : packoffset(c13);
    float4 _284_reconstructionParam1 : packoffset(c14);
    float4 _284_reconstructionParam2 : packoffset(c15);
};

Texture2D<float4> _uvDistortionTex : register(t3);
SamplerState sampler_uvDistortionTex : register(s3);
Texture2D<float4> _colorTex : register(t0);
SamplerState sampler_colorTex : register(s0);
Texture2D<float4> _normalTex : register(t1);
SamplerState sampler_normalTex : register(s1);
Texture2D<float4> _alphaTex : register(t2);
SamplerState sampler_alphaTex : register(s2);
Texture2D<float4> _blendUVDistortionTex : register(t6);
SamplerState sampler_blendUVDistortionTex : register(s6);
Texture2D<float4> _blendTex : register(t4);
SamplerState sampler_blendTex : register(s4);
Texture2D<float4> _blendAlphaTex : register(t5);
SamplerState sampler_blendAlphaTex : register(s5);
Texture2D<float4> _depthTex : register(t7);
SamplerState sampler_depthTex : register(s7);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float3 Input_WorldN;
static float3 Input_WorldB;
static float3 Input_WorldT;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float2 Input_Others;
static float4 Input_PosP;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float3 Input_WorldB : TEXCOORD3;
    float3 Input_WorldT : TEXCOORD4;
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
    float2 param_2 = _284_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(_uvDistortionTex, sampler_uvDistortionTex, param_1, param_2);
    UVOffset *= _284_fUVDistortionParameter.x;
    float4 Output = _colorTex.Sample(sampler_colorTex, Input.UV + UVOffset) * Input.Color;
    float3 texNormal = (_normalTex.Sample(sampler_normalTex, Input.UV + UVOffset).xyz - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _colorTex, sampler_colorTex, _284_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 AlphaTexColor = _alphaTex.Sample(sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = _284_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(_blendUVDistortionTex, sampler_blendUVDistortionTex, param_5, param_6);
    BlendUVOffset *= _284_fUVDistortionParameter.y;
    float4 BlendTextureColor = _blendTex.Sample(sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = _blendAlphaTex.Sample(sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _284_fBlendTextureParameter.x);
    Output = param_7;
    float diffuse = max(dot(_284_fLightDirection.xyz, localNormal), 0.0f);
    float3 _457 = Output.xyz * ((_284_fLightColor.xyz * diffuse) + _284_fLightAmbient.xyz);
    Output = float4(_457.x, _457.y, _457.z, Output.w);
    if (_284_fFalloffParam.Param.x == 1.0f)
    {
        float3 cameraVec = normalize(-_284_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, float3(localNormal.x, localNormal.y, localNormal.z)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_284_fFalloffParam.EndColor, _284_fFalloffParam.BeginColor, pow(CdotN, _284_fFalloffParam.Param.z).xxxx);
        if (_284_fFalloffParam.Param.y == 0.0f)
        {
            float3 _502 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_502.x, _502.y, _502.z, Output.w);
        }
        else
        {
            if (_284_fFalloffParam.Param.y == 1.0f)
            {
                float3 _515 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_515.x, _515.y, _515.z, Output.w);
            }
            else
            {
                if (_284_fFalloffParam.Param.y == 2.0f)
                {
                    float3 _528 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_528.x, _528.y, _528.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _541 = Output.xyz * _284_fEmissiveScaling.x;
    Output = float4(_541.x, _541.y, _541.z, Output.w);
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    if (_284_softParticleParam.w != 0.0f)
    {
        float backgroundZ = _depthTex.Sample(sampler_depthTex, screenUV).x;
        float param_8 = backgroundZ;
        float param_9 = screenPos.z;
        float4 param_10 = _284_softParticleParam;
        float4 param_11 = _284_reconstructionParam1;
        float4 param_12 = _284_reconstructionParam2;
        Output.w *= SoftParticle(param_8, param_9, param_10, param_11, param_12);
    }
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _624 = lerp(_284_fEdgeColor.xyz * _284_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _284_fEdgeParameter.x).xxx);
    Output = float4(_624.x, _624.y, _624.z, Output.w);
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.WorldN = Input_WorldN;
    Input.WorldB = Input_WorldB;
    Input.WorldT = Input_WorldT;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    Input.PosP = Input_PosP;
    float4 _670 = _main(Input);
    _entryPointOutput = _670;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldB = stage_input.Input_WorldB;
    Input_WorldT = stage_input.Input_WorldT;
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
