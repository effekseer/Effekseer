struct PS_Input
{
    float4 Position;
    float4 VColor;
    float2 UV;
    float3 WorldP;
    float3 WorldN;
    float3 WorldT;
    float3 WorldB;
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
    float4 _187_fLightDirection : packoffset(c0);
    float4 _187_fLightColor : packoffset(c1);
    float4 _187_fLightAmbient : packoffset(c2);
    float4 _187_fFlipbookParameter : packoffset(c3);
    float4 _187_fUVDistortionParameter : packoffset(c4);
    float4 _187_fBlendTextureParameter : packoffset(c5);
    float4 _187_fEmissiveScaling : packoffset(c6);
    float4 _187_fEdgeColor : packoffset(c7);
    float4 _187_fEdgeParameter : packoffset(c8);
};

Texture2D<float4> g_uvDistortionTexture : register(t3);
SamplerState g_uvDistortionSampler : register(s3);
Texture2D<float4> g_normalTexture : register(t1);
SamplerState g_normalSampler : register(s1);
Texture2D<float4> g_colorTexture : register(t0);
SamplerState g_colorSampler : register(s0);
Texture2D<float4> g_alphaTexture : register(t2);
SamplerState g_alphaSampler : register(s2);
Texture2D<float4> g_blendUVDistortionTexture : register(t6);
SamplerState g_blendUVDistortionSampler : register(s6);
Texture2D<float4> g_blendTexture : register(t4);
SamplerState g_blendSampler : register(s4);
Texture2D<float4> g_blendAlphaTexture : register(t5);
SamplerState g_blendAlphaSampler : register(s5);

static float4 gl_FragCoord;
static float4 Input_VColor;
static float2 Input_UV;
static float3 Input_WorldP;
static float3 Input_WorldN;
static float3 Input_WorldT;
static float3 Input_WorldB;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float2 Input_Others;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    float4 Input_VColor : TEXCOORD0;
    float2 Input_UV : TEXCOORD1;
    float3 Input_WorldP : TEXCOORD2;
    float3 Input_WorldN : TEXCOORD3;
    float3 Input_WorldT : TEXCOORD4;
    float3 Input_WorldB : TEXCOORD5;
    float4 Input_Alpha_Dist_UV : TEXCOORD6;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD7;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD8;
    float2 Input_Others : TEXCOORD9;
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
        float3 _77 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_77.x, _77.y, _77.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _89 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_89.x, _89.y, _89.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _102 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_102.x, _102.y, _102.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _115 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_115.x, _115.y, _115.z, dstColor.w);
                }
            }
        }
    }
}

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 UVOffset = 0.0f.xx;
    UVOffset = (g_uvDistortionTexture.Sample(g_uvDistortionSampler, advancedParam.UVDistortionUV).xy * 2.0f) - 1.0f.xx;
    UVOffset *= _187_fUVDistortionParameter.x;
    float diffuse = 1.0f;
    float3 loN = g_normalTexture.Sample(g_normalSampler, Input.UV + UVOffset).xyz;
    float3 texNormal = (loN - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    diffuse = max(dot(_187_fLightDirection.xyz, localNormal), 0.0f);
    float4 Output = g_colorTexture.Sample(g_colorSampler, Input.UV + UVOffset) * Input.VColor;
    float4 param_1 = Output;
    float param_2 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_1, g_colorTexture, g_colorSampler, _187_fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, param_2);
    Output = param_1;
    float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 BlendUVOffset = (g_blendUVDistortionTexture.Sample(g_blendUVDistortionSampler, advancedParam.BlendUVDistortionUV).xy * 2.0f) - 1.0f.xx;
    BlendUVOffset *= _187_fUVDistortionParameter.y;
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_3 = Output;
    ApplyTextureBlending(param_3, BlendTextureColor, _187_fBlendTextureParameter.x);
    Output = param_3;
    float3 _347 = Output.xyz * _187_fEmissiveScaling.x;
    Output = float4(_347.x, _347.y, _347.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _372 = Output.xyz * (float3(diffuse, diffuse, diffuse) + float3(_187_fLightAmbient.xyz));
    Output = float4(_372.x, _372.y, _372.z, Output.w);
    float3 _393 = lerp(_187_fEdgeColor.xyz * _187_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _187_fEdgeParameter.x).xxx);
    Output = float4(_393.x, _393.y, _393.z, Output.w);
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.VColor = Input_VColor;
    Input.UV = Input_UV;
    Input.WorldP = Input_WorldP;
    Input.WorldN = Input_WorldN;
    Input.WorldT = Input_WorldT;
    Input.WorldB = Input_WorldB;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    float4 _439 = _main(Input);
    _entryPointOutput = _439;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_VColor = stage_input.Input_VColor;
    Input_UV = stage_input.Input_UV;
    Input_WorldP = stage_input.Input_WorldP;
    Input_WorldN = stage_input.Input_WorldN;
    Input_WorldT = stage_input.Input_WorldT;
    Input_WorldB = stage_input.Input_WorldB;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    Input_Others = stage_input.Input_Others;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = _entryPointOutput;
    return stage_output;
}
