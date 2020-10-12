struct PS_Input
{
    float4 Position;
    float4 Color;
    float2 UV;
    float4 Pos;
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
    float4 _209_g_scale : packoffset(c0);
    float4 _209_mUVInversedBack : packoffset(c1);
    float4 _209_flipbookParameter : packoffset(c2);
    float4 _209_uvDistortionParameter : packoffset(c3);
    float4 _209_blendTextureParameter : packoffset(c4);
};

Texture2D<float4> g_uvDistortionTexture : register(t3);
SamplerState g_uvDistortionSampler : register(s3);
Texture2D<float4> g_texture : register(t0);
SamplerState g_sampler : register(s0);
Texture2D<float4> g_alphaTexture : register(t2);
SamplerState g_alphaSampler : register(s2);
Texture2D<float4> g_blendUVDistortionTexture : register(t6);
SamplerState g_blendUVDistortionSampler : register(s6);
Texture2D<float4> g_blendTexture : register(t4);
SamplerState g_blendSampler : register(s4);
Texture2D<float4> g_blendAlphaTexture : register(t5);
SamplerState g_blendAlphaSampler : register(s5);
Texture2D<float4> g_backTexture : register(t1);
SamplerState g_backSampler : register(s1);

static float4 gl_FragCoord;
static float4 Input_Color;
static float2 Input_UV;
static float4 Input_Pos;
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
    float4 Input_Pos : TEXCOORD2;
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
    float2 param_2 = _209_uvDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(g_uvDistortionTexture, g_uvDistortionSampler, param_1, param_2);
    UVOffset *= _209_uvDistortionParameter.x;
    float4 Output = g_texture.Sample(g_sampler, Input.UV + UVOffset);
    Output.w *= Input.Color.w;
    float4 param_3 = Output;
    float param_4 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_3, g_texture, g_sampler, _209_flipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_4);
    Output = param_3;
    float4 AlphaTexColor = g_alphaTexture.Sample(g_alphaSampler, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_5 = advancedParam.BlendUVDistortionUV;
    float2 param_6 = _209_uvDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(g_blendUVDistortionTexture, g_blendUVDistortionSampler, param_5, param_6);
    BlendUVOffset *= _209_uvDistortionParameter.y;
    float4 BlendTextureColor = g_blendTexture.Sample(g_blendSampler, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendAlphaTextureColor = g_blendAlphaTexture.Sample(g_blendAlphaSampler, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_7 = Output;
    ApplyTextureBlending(param_7, BlendTextureColor, _209_blendTextureParameter.x);
    Output = param_7;
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float2 pos = Input.Pos.xy / Input.Pos.w.xx;
    float2 posU = Input.PosU.xy / Input.PosU.w.xx;
    float2 posR = Input.PosR.xy / Input.PosR.w.xx;
    float xscale = (((Output.x * 2.0f) - 1.0f) * Input.Color.x) * _209_g_scale.x;
    float yscale = (((Output.y * 2.0f) - 1.0f) * Input.Color.y) * _209_g_scale.x;
    float2 uv = (pos + ((posR - pos) * xscale)) + ((posU - pos) * yscale);
    uv.x = (uv.x + 1.0f) * 0.5f;
    uv.y = 1.0f - ((uv.y + 1.0f) * 0.5f);
    uv.y = _209_mUVInversedBack.x + (_209_mUVInversedBack.y * uv.y);
    float3 color = float3(g_backTexture.Sample(g_backSampler, uv).xyz);
    Output = float4(color.x, color.y, color.z, Output.w);
    return Output;
}

void frag_main()
{
    PS_Input Input;
    Input.Position = gl_FragCoord;
    Input.Color = Input_Color;
    Input.UV = Input_UV;
    Input.Pos = Input_Pos;
    Input.PosU = Input_PosU;
    Input.PosR = Input_PosR;
    Input.Alpha_Dist_UV = Input_Alpha_Dist_UV;
    Input.Blend_Alpha_Dist_UV = Input_Blend_Alpha_Dist_UV;
    Input.Blend_FBNextIndex_UV = Input_Blend_FBNextIndex_UV;
    Input.Others = Input_Others;
    float4 _467 = _main(Input);
    _entryPointOutput = _467;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord;
    Input_Color = stage_input.Input_Color;
    Input_UV = stage_input.Input_UV;
    Input_Pos = stage_input.Input_Pos;
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
