struct PS_Input
{
    float4 PosVS;
    float4 Color;
    float4 UV_Others;
    float3 WorldN;
    float4 Alpha_Dist_UV;
    float4 Blend_Alpha_Dist_UV;
    float4 Blend_FBNextIndex_UV;
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
    float4 _284_fLightDirection : register(c0);
    float4 _284_fLightColor : register(c1);
    float4 _284_fLightAmbient : register(c2);
    float4 _284_fFlipbookParameter : register(c3);
    float4 _284_fUVDistortionParameter : register(c4);
    float4 _284_fBlendTextureParameter : register(c5);
    float4 _284_fCameraFrontDirection : register(c6);
    float4 _284_fFalloffParameter : register(c7);
    float4 _284_fFalloffBeginColor : register(c8);
    float4 _284_fFalloffEndColor : register(c9);
    float4 _284_fEmissiveScaling : register(c10);
    float4 _284_fEdgeColor : register(c11);
    float4 _284_fEdgeParameter : register(c12);
    float4 _284_softParticleParam : register(c13);
    float4 _284_reconstructionParam1 : register(c14);
    float4 _284_reconstructionParam2 : register(c15);
    float4 _284_mUVInversedBack : register(c16);
    float4 _284_miscFlags : register(c17);
};

uniform sampler2D Sampler_sampler_uvDistortionTex : register(s2);
uniform sampler2D Sampler_sampler_colorTex : register(s0);
uniform sampler2D Sampler_sampler_alphaTex : register(s1);
uniform sampler2D Sampler_sampler_blendUVDistortionTex : register(s5);
uniform sampler2D Sampler_sampler_blendTex : register(s3);
uniform sampler2D Sampler_sampler_blendAlphaTex : register(s4);

static float4 gl_FragCoord;
static float4 Input_Color;
static float4 Input_UV_Others;
static float3 Input_WorldN;
static float4 Input_Alpha_Dist_UV;
static float4 Input_Blend_Alpha_Dist_UV;
static float4 Input_Blend_FBNextIndex_UV;
static float4 _entryPointOutput;

struct SPIRV_Cross_Input
{
    centroid float4 Input_Color : TEXCOORD0;
    centroid float4 Input_UV_Others : TEXCOORD1;
    float3 Input_WorldN : TEXCOORD2;
    float4 Input_Alpha_Dist_UV : TEXCOORD3;
    float4 Input_Blend_Alpha_Dist_UV : TEXCOORD4;
    float4 Input_Blend_FBNextIndex_UV : TEXCOORD5;
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
    ret.FlipbookRate = psinput.UV_Others.z;
    ret.AlphaThreshold = psinput.UV_Others.w;
    return ret;
}

float2 UVDistortionOffset(float2 uv, float2 uvInversed, sampler2D SPIRV_Cross_Combinedts)
{
    float2 UVOffset = (tex2D(SPIRV_Cross_Combinedts, uv).xy * 2.0f) - 1.0f.xx;
    UVOffset.y *= (-1.0f);
    UVOffset.y = uvInversed.x + (uvInversed.y * UVOffset.y);
    return UVOffset;
}

float3 PositivePow(float3 base, float3 power)
{
    return pow(max(abs(base), 1.1920928955078125e-07f.xxx), power);
}

float3 LinearToSRGB(float3 c)
{
    float3 param = c;
    float3 param_1 = 0.4166666567325592041015625f.xxx;
    return max((PositivePow(param, param_1) * 1.05499994754791259765625f) - 0.054999999701976776123046875f.xxx, 0.0f.xxx);
}

float4 LinearToSRGB(float4 c)
{
    float3 param = c.xyz;
    return float4(LinearToSRGB(param), c.w);
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
        float3 _108 = (blendColor.xyz * blendColor.w) + (dstColor.xyz * (1.0f - blendColor.w));
        dstColor = float4(_108.x, _108.y, _108.z, dstColor.w);
    }
    else
    {
        if (blendType == 1.0f)
        {
            float3 _120 = dstColor.xyz + (blendColor.xyz * blendColor.w);
            dstColor = float4(_120.x, _120.y, _120.z, dstColor.w);
        }
        else
        {
            if (blendType == 2.0f)
            {
                float3 _133 = dstColor.xyz - (blendColor.xyz * blendColor.w);
                dstColor = float4(_133.x, _133.y, _133.z, dstColor.w);
            }
            else
            {
                if (blendType == 3.0f)
                {
                    float3 _146 = dstColor.xyz * (blendColor.xyz * blendColor.w);
                    dstColor = float4(_146.x, _146.y, _146.z, dstColor.w);
                }
            }
        }
    }
}

float3 SRGBToLinear(float3 c)
{
    return c * ((c * ((c * 0.305306017398834228515625f) + 0.6821711063385009765625f.xxx)) + 0.01252287812530994415283203125f.xxx);
}

float4 SRGBToLinear(float4 c)
{
    float3 param = c.xyz;
    return float4(SRGBToLinear(param), c.w);
}

float4 ConvertToScreen(float4 c)
{
    if (_284_miscFlags.x == 0.0f)
    {
        return c;
    }
    float4 param = c;
    return SRGBToLinear(param);
}

float4 _main(PS_Input Input)
{
    PS_Input param = Input;
    AdvancedParameter advancedParam = DisolveAdvancedParameter(param);
    float2 param_1 = advancedParam.UVDistortionUV;
    float2 param_2 = _284_fUVDistortionParameter.zw;
    float2 UVOffset = UVDistortionOffset(param_1, param_2, Sampler_sampler_uvDistortionTex);
    UVOffset *= _284_fUVDistortionParameter.x;
    float4 param_3 = tex2D(Sampler_sampler_colorTex, Input.UV_Others.xy + UVOffset);
    float4 Output = LinearToSRGB(param_3) * Input.Color;
    float4 param_4 = Output;
    float param_5 = advancedParam.FlipbookRate;
    ApplyFlipbook(param_4, _284_fFlipbookParameter, Input.Color, advancedParam.FlipbookNextIndexUV + UVOffset, param_5, Sampler_sampler_colorTex);
    Output = param_4;
    float4 AlphaTexColor = tex2D(Sampler_sampler_alphaTex, advancedParam.AlphaUV + UVOffset);
    Output.w *= (AlphaTexColor.x * AlphaTexColor.w);
    float2 param_6 = advancedParam.BlendUVDistortionUV;
    float2 param_7 = _284_fUVDistortionParameter.zw;
    float2 BlendUVOffset = UVDistortionOffset(param_6, param_7, Sampler_sampler_blendUVDistortionTex);
    BlendUVOffset *= _284_fUVDistortionParameter.y;
    float4 param_8 = tex2D(Sampler_sampler_blendTex, advancedParam.BlendUV + BlendUVOffset);
    float4 BlendTextureColor = LinearToSRGB(param_8);
    float4 BlendAlphaTextureColor = tex2D(Sampler_sampler_blendAlphaTex, advancedParam.BlendAlphaUV + BlendUVOffset);
    BlendTextureColor.w *= (BlendAlphaTextureColor.x * BlendAlphaTextureColor.w);
    float4 param_9 = Output;
    ApplyTextureBlending(param_9, BlendTextureColor, _284_fBlendTextureParameter.x);
    Output = param_9;
    if (_284_fFalloffParameter.x == 1.0f)
    {
        float3 cameraVec = normalize(-_284_fCameraFrontDirection.xyz);
        float CdotN = clamp(dot(cameraVec, normalize(Input.WorldN)), 0.0f, 1.0f);
        float4 FalloffBlendColor = lerp(_284_fFalloffEndColor, _284_fFalloffBeginColor, pow(CdotN, _284_fFalloffParameter.z).xxxx);
        if (_284_fFalloffParameter.y == 0.0f)
        {
            float3 _469 = Output.xyz + FalloffBlendColor.xyz;
            Output = float4(_469.x, _469.y, _469.z, Output.w);
        }
        else
        {
            if (_284_fFalloffParameter.y == 1.0f)
            {
                float3 _482 = Output.xyz - FalloffBlendColor.xyz;
                Output = float4(_482.x, _482.y, _482.z, Output.w);
            }
            else
            {
                if (_284_fFalloffParameter.y == 2.0f)
                {
                    float3 _495 = Output.xyz * FalloffBlendColor.xyz;
                    Output = float4(_495.x, _495.y, _495.z, Output.w);
                }
            }
        }
        Output.w *= FalloffBlendColor.w;
    }
    float3 _509 = Output.xyz * _284_fEmissiveScaling.x;
    Output = float4(_509.x, _509.y, _509.z, Output.w);
    if (Output.w <= max(0.0f, advancedParam.AlphaThreshold))
    {
        discard;
    }
    float3 _541 = lerp(_284_fEdgeColor.xyz * _284_fEdgeParameter.y, Output.xyz, ceil((Output.w - advancedParam.AlphaThreshold) - _284_fEdgeParameter.x).xxx);
    Output = float4(_541.x, _541.y, _541.z, Output.w);
    float4 param_10 = Output;
    return ConvertToScreen(param_10);
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
    float4 _576 = _main(Input);
    _entryPointOutput = _576;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    gl_FragCoord = stage_input.gl_FragCoord + float4(0.5f, 0.5f, 0.0f, 0.0f);
    Input_Color = stage_input.Input_Color;
    Input_UV_Others = stage_input.Input_UV_Others;
    Input_WorldN = stage_input.Input_WorldN;
    Input_Alpha_Dist_UV = stage_input.Input_Alpha_Dist_UV;
    Input_Blend_Alpha_Dist_UV = stage_input.Input_Blend_Alpha_Dist_UV;
    Input_Blend_FBNextIndex_UV = stage_input.Input_Blend_FBNextIndex_UV;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output._entryPointOutput = float4(_entryPointOutput);
    return stage_output;
}
