struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float2 UV;
    float4 PosP;
    float4 PosU;
    float4 PosR;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
    float2 Others;
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
    float4 _263_flipbookParameter : packoffset(c0);
    float4 _263_uvDistortionParameter : packoffset(c1);
    float4 _263_blendTextureParameter : packoffset(c2);
    float4 _263_emissiveScaling : packoffset(c3);
    float4 _263_edgeColor : packoffset(c4);
    float4 _263_edgeParameter : packoffset(c5);
    float4 _263_softParticleAndReconstructionParam1 : packoffset(c6);
    float4 _263_reconstructionParam2 : packoffset(c7);
};

Texture2D<float4> g_uvDistortionTexture : register(t2);
SamplerState g_uvDistortionSampler : register(s2);
Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);
Texture2D<float4> g_depthTexture : register(t6);
SamplerState g_depthSampler : register(s6);
Texture2D<float4> g_alphaTexture : register(t1);
SamplerState g_alphaSampler : register(s1);
Texture2D<float4> g_blendUVDistortionTexture : register(t5);
SamplerState g_blendUVDistortionSampler : register(s5);
Texture2D<float4> g_blendTexture : register(t3);
SamplerState g_blendSampler : register(s3);
Texture2D<float4> g_blendAlphaTexture : register(t4);
SamplerState g_blendAlphaSampler : register(s4);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_PosP;
static float4 Input_PosU;
static float4 Input_PosR;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float2 Input_Others;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float2 Input_UV : TEXCOORD1;
    float4 Input_PosP : TEXCOORD2;
    float4 Input_PosU : TEXCOORD3;
    float4 Input_PosR : TEXCOORD4;
    float4 Input_Alpha_Dist_UV : TEXCOORD5;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD6;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD7;
    float2 Input_Others : TEXCOORD8;
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

float SoftParticle(float backgroundZ, float meshZ, float softparticleParam, float2 reconstruct1, float4 reconstruct2)
{
    float _distance = softparticleParam;
    float2 rescale = reconstruct1;
    float4 params = reconstruct2;
    float2 zs = float2((backgroundZ * rescale.x) + rescale.y, meshZ);
    float2 depth = ((zs * params.w) - params.y.xx) / (params.x.xx - (zs * params.z));
    return min(max((depth.y - depth.x) / _distance, 0.0f), 1.0f);
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

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _263_uvDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(g_uvDistortionTexture, g_uvDistortionSampler, param_1, param_2);
    UVOffset *= _263_uvDistortionParameter.x;
    float4 Output = Input.Color * g_texture.Sample(g_sampler, Input.UV + UVOffset);
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, g_texture, g_sampler, _263_flipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 screenPos = Input.PosP / Input.PosP.w.xxxx;
    float2 screenUV = (screenPos.xy + 1.0f.xx) / 2.0f.xx;
    screenUV.y = 1.0f - screenUV.y;
    float backgroundZ = g_depthTexture.Sample(g_depthSampler, screenUV).x;
    if (_263_softParticleAndReconstructionParam1.x != 0.0f)
    {
        float param_5 = backgroundZ;
        float param_6 = screenPos.z;
        float param_7 = _263_softParticleAndReconstructionParam1.x;
        float2 param_8 = _263_softParticleAndReconstructionParam1.yz;
        float4 param_9 = _263_reconstructionParam2;
        Output.w *= SoftParticle(param_5, param_6, param_7, param_8, param_9);
    }
    float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_10 = advancedParam.BlendUVDistortionUV;
    float2 param_11 = _263_uvDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(g_blendUVDistortionTexture, g_blendUVDistortionSampler, param_10, param_11);
    BlendUVOffset.y = _263_uvDistortionParameter.z + (_263_uvDistortionParameter.w * BlendUVOffset.y);
    BlendUVOffset *= _263_uvDistortionParameter.y;
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_12 = Output;
    ApplyTextureBlending(param_12, BlendTextureColor, _263_blendTextureParameter.x);
    Output = param_12;
    float3 _444 = Output.xyz * _263_emissiveScaling.x;
    Output = float4(_444.x, _444.y, _444.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _474 = lerp(_263_edgeColor.xyz * _263_edgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _263_edgeParameter.x).xxx);
    Output = float4(_474.x, _474.y, _474.z, Output.w);
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.PosVS = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.PosP = Input_PosP;
    Input.PosU = Input_PosU;
    Input.PosR = Input_PosR;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    float4 _516 = _main(Input);
    _entryPointOutput = _516;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_PosP = stage_input.Input_PosP;
    Input_PosU = stage_input.Input_PosU;
    Input_PosR = stage_input.Input_PosR;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    Input_Others = stage_input.Input_Others;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
