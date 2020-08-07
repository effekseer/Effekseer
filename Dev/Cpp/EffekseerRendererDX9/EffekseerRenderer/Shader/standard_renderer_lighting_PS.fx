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

cbuffer PS_ConstanBuffer : register(b0)
{
    float4 _210_fLightDirection : register(c0);
    float4 _210_fLightColor : register(c1);
    float4 _210_fLightAmbient : register(c2);
    float4 _210_fFlipbookParameter : register(c3);
    float4 _210_fUVDistortionParameter : register(c4);
    float4 _210_fBlendTextureParameter : register(c5);
    float4 _210_fEmissiveScaling : register(c6);
    float4 _210_fEdgeColor : register(c7);
    float4 _210_fEdgeParameter : register(c8);
};

uniform sampler2D Sampler_g_uvDistortionSampler : register(s3);
uniform sampler2D Sampler_g_normalSampler : register(s1);
uniform sampler2D Sampler_g_colorSampler : register(s0);
uniform sampler2D Sampler_g_alphaSampler : register(s2);
uniform sampler2D Sampler_g_blendUVDistortionSampler : register(s6);
uniform sampler2D Sampler_g_blendSampler : register(s4);
uniform sampler2D Sampler_g_blendAlphaSampler : register(s5);

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
    float4 gl_FragCoord : VPOS;
};

struct SPIRV_Cross_Output
{
    float4 _entryPointOutput : COLOR0;
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

float2 UVDistortionOffset(float2 uv, float2 uvInversed, sampler2D SPIRV_Cross_Combinedts)
{
    float2 UVOffset = (tex2D(SPIRV_Cross_Combinedts, uv).xy * 2.0f) - 1.0f.xx;
    UVOffset.y *= (-1.0f);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

void ApplyFlipbook(inout float4 dst, float4 flipbookParameter, float4 vcolor, float2 nextUV, float flipbookRate, sampler2D SPIRV_Cross_Combinedts)
{
    if (flipbookParameter.x > 0.0f)
    {
        float4 NextPixelColor = tex2D(SPIRV_Cross_Combinedts, nextUV) * vcolor;
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
        float3 _85 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_85.x, _85.y, _85.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _97 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_97.x, _97.y, _97.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _110 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_110.x, _110.y, _110.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _123 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_123.x, _123.y, _123.z, dstColor.w);
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
    float2 param_2 = _210_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_g_uvDistortionSampler);
    UVOffset *= _210_fUVDistortionParameter.x;
    float diffuse = 1.0f;
    float3 loN = tex2D(Sampler_g_normalSampler, Input.UV + UVOffset).xyz;
    float3 texNormal = (loN - 0.5f.xxx) * 2.0f;
    float3 localNormal = normalize(mul(texNormal, float3x3(float3(Input.WorldT), float3(Input.WorldB), float3(Input.WorldN))));
    diffuse = max(dot(_210_fLightDirection.xyz, localNormal), 0.0f);
    float4 Output = tex2D(Sampler_g_colorSampler, Input.UV + UVOffset) * Input.VColor;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, _210_fFlipbookParameter, Input.VColor, advancedParam.FlipbookNextIndexUV + UVOffset, param_4, Sampler_g_colorSampler);
    Output = param_3;
    float4 AlphaTexColor = tex2D(Sampler_g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = _210_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(param_5, param_6, Sampler_g_blendUVDistortionSampler);
    BlendUVOffset *= _210_fUVDistortionParameter.y;
    float4 BlendTextureColor = tex2D(Sampler_g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = tex2D(Sampler_g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _210_fBlendTextureParameter.x);
    Output = param_7;
    float3 _378 = Output.xyz * _210_fEmissiveScaling.x;
    Output = float4(_378.x, _378.y, _378.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _403 = Output.xyz * (float3(diffuse, diffuse, diffuse) + float3(_210_fLightAmbient.xyz));
    Output = float4(_403.x, _403.y, _403.z, Output.w);
    float3 _424 = lerp(_210_fEdgeColor.xyz * _210_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _210_fEdgeParameter.x).xxx);
    Output = float4(_424.x, _424.y, _424.z, Output.w);
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
    float4 _470 = _main(Input);
    _entryPointOutput = _470;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
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
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
